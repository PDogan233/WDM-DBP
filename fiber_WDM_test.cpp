// fiber_WDM_test.cpp
//
// A simple test program that builds a WDM composite signal (16-QAM),
// propagates it through FiberWDM (multiple spans), and demodulates per-channel.
// Simplifications:
//  - Rectangular pulse shaping (also no match filter)
//  - No DBP
//  - ASE noise set to zero for initial testing
//
// Make sure to link with FFTW and to compile fiber_WDM.cpp into the project.

#include "fiber_WDM.h"
#include <random>
#include <iostream>
#include <cmath>
#include <complex>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <fstream>
#include <iomanip>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using VecReal = std::vector<double>;
using VecComplex = std::vector<std::complex<double>>;
using Complex = std::complex<double>;

// -----------------------------
// Helper utilities
// -----------------------------
inline int nextpow2_int(int x) {
    if (x <= 1) return 0;
    int p = 0;
    int v = 1;
    while (v < x) { v <<= 1; ++p; }
    return p;
}

static VecReal make_time_vector(int Nt, double fs) {
    VecReal t(Nt);
    double dt = 1.0 / fs;
    for (int i = 0; i < Nt; ++i) t[i] = i * dt;
    return t;
}

// 16-QAM modulation (4 bits -> 1 symbol). Use mapping similar to MATLAB rectangular grid.
// bits length must be Nsym*4
static std::vector<Complex> modulate_16qam(const std::vector<int>& bits, int Nsym) {
    assert(static_cast<int>(bits.size()) == Nsym * 4);
    std::vector<Complex> symbols(Nsym);
    auto bits_to_level = [](int b0, int b1) -> double {
        // map pairs -> {-3,-1,1,3} using Gray-like mapping
        int val = (b0 << 1) | b1;
        switch (val) {
        case 0: return -3.0; // 00
        case 1: return -1.0; // 01
        case 3: return  1.0; // 11
        case 2: return  3.0; // 10
        default: return 0.0;
        }
        };
    for (int i = 0; i < Nsym; ++i) {
        int idx = 4 * i;
        double re = bits_to_level(bits[idx + 0], bits[idx + 1]);
        double im = bits_to_level(bits[idx + 2], bits[idx + 3]);
        symbols[i] = Complex(re, im);
    }
    return symbols;
}

// 16-QAM demodulation -> bits (nearest decision to levels {-3,-1,1,3})
static std::vector<int> demodulate_16qam(const std::vector<Complex>& symbols) {
    int N = static_cast<int>(symbols.size());
    std::vector<int> bits;
    bits.reserve(N * 4);
    auto level_to_bits = [](double v) -> std::pair<int, int> {
        if (v < -2.0) return { 0,0 }; // -3
        if (v < 0.0)  return { 0,1 }; // -1
        if (v < 2.0)  return { 1,1 }; // 1
        return { 1,0 };               // 3
        };
    for (int i = 0; i < N; ++i) {
        double re = symbols[i].real();
        double im = symbols[i].imag();
        auto pr = level_to_bits(re);
        auto pi = level_to_bits(im);
        bits.push_back(pr.first);
        bits.push_back(pr.second);
        bits.push_back(pi.first);
        bits.push_back(pi.second);
    }
    return bits;
}

// Upsample & rectangular pulse shaping: each symbol repeated sps times
static VecComplex upsample_and_rect(const std::vector<Complex>& symbols, int sps) {
    int Nsym = static_cast<int>(symbols.size());
    VecComplex out(Nsym * sps);
    for (int i = 0; i < Nsym; ++i) {
        for (int j = 0; j < sps; ++j) out[i * sps + j] = symbols[i];
    }
    return out;
}

// average power (mean |x|^2)
static double avg_power(const VecComplex& x) {
    double s = 0.0;
    for (const auto& c : x) s += std::norm(c);
    return s / static_cast<double>(x.size());
}

// Add AWGN noise (complex) with variance per complex sample = noise_var
static void add_awgn(VecComplex& x, double noise_var_per_complex, std::mt19937& rng) {
    if (noise_var_per_complex <= 0.0) return;
    std::normal_distribution<double> nd(0.0, std::sqrt(noise_var_per_complex / 2.0));
    for (size_t i = 0; i < x.size(); ++i) {
        double nr = nd(rng);
        double ni = nd(rng);
        x[i] += Complex(nr, ni);
    }
}

static void save_complex_vector_csv(const std::string& fname, const VecComplex& v) {
    std::ofstream ofs(fname);
    ofs << "real,imag\n";
    ofs << std::setprecision(12);
    for (size_t i = 0; i < v.size(); ++i) {
        ofs << v[i].real() << "," << v[i].imag() << "\n";
    }
    ofs.close();
}

static void save_bits_csv(const std::string& fname, const std::vector<int>& bits) {
    std::ofstream ofs(fname);
    ofs << "bit\n";
    for (size_t i = 0; i < bits.size(); ++i) ofs << bits[i] << "\n";
    ofs.close();
}

// Save spectrum (magnitude) computed via FiberWDM::fft + fftshift to CSV with freq column
static void save_spectrum_csv(const std::string& fname, const VecComplex& sig, double df) {
    VecComplex S = FiberWDM::fft(sig);
    VecComplex Ss = FiberWDM::fftshift(S);
    int N = static_cast<int>(Ss.size());
    std::ofstream ofs(fname);
    ofs << "freq_Hz,mag\n";
    for (int i = 0; i < N; ++i) {
        double freq = (static_cast<double>(i) - static_cast<double>(N) / 2.0) * df;
        ofs << std::setprecision(12) << freq << "," << std::abs(Ss[i]) << "\n";
    }
    ofs.close();
}

// -----------------------------
// Main test program
// -----------------------------
int main() {
    // Parameters 
    int Nsym = 1 << 13; // adjust to smaller for quick tests if needed
    int M = 16;
    int k = 4; // bits per symbol
    double Rs = 16e9; // symbol rate (Hz)
    double rrc_rolloff = 0.20; // we use just for bandwidth estimate
    double B_ch = Rs * (1.0 + rrc_rolloff);

    // WDM layout
    int Nch = 3;
    double DeltaF = 50e9; // channel spacing
    std::vector<int> ch_idx;
    for (int m = 0; m < Nch; ++m) {
        ch_idx.push_back(m - (Nch - 1) / 2); // symmetric around center
    }
    double margin = 0.1 * B_ch;
    if (DeltaF < B_ch + margin) DeltaF = B_ch + margin;
    double B_wdm = (Nch - 1) * DeltaF + B_ch;

    // Sampling
    double guard_factor = 2.0;
    double fs_target = B_wdm * (1.0 + guard_factor);
    int sps = 1*static_cast<int>(std::ceil(fs_target / Rs));
    double fs = sps * Rs;
    int Nt = 1 << nextpow2_int(Nsym * sps); // Nt = 2^nextpow2(Nsym * sps)
    double dt = 1.0 / fs;
    double df = fs / static_cast<double>(Nt);
    VecReal t = make_time_vector(Nt, fs);

    std::cout << "Nsym=" << Nsym << ", sps=" << sps << ", Nt=" << Nt << ", fs=" << fs << "\n";

    // Physical parameters
    double h_planck = 6.62607015e-34;
    double lambda0 = 1550e-9;
    double c = 299792458.0;
    double nu0 = c / lambda0;

    double L_span = 100e3; // m
    int Nspans = 1;
    double alpha_dBpm = 0.2e-3; // dB/m
    double GroupRef = 1.47;
    double Dispersion = 1*16.5e-6;
    double Dis_S = 1*0.08e3;
    double n2 = 1*2.6e-20;
    double Aeff = 80e-12;

    // NLSE params will be computed inside FiberWDM

    // Launch power
    double Pdbm_ch = 0.0;
    double PinW_ch = std::pow(10.0, (Pdbm_ch - 30.0) / 10.0); // Watts

    // Amplifier parameters
    double G_dB = alpha_dBpm * L_span; // simple gain to compensate loss per span
    double G_lin = std::pow(10.0, G_dB / 10.0);
    double NF_dB = 5.0;
    double nsp = 0.5 * std::pow(10.0, NF_dB / 10.0);

    // Step control parameters
    std::string method = "global_error"; // options: "constant", "local_error", "global_error"
    double control_param;
    if (method == "global_error") {
        control_param = 1e-1; // GE_target
    }
    else if (method == "local_error") {
        control_param = 1e-3; // LE_target
    }
    else {
        control_param = 1e3;  // dz (fallback constant step)
    }
    double dz = 2e3; // backup SSF step


    // Create FiberWDM object
    FiberWDM fiber(L_span, alpha_dBpm, GroupRef, Dispersion, Dis_S,
        n2, Aeff, lambda0, Nt, fs, method, control_param);

    // Prepare containers
    VecComplex E_total(Nt, Complex(0.0, 0.0));
    std::vector<double> avg_pow_tx_up(Nch, 0.0);
    std::vector< std::vector<int> > tx_bits(Nch);
    std::vector< std::vector<Complex> > tx_symbols(Nch);
    std::vector< VecComplex > tx_wave(Nch, VecComplex(Nt, Complex(0.0, 0.0)));

    // RNG
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> bit_dist(0, 1);

    // ---------- TX: generate WDM composite signal (rectangular pulse shaping) ----------
    for (int m = 0; m < Nch; ++m) {
        // bits
        std::vector<int> bits(Nsym * k);
        for (int i = 0; i < Nsym * k; ++i) bits[i] = bit_dist(rng);
        tx_bits[m] = bits;

        // symbols
        std::vector<Complex> symbols = modulate_16qam(bits, Nsym);
        tx_symbols[m] = symbols;

        // upsample & rect-shape
        VecComplex shaped = upsample_and_rect(symbols, sps);

        // avg power
        double ap = avg_power(shaped);
        avg_pow_tx_up[m] = ap;
        double scale = 1.0;
        if (ap > 0.0) scale = std::sqrt(PinW_ch / ap);
        for (auto& v : shaped) v *= scale;

        // zero-pad into Nt vector
        for (int i = 0; i < Nt; ++i) {
            if (i < static_cast<int>(shaped.size())) tx_wave[m][i] = shaped[i];
            else tx_wave[m][i] = Complex(0.0, 0.0);
        }

        // frequency shift and add to E_total
        double fch = static_cast<double>(ch_idx[m]) * DeltaF;
        for (int i = 0; i < Nt; ++i) {
            double phase = 2.0 * M_PI * fch * t[i];
            Complex ph = Complex(std::cos(phase), std::sin(phase));
            E_total[i] += tx_wave[m][i] * ph;
        }

        std::cout << "[TX] channel " << m << " avg_power(before norm)=" << ap
            << " scale=" << scale << "\n";
    }

    // ------ Save TX data per channel for debugging ------
    for (int m = 0; m < Nch; ++m) {
        // tx_wave: full-length time-domain (Nt samples) complex waveform
        std::string fn_wave = "tx_wave_ch" + std::to_string(m) + ".csv";
        save_complex_vector_csv(fn_wave, tx_wave[m]);

        // tx_symbols: symbol sequence before upsampling (Nsym complex values)
        std::string fn_sym = "tx_symbols_ch" + std::to_string(m) + ".csv";
        save_complex_vector_csv(fn_sym, tx_symbols[m]);

        // tx_bits: bits (0/1)
        std::string fn_bits = "tx_bits_ch" + std::to_string(m) + ".csv";
        save_bits_csv(fn_bits, tx_bits[m]);

        std::cout << "[SAVE] TX channel " << m << " saved to: "
            << fn_wave << ", " << fn_sym << ", " << fn_bits << std::endl;
    }

    // Save TX composite spectrum for debugging
    save_complex_vector_csv("E_total.csv", E_total);
    save_spectrum_csv("spectrum_tx.csv", E_total, df);

    // ---------- Channel propagation: Nspans of SSF + EDFA ----------
    VecComplex E = E_total;
    for (int span = 0; span < Nspans; ++span) {
        // propagate whole WDM composite field through fiber span
        E = fiber.propagate(E, L_span, dz, control_param, method, B_wdm);

        // Apply amplifier gain and add ASE noise (we set noise=0 for now)
        // ASE_PSD = nsp * h_planck * nu0 * (G_lin - 1); // W/Hz
        // total noise power in band ~ ASE_PSD * fs/2 per quadrature -> add corresponding complex gaussian
        // For now, keep noise = 0 to simplify debugging
        VecComplex noise(Nt, Complex(0.0, 0.0));
        // add_awgn_noise(noise, noise_var, rng);

        // Apply field-level gain
        for (int i = 0; i < Nt; ++i) E[i] = std::sqrt(G_lin) * E[i] + noise[i];
    }

    // Save spectrum after propagation
    save_complex_vector_csv("E.csv", E);
    save_spectrum_csv("spectrum_after.csv", E, df);

    // ---------- RX: per-channel downconvert, matched filter, sample, demodulate ----------
    std::vector<double> BER(Nch, 0.0);

    // Containers to save RX data for debugging and later inspection
    std::vector< VecComplex > rx_wave(Nch, VecComplex());   // full downconverted waveform per channel
    std::vector< std::vector<Complex> > rx_symbols(Nch);    // sampled symbols per channel
    std::vector< std::vector<int> > rx_bits(Nch);           // decoded bits per channel

    // compute group delay in samples (but do NOT apply as startIdx)
    double group_delay_seconds = fiber.beta1 * static_cast<double>(Nspans) * L_span;
    int group_delay_samples = static_cast<int>(std::round(group_delay_seconds * fs));
    std::cout << "[INFO] group_delay_samples = " << group_delay_samples << "\n";

    int startIdx = sps/2; 

    // RX loop per channel
    for (int m = 0; m < Nch; ++m) {
        double fch = static_cast<double>(ch_idx[m]) * DeltaF;

        // downconvert to baseband (full-length)
        VecComplex r(Nt);
        for (int i = 0; i < Nt; ++i) {
            double phase = -2.0 * M_PI * fch * t[i];
            Complex ph = Complex(std::cos(phase), std::sin(phase));
            r[i] = E[i] * ph;
        }
        // store downconverted waveform
        rx_wave[m] = r;


        // Since TX used rectangular shaping, we directly sample at symbol instants:
        // sample indices: startIdx + n * sps, for n=0..Nsym-1, break if index >= Nt
        std::vector<Complex> y;
        y.reserve(Nsym);
        int idx = startIdx;
        for (int n = 0; n < Nsym; ++n) {
            if (idx >= Nt) break;
            y.push_back(r[idx]);
            idx += sps;
        }


        // scale back to transmitter amplitude (inverse of TX normalization)
        double inv_scale = 1.0;
        if (avg_pow_tx_up[m] > 0.0) inv_scale = std::sqrt(avg_pow_tx_up[m] / PinW_ch);
        for (auto& v : y) v *= inv_scale;

        // save sampled symbols (before scale)
        rx_symbols[m] = y;

        // demodulate sampled symbols into bits
        std::vector<int> bits_hat = demodulate_16qam(y);
        rx_bits[m] = bits_hat;

        // compute BER: compare available bits
        int nbits_hat = static_cast<int>(bits_hat.size());
        int nbits_tx = static_cast<int>(tx_bits[m].size());
        int nbits = std::min(nbits_hat, nbits_tx);

        // inside RX per-channel loop, after rx_wave[m], rx_symbols[m], rx_bits[m] are set:
        {
            std::string fn_rx_wave = "rx_wave_ch" + std::to_string(m) + ".csv";
            save_complex_vector_csv(fn_rx_wave, rx_wave[m]);

            std::string fn_rx_sym = "rx_symbols_ch" + std::to_string(m) + ".csv";
            save_complex_vector_csv(fn_rx_sym, rx_symbols[m]);

            std::string fn_rx_bits = "rx_bits_ch" + std::to_string(m) + ".csv";
            save_bits_csv(fn_rx_bits, rx_bits[m]);

            std::cout << "[SAVE] RX channel " << m << " saved to: "
                << fn_rx_wave << ", " << fn_rx_sym << ", " << fn_rx_bits << std::endl;
        }

        if (nbits == 0) {
            std::cout << "[WARN] Channel " << m << ": no bits available for BER calculation (nbits_hat="
                << nbits_hat << ", nbits_tx=" << nbits_tx << "). Setting BER=NaN\n";
            BER[m] = NAN;
        }
        else {
            int err = 0;
            for (int i = 0; i < nbits; ++i) {
                if (bits_hat[i] != tx_bits[m][i]) ++err;
            }
            BER[m] = static_cast<double>(err) / static_cast<double>(nbits);
            std::cout << "[RESULT] Channel " << m << " BER=" << BER[m]
                << " (errors=" << err << " / " << nbits << " bits)\n";
        }
    }

    std::cout << "Finished test. Saved TX/RX data and spectra to CSV files.\n";
    std::cout << "You can plot 'spectrum_tx.csv' and 'spectrum_after.csv', and inspect "
        "'rx_wave_ch#.csv', 'rx_symbols_ch#.csv', 'rx_bits_ch#.csv' for debug.\n";
    return 0;
}