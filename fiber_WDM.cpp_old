#include "fiber_WDM.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <fftw3.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cctype> // for std::tolower
#include <fstream>   // for saving CSV debug files
#include <iomanip>   // for formatting numeric output


/* -----------------------------------------------------------------------------
 * fiber_WDM.cpp
 *
 * Implementation of FiberWDM class.
 *
 *  - Create/destroy FFTW plan per call (clear, though not fastest).
 * -------------------------------------------------------------------------- */

 // -----------------------------
 // Constructor / Destructor
 // -----------------------------
FiberWDM::FiberWDM(double L_span_,
    double alpha_dBpm_,
    double GroupRef_,
    double Dispersion_,
    double Dis_S_,
    double n2_,
    double Aeff_,
    double lambda0_,
    int Nt_,
    double fs_,
    const std::string& step_method_,
    double control_param_)
    : L_span(L_span_),
    alpha_dBpm(alpha_dBpm_),
    GroupRef(GroupRef_),
    Dispersion(Dispersion_),
    Dis_S(Dis_S_),
    n2(n2_),
    Aeff(Aeff_),
    lambda0(lambda0_),
    Nt(Nt_),
    fs(fs_)
{
    // Basic sanity checks
    if (Nt <= 0) {
        throw std::runtime_error("Nt must be positive");
    }
    if (fs <= 0.0) {
        throw std::runtime_error("fs must be positive");
    }

    // Frequency spacing df = fs / Nt (use double division)
    df = fs / static_cast<double>(Nt);

    // Compute derived fiber parameters (beta, gamma, alpha) and frequency grid
    computeNLSEParameters();
    buildFrequencyGrid();
}

FiberWDM::~FiberWDM() {
    // Destructor: currently empty because we do not keep persistent system resources.
    // If you later allocate persistent FFTW plans or files, free them here.
    cleanup();
}

// -----------------------------
// computeNLSEParameters()
// Map physical fiber params to NLSE coefficients.
// -----------------------------
void FiberWDM::computeNLSEParameters() {
    const double c = 299792458.0; // speed of light [m/s]

    // beta1: group delay per meter (s/m) approximated by n_g / c
    beta1 = GroupRef / c;

    // beta2: related to dispersion parameter D [s/m^2]
    // beta2 = - Dispersion * lambda0^2 / (2*pi*c)
    beta2 = -Dispersion * (lambda0 * lambda0) / (2.0 * M_PI * c);

    // beta3: from dispersion slope Dis_S and D
    double factor = (lambda0 * lambda0) / (2.0 * M_PI * c);
    beta3 = factor * factor * (Dis_S + 2.0 * Dispersion / lambda0);

    // gamma: nonlinear coefficient (1/(W*m))
    gamma = 2.0 * M_PI * n2 / (lambda0 * Aeff);

    // alpha (dB/m) to Neper/m (power-level): alpha_np = ln(10^(alpha_dBpm/10))
    alpha_np = std::log(std::pow(10.0, alpha_dBpm / 10.0));

    // Print values for debug/verification (helpful for tests)
    std::cout << "[FiberWDM] NLSE params computed: beta1=" << beta1
        << " beta2=" << beta2 << " beta3=" << beta3
        << " gamma=" << gamma << " alpha_np=" << alpha_np << std::endl;
}

// -----------------------------
// buildFrequencyGrid()
// Build freq and omega arrays in fftshift ordering ([-fs/2 .. fs/2))
// Also precompute beta_omega (0.5*beta2*omega^2 + 1/6*beta3*omega^3)
// -----------------------------
void FiberWDM::buildFrequencyGrid() {
    freq.assign(Nt, 0.0);
    omega.assign(Nt, 0.0);
    beta_omega.assign(Nt, 0.0);

    int half = Nt / 2;
    for (int k = 0; k < Nt; ++k) {
        // index mapping for fftshift order: [-N/2, ..., N/2 - 1]
        int idx = k - half;
        freq[k] = static_cast<double>(idx) * df;
        omega[k] = 2.0 * M_PI * freq[k];
    }

    for (int i = 0; i < Nt; ++i) {
        double w = omega[i];
        beta_omega[i] = 0.5 * beta2 * (w * w) + (1.0 / 6.0) * beta3 * (w * w * w);
    }
}

// -----------------------------
// propagate()
// Main SSF loop implementing Strang splitting.
// -----------------------------
VecComplex FiberWDM::propagate(const VecComplex& x,
    double L,
    double dz,
    double control_param,
    const std::string& method,
    double B_wdm)
{
    if (static_cast<int>(x.size()) != Nt) {
        throw std::runtime_error("Input length must equal Nt");
    }

    VecComplex y = x; // mutable copy
    double z = 0.0;
    int step_count = 0;

    while (z < L - 1e-12) {
        ++step_count;
        double h = chooseStep(method, dz, L, z, control_param, y, gamma, beta2, B_wdm);
        if (h <= 0.0) {
            h = L - z;
        }
        if (z + h > L) {
            h = L - z;
        }

        // Informational print
        std::cout << "Step " << step_count << ": z( km ) = " << z / 1e3
            << ", h (m) = " << h << std::endl;

        // Strang symmetric split: NL(h/2) -> Disp(h) -> NL(h/2)
        y = applyNonlinearity(y, h / 2.0);
        y = applyDispersion(y, h);
        y = applyNonlinearity(y, h / 2.0);

        z += h;
    }

    return y;
}

// -----------------------------
// applyDispersion()
// Implements linear propagation in frequency domain:
//  X = fft(y_in)
//  Xs = fftshift(X)
//  Xs = Xs .* H   (H in fftshift order)
//  Xu = ifftshift(Xs)
//  y_out = ifft(Xu)
// where H = exp( (-alpha_np/2)*h - j * beta_omega * h )
// -----------------------------
VecComplex FiberWDM::applyDispersion(const VecComplex& y_in, double h) {
    // forward FFT (no normalization)
    VecComplex X = fft(y_in);

    // shift to symmetric freq order
    VecComplex Xs = fftshift(X);

    // --- DEBUG: compute H vector across the whole omega grid before applying ---
    // We compute H for each frequency bin: H[i] = exp( (-alpha_np/2)*h - j * beta_omega[i] * h )
    // and optionally save Xs and H to CSV for inspection the first time this function is called.
    static bool saved = false; // only save once to avoid creating many files
    if (!saved) {
        int N = Nt;
        std::vector<Complex> Hvec(N);
        for (int i = 0; i < N; ++i) {
            Hvec[i] = std::exp(Complex(-(alpha_np / 2.0) * h, -beta_omega[i] * h));
        }

        // Save Xs (fftshifted spectrum) to CSV
        std::ofstream ofsXs("applyDispersion_Xs.csv");
        ofsXs << "freq_Hz,real,imag,mag\n";
        ofsXs << std::setprecision(12);
        for (int i = 0; i < N; ++i) {
            double freq_i = freq[i]; // freq stored in fftshift order in class
            ofsXs << freq_i << "," << Xs[i].real() << "," << Xs[i].imag() << "," << std::abs(Xs[i]) << "\n";
        }
        ofsXs.close();

        // Save H vector (transfer function) to CSV
        std::ofstream ofsH("applyDispersion_H.csv");
        ofsH << "freq_Hz,real,imag,mag,phase\n";
        ofsH << std::setprecision(12);
        for (int i = 0; i < N; ++i) {
            double freq_i = freq[i];
            double mag = std::abs(Hvec[i]);
            double phase = std::arg(Hvec[i]);
            ofsH << freq_i << "," << Hvec[i].real() << "," << Hvec[i].imag() << "," << mag << "," << phase << "\n";
        }
        ofsH.close();

        std::cout << "[DEBUG] Saved applyDispersion_Xs.csv and applyDispersion_H.csv (first call)." << std::endl;
        saved = true;
    }


    // multiply by transfer function H (elementwise)
    VecComplex Xs_out(Nt);
    for (int i = 0; i < Nt; ++i) {
        // field-level attenuation uses alpha_np/2
        Complex H = std::exp(Complex(-(alpha_np / 2.0) * h, -beta_omega[i] * h));
        Xs_out[i] = Xs[i] * H;
    }

    // inverse shift and inverse FFT (with 1/N normalization)
    VecComplex Xu = ifftshift(Xs_out);
    VecComplex y_out = ifft(Xu);

    return y_out;
}

// -----------------------------
// applyNonlinearity()
// Pointwise Kerr phase: y_out = y_in * exp(-j * gamma * |y|^2 * h)
// -----------------------------
VecComplex FiberWDM::applyNonlinearity(const VecComplex& y_in, double h) {
    VecComplex y_out(Nt);
    for (int i = 0; i < Nt; ++i) {
        double power = std::norm(y_in[i]); // |y|^2
        double phase = -gamma * power * h; // negative sign matches forward propagation convention
        Complex ph = std::polar(1.0, phase); // exp(j*phase)
        y_out[i] = y_in[i] * ph;
    }
    return y_out;
}

// -----------------------------
// chooseStep()
// Implements 'constant', 'local_error', 'global_error' stepping rules.
// -----------------------------
double FiberWDM::chooseStep(const std::string& method,
    double dz,
    double L,
    double z,
    double control_param,
    const VecComplex& y,
    double gamma_local,
    double beta2_local,
    double B_wdm)
{
    // lowercase method for easier comparison
    std::string m = method;
    for (auto& ch : m) ch = static_cast<char>(std::tolower(ch));

    if (m.empty() || m == "constant") {
        if (control_param > 0.0) return control_param;
        return dz;
    }
    else if (m == "local_error") {
        double LE_target = control_param;
        double Pmax = 0.0;
        for (const auto& v : y) {
            double p = std::norm(v);
            if (p > Pmax) Pmax = p;
        }
        if (Pmax <= 0.0) return L - z;
        double denom = gamma_local * Pmax * (4.0 * M_PI * M_PI * beta2_local * beta2_local) * std::pow(B_wdm, 4);
        if (denom <= 0.0) return dz;
        return std::pow(LE_target / denom, 1.0 / 3.0);
    }
    else if (m == "global_error") {
        double GE_target = control_param;
        double Pmax = 0.0;
        for (const auto& v : y) {
            double p = std::norm(v);
            if (p > Pmax) Pmax = p;
        }
        if (Pmax <= 0.0) return L - z;
        double denom = L * gamma_local * Pmax * (4.0 * M_PI * M_PI * beta2_local * beta2_local) * std::pow(B_wdm, 4);
        if (denom <= 0.0) return dz;
        return std::sqrt(GE_target / denom);
    }
    else {
        // unknown method -> fallback
        if (control_param > 0.0) return control_param;
        return dz;
    }
}

// -----------------------------
// FFT / IFFT wrappers using FFTW
// Behavior note:
//  - fft returns forward transform without dividing by N (MATLAB-like).
//  - ifft returns inverse transform with 1/N normalization.
// Implementation is simple (plan-per-call) to keep code readable.
// -----------------------------
VecComplex FiberWDM::fft(const VecComplex& in) {
    int N = static_cast<int>(in.size());
    VecComplex out(N);

    fftw_complex* in_arr = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex* out_arr = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

    for (int i = 0; i < N; ++i) {
        in_arr[i][0] = in[i].real();
        in_arr[i][1] = in[i].imag();
    }

    // create plan and execute
    fftw_plan p = fftw_plan_dft_1d(N, in_arr, out_arr, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);

    for (int i = 0; i < N; ++i) {
        out[i] = Complex(out_arr[i][0], out_arr[i][1]); // no normalization
    }

    fftw_destroy_plan(p);
    fftw_free(in_arr);
    fftw_free(out_arr);

    return out;
}

VecComplex FiberWDM::ifft(const VecComplex& in) {
    int N = static_cast<int>(in.size());
    VecComplex out(N);

    fftw_complex* in_arr = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex* out_arr = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

    for (int i = 0; i < N; ++i) {
        in_arr[i][0] = in[i].real();
        in_arr[i][1] = in[i].imag();
    }

    // create plan and execute inverse FFT
    fftw_plan p = fftw_plan_dft_1d(N, in_arr, out_arr, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(p);

    // apply 1/N normalization to match MATLAB ifft
    for (int i = 0; i < N; ++i) {
        out[i] = Complex(out_arr[i][0] / static_cast<double>(N),
            out_arr[i][1] / static_cast<double>(N));
    }

    fftw_destroy_plan(p);
    fftw_free(in_arr);
    fftw_free(out_arr);

    return out;
}

// -----------------------------
// fftshift / ifftshift
// Simple, explicit implementation for beginners.
// For even N: swap halves. For odd N: use floor(N/2) style shift.
// -----------------------------
VecComplex FiberWDM::fftshift(const VecComplex& in) {
    int N = static_cast<int>(in.size());
    VecComplex out(N);
    int half = N / 2;

    if (N % 2 == 0) {
        for (int i = 0; i < half; ++i) {
            out[i] = in[i + half];
            out[i + half] = in[i];
        }
    }
    else {
        // odd N: shift by floor(N/2) + 1 for MATLAB-like behavior
        int k = half;
        for (int i = 0; i < N; ++i) {
            int idx = (i + k + 1) % N;
            out[i] = in[idx];
        }
    }
    return out;
}

VecComplex FiberWDM::ifftshift(const VecComplex& in) {
    int N = static_cast<int>(in.size());
    VecComplex out(N);
    int half = N / 2;

    if (N % 2 == 0) {
        for (int i = 0; i < half; ++i) {
            out[i] = in[i + half];
            out[i + half] = in[i];
        }
    }
    else {
        int k = half;
        for (int i = 0; i < N; ++i) {
            int idx = (i - (k + 1) + N) % N;
            out[i] = in[idx];
        }
    }
    return out;
}

// -----------------------------
// cleanup()
// Placeholder for future persistent resources cleanup.
// -----------------------------
void FiberWDM::cleanup() {
    // nothing to do currently
}
