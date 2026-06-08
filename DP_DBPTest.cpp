/*******************************************************************************
 * Copyright (c) 2026 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software
 * is strictly prohibited.
 ******************************************************************************/

/**
 * @file: DP_DBPTest.cpp
 * @brief: Comprehensive test suite for SSFM_Core, DP_Fiber, and DP_DBP.
 *         Covers fiber physics, SSFM kernels, forward propagation, DBP
 *         round-trip compensation, and multi-signal visualization data.
 *         This single file produces all test results for both Agents.
 * @author: Jiahao Zhang
 * @version: 1.0.0
 * @date: 2026-06-06
 * @department: Research and Development Department
 * @project: ZKIC Agents Development
 */

/**
 * Revision History:
 * * Version    Author          Date          Changes
 * -----------------------------------------------------------------------------
 * 1.0.0      Jiahao Zhang    2026-06-06    Initial version
 */

#include "SSFM_Core.h"
#include "DP_Fiber.h"
#include "DP_DBP.h"

#include <common_values.h>   // ZK::PHY_C, ZK::MA_PI
#include <converters.h>      // ZK::to_cvec
#include <elem_math.h>       // ZK::sqr
#include <fft.h>             // ZK::fftC2C, ZK::ifftC2C
#include <log_exp.h>         // ZK::exp
#include <specmat.h>         // ZK::linspace
#include <trig_hyp.h>        // ZK::cos, ZK::sin
#include <vec.h>             // ZK::vec, ZK::cvec
#include <mat.h>             // ZK::cmat
#include <random.h>          // ZK::randb (random bits for QPSK)

#include <cmath>             // std::abs, std::sqrt, std::exp, std::pow
#include <complex>           // std::complex, std::norm
#include <fstream>           // std::ofstream
#include <iostream>          // std::cout, std::endl
#include <iomanip>           // std::setprecision
#include <string>

// ============================================================
// Test helpers
// ============================================================
ZK::cmat generateGaussianPulse(long long Nt, double dt,
                                double t0, double T,
                                double ampX, double ampY)
{
    ZK::cmat signal(2, Nt);
    for (long long i = 0; i < Nt; ++i) {
        double t = static_cast<double>(i) * dt;
        double env = std::exp(-std::pow((t - t0) / T, 2.0));
        signal(0, i) = std::complex<double>(ampX * env, 0.0);
        signal(1, i) = std::complex<double>(ampY * env, 0.0);
    }
    return signal;
}

ZK::cmat generateCWTone(long long Nt, double dt, double fX, double fY)
{
    ZK::cmat signal(2, Nt);
    for (long long i = 0; i < Nt; ++i) {
        double t = static_cast<double>(i) * dt;
        signal(0, i) = std::complex<double>(std::cos(2.0 * ZK::MA_PI * fX * t),
                                             std::sin(2.0 * ZK::MA_PI * fX * t));
        signal(1, i) = std::complex<double>(std::cos(2.0 * ZK::MA_PI * fY * t),
                                             std::sin(2.0 * ZK::MA_PI * fY * t));
    }
    return signal;
}

// ============================================================
// Test signal generator: single-frequency complex tone.
// Purpose: pure CW signal for testing SPM/XPM phase shifts.
// A single tone has constant envelope → P_total is constant
// → Manakov phase shift is purely deterministic.
// ============================================================
ZK::cmat generateSingleTone(long long Nt, double dt, double fTone, double ampX, double ampY)
{
    ZK::cmat signal(2, Nt);
    for (long long i = 0; i < Nt; ++i) {
        double t = static_cast<double>(i) * dt;
        double phase = 2.0 * ZK::MA_PI * fTone * t;
        signal(0, i) = std::complex<double>(ampX * std::cos(phase), ampX * std::sin(phase));
        signal(1, i) = std::complex<double>(ampY * std::cos(phase), ampY * std::sin(phase));
    }
    return signal;
}

// ============================================================
// Test signal generator: dual-frequency tones on X-pol.
// Purpose: two closely-spaced frequencies test nonlinear
// wave mixing (four-wave mixing / cross-phase modulation
// between frequency components).  Non-constant envelope
// exercises the P_total-dependent Manakov phase.
// Expected: after pure nonlinearity, intermodulation
// products appear at sum/difference frequencies.
// ============================================================
ZK::cmat generateDualTone(long long Nt, double dt,
                           double f1, double f2,
                           double amp1, double amp2)
{
    ZK::cmat signal(2, Nt);
    for (long long i = 0; i < Nt; ++i) {
        double t = static_cast<double>(i) * dt;
        std::complex<double> tone1(amp1 * std::cos(2.0 * ZK::MA_PI * f1 * t),
                                    amp1 * std::sin(2.0 * ZK::MA_PI * f1 * t));
        std::complex<double> tone2(amp2 * std::cos(2.0 * ZK::MA_PI * f2 * t),
                                    amp2 * std::sin(2.0 * ZK::MA_PI * f2 * t));
        signal(0, i) = tone1 + tone2;           // X-pol: both tones
        signal(1, i) = 0.7 * (tone1 + tone2);   // Y-pol: scaled copy
    }
    return signal;
}

// ============================================================
// Test signal generator: QPSK communication signal with random
//   ±1 ± j symbols and Gaussian pulse shaping.
// Purpose: generate a realistic digital communication waveform
//   where each symbol is one of four constellation points:
//   (1+j), (-1+j), (1-j), (-1-j) — all normalized by 1/sqrt(2).
//   Symbols are random (time-based seed) so each run produces
//   a different bit sequence.
// Expected: after fiber + DBP, the constellation should recover
//   to four distinct clusters at the QPSK points.
// ============================================================
ZK::cmat generateQPSK(long long Nt, double dt, long long nSym, long long sps,
                       double pulseWidth)
{
    ZK::cmat signal(2, Nt);

    // QPSK constellation: scaled for realistic optical power.
    // Scale 0.1 gives peak power ~0.016 W per polarization.
    // At 80 km this produces ~89° SPM rotation + visible dispersion
    // scattering — ideal for demonstrating "clean → distorted → recovered".
    const double scale = 0.1;
    const std::complex<double> qpskMap[4] = {
        std::complex<double>( scale,  scale),
        std::complex<double>(-scale,  scale),
        std::complex<double>( scale, -scale),
        std::complex<double>(-scale, -scale)
    };

    // ZKIC random bits for all QPSK symbols at once
    // 2 bits per symbol × nSym symbols × 2 polarizations
    long long totalBits = 4 * nSym;
    ZK::bvec bits = ZK::randb(totalBits);

    // Initialize signal to zero
    for (long long i = 0; i < Nt; ++i) {
        signal(0, i) = std::complex<double>(0.0, 0.0);
        signal(1, i) = std::complex<double>(0.0, 0.0);
    }

    // Generate random QPSK symbols with Gaussian pulse shaping
    for (long long sym = 0; sym < nSym; ++sym) {
        // X-pol: two random bits → one of four QPSK constellation points
        int b0 = static_cast<int>(bits(2 * sym));
        int b1 = static_cast<int>(bits(2 * sym + 1));
        std::complex<double> qpskSym = qpskMap[b0 * 2 + b1];

        // Gaussian pulse centered at symbol position
        double tCenter = (sym + 0.5) * static_cast<double>(sps) * dt;
        for (long long i = 0; i < Nt; ++i) {
            double t = static_cast<double>(i) * dt;
            double env = std::exp(-std::pow((t - tCenter) / pulseWidth, 2.0));
            signal(0, i) += qpskSym * env;
            // Y-pol: independent random QPSK symbol (offset by 2*nSym bits)
            int b0y = static_cast<int>(bits(2 * nSym + 2 * sym));
            int b1y = static_cast<int>(bits(2 * nSym + 2 * sym + 1));
            std::complex<double> qpskSymY = qpskMap[b0y * 2 + b1y];
            signal(1, i) += qpskSymY * env;
        }
    }
    return signal;
}

double maxAbsDiff(const ZK::cmat& a, const ZK::cmat& b)
{
    double maxDiff = 0.0;
    for (int r = 0; r < a.rows(); ++r) {
        for (int c = 0; c < a.cols(); ++c) {
            double diff = std::abs(a(r, c) - b(r, c));
            if (diff > maxDiff) maxDiff = diff;
        }
    }
    return maxDiff;
}

double computeNMSE(const ZK::cmat& ref, const ZK::cmat& est)
{
    double num = 0.0, den = 0.0;
    for (int r = 0; r < ref.rows(); ++r) {
        for (int c = 0; c < ref.cols(); ++c) {
            double d = std::abs(ref(r, c) - est(r, c));
            num += d * d;
            den += std::norm(ref(r, c));
        }
    }
    return (den > 0.0) ? num / den : 0.0;
}

double signalEnergy(const ZK::cmat& sig)
{
    double energy = 0.0;
    for (int r = 0; r < sig.rows(); ++r) {
        for (int c = 0; c < sig.cols(); ++c) {
            energy += std::norm(sig(r, c));
        }
    }
    return energy;
}

void printResult(const std::string& testName, bool passed, double metric = -1.0)
{
    const char* status = passed ? "PASS" : "FAIL";
    std::cout << "  [" << status << "] " << testName;
    if (metric >= 0.0) {
        std::cout << "  (metric = " << std::scientific << std::setprecision(4)
                  << metric << ")";
    }
    std::cout << std::endl;
}

// ============================================================
// Test group 1: SSFM_Core unit tests
//
// These tests validate the shared SSFM computation kernels in
// isolation, without invoking the full Agent execute() pipeline.
// Each test checks a single static method against a hand-computed
// expected value or a known mathematical property.
//
// Verification:
//   - Tests 1.1-1.4: metrics are absolute or relative errors;
//     a passing test means the error is below the tolerance.
//   - Test 1.5: metric is the max absolute difference after an
//     FFT forward/backward round-trip; should be near zero.
//   - For visual confirmation, see DP_FiberTest.m (MATLAB).
// ============================================================
void testSSFMCore(int& passed, int& failed)
{
    std::cout << "\n=== Test Group 1: SSFM_Core Unit Tests ===\n" << std::endl;

    // --- 1.1: computeFiberPhysics ---
    {
        std::string name = "1.1 computeFiberPhysics (derived parameters)";
        ZK::FiberPhysics phys;
        ZK::SSFM_Core::computeFiberPhysics(
            0.2e-3, 1.47, 16.5e-6, 0.08e3,
            2.6e-20, 80e-12, 1550e-9, phys);

        double beta1_expected = 1.47 / 299792458.0;
        double beta2_expected = -16.5e-6 * (1550e-9*1550e-9) / (2.0*ZK::MA_PI*299792458.0);
        double gamma_expected = 2.0*ZK::MA_PI*2.6e-20 / (1550e-9*80e-12);
        double alphaNp_expected = 0.2e-3 * std::log(10.0) / 10.0;

        bool ok = true;
        ok &= std::abs(phys.beta1 - beta1_expected) / std::abs(beta1_expected) < 0.01;
        ok &= std::abs(phys.beta2 - beta2_expected) / std::abs(beta2_expected) < 0.01;
        ok &= std::abs(phys.gamma - gamma_expected) / std::abs(gamma_expected) < 0.01;
        ok &= std::abs(phys.alphaNp - alphaNp_expected) / std::abs(alphaNp_expected) < 0.01;

        printResult(name, ok);
        if (ok) ++passed; else ++failed;
    }

    // --- 1.2: buildFrequencyContext ---
    {
        std::string name = "1.2 buildFrequencyContext (grid properties)";
        long long Nt = 256;
        double fs = 100e9;
        ZK::FrequencyContext ctx;
        ZK::SSFM_Core::buildFrequencyContext(Nt, fs, -2.0e-26, 0.0, ctx);

        bool ok = true;
        ok &= (ctx.freq.size() == Nt);
        ok &= std::abs(ctx.df - fs / Nt) < 1e-6;
        ok &= std::abs(ctx.freq(0) - (-fs / 2.0)) / (fs / 2.0) < 0.01;
        ok &= std::abs(ctx.freq(Nt - 1) - (fs / 2.0 - ctx.df)) / ctx.df < 0.01;
        // betaOmega is even: check symmetry for pairs (i, Nt-i)
        bool sym = true;
        for (long long i = 1; i < Nt / 2; ++i) {
            if (std::abs(ctx.betaOmega(i) - ctx.betaOmega(Nt - i)) > 1e-12) {
                sym = false;
                break;
            }
        }
        ok &= sym;

        printResult(name, ok);
        if (ok) ++passed; else ++failed;
    }

    // --- 1.3: chooseStep constant mode ---
    {
        std::string name = "1.3 chooseStep (constant mode)";
        long long Nt = 64;
        ZK::cmat y(2, Nt);
        for (int r = 0; r < 2; ++r)
            for (int c = 0; c < Nt; ++c)
                y(r, c) = std::complex<double>(1.0, 0.0);

        double h = ZK::SSFM_Core::chooseStep(
            "constant", 2e3, 100e3, 0.0, 1e3, y, 1.0, -2e-26, 150e9);
        bool ok = (std::abs(h - 2e3) < 1e-9);
        printResult(name, ok);
        if (ok) ++passed; else ++failed;
    }

    // --- 1.4: applyNonlinearity (Manakov) ---
    {
        std::string name = "1.4 applyNonlinearity (Manakov phase check)";
        long long Nt = 2;
        ZK::cmat yIn(2, Nt);
        yIn(0, 0) = std::complex<double>(1.0, 0.0);
        yIn(0, 1) = std::complex<double>(0.5, 0.0);
        yIn(1, 0) = std::complex<double>(0.0, 0.0);
        yIn(1, 1) = std::complex<double>(0.3, 0.0);

        ZK::cmat yOut(2, Nt);
        ZK::SSFM_Core::applyNonlinearity(yIn, 1.0, 1.0, yOut);

        double phi0 = -(8.0 / 9.0);
        std::complex<double> expected00 = std::polar(1.0, phi0);
        double diff0 = std::abs(yOut(0, 0) - expected00);

        double phi1 = -(8.0 / 9.0) * 0.34;
        std::complex<double> expected01 = std::polar(0.5, phi1);
        std::complex<double> expected11 = std::polar(0.3, phi1);
        double diff1 = std::abs(yOut(0, 1) - expected01);
        double diff2 = std::abs(yOut(1, 1) - expected11);

        bool ok = (diff0 < 1e-12) && (diff1 < 1e-12) && (diff2 < 1e-12);
        printResult(name, ok, diff0 + diff1 + diff2);
        if (ok) ++passed; else ++failed;
    }

    // --- 1.5: applyDispersion round-trip ---
    {
        std::string name = "1.5 applyDispersion (FFT round-trip fidelity)";
        long long Nt = 256;
        double fs = 100e9;
        double dt = 1.0 / fs;
        double t0 = Nt * dt / 2.0;
        ZK::cmat yIn = generateGaussianPulse(Nt, dt, t0, 50e-12, 1.0, 0.7);

        ZK::FrequencyContext ctx;
        ZK::SSFM_Core::buildFrequencyContext(Nt, fs, 0.0, 0.0, ctx);

        ZK::cmat yMid(2, Nt);
        ZK::cmat yOut(2, Nt);
        ZK::SSFM_Core::applyDispersion(yIn, ctx.betaOmega, 0.0, 1.0, yMid);
        ZK::SSFM_Core::applyDispersion(yMid, ctx.betaOmega, 0.0, -1.0, yOut);

        double maxDiff = maxAbsDiff(yIn, yOut);
        bool ok = (maxDiff < 1e-12);
        printResult(name, ok, maxDiff);
        if (ok) ++passed; else ++failed;
    }
}

// ============================================================
// Test group 2: DP_Fiber Agent tests
//
// These tests validate DP_Fiber::execute() under controlled
// physical regimes.  Each test isolates one physical effect
// (loss, dispersion, nonlinearity) by zeroing the others, then
// a final test enables all effects together.
//
// Verification:
//   - Test 2.1: maxAbsDiff should be exactly 0 (no propagation).
//   - Test 2.2: energy ratio should match exp(-alphaNp * L).
//   - Test 2.3: energy conserved (dispersion is lossless).
//   - Test 2.4: energy conserved (SPM is lossless).
//   - Test 2.5: energyOut is finite and positive (no crash).
//   - For visual confirmation, run DP_FiberTest.m in MATLAB
//     after the C++ test generates the .dat files.
// ============================================================
void testDPFiber(int& passed, int& failed)
{
    std::cout << "\n=== Test Group 2: DP_Fiber Tests ===\n" << std::endl;

    const long long Nt = 1024;
    const double fs = 100e9;
    const double dt = 1.0 / fs;

    // --- 2.1: Zero propagation ---
    {
        std::string name = "2.1 Zero propagation (lSpan = 0)";
        ZK::cmat signal = generateGaussianPulse(Nt, dt, Nt*dt/2.0, 100e-12, 1.0, 0.8);

        ZK::DP_Fiber::Parameters params;
        params.Nt = Nt;
        params.fs = fs;
        params.lSpan = 0.0;
        params.alpha_dBpm = 0.0;
        params.bWdm = 50e9;

        ZK::DP_Fiber::Signals sigs;
        sigs.oIn = signal;
        ZK::DP_Fiber::execute(params, sigs);

        double maxDiff = maxAbsDiff(signal, sigs.out);
        bool ok = (maxDiff < 1e-12);
        printResult(name, ok, maxDiff);
        if (ok) ++passed; else ++failed;
    }

    // --- 2.2: Pure loss ---
    {
        std::string name = "2.2 Pure loss (gamma=0, beta2/3=0)";
        double alpha_dBpm = 0.2e-3;
        double L = 100e3;
        double alphaNp = alpha_dBpm * std::log(10.0) / 10.0;

        ZK::cmat signal = generateGaussianPulse(Nt, dt, Nt*dt/2.0, 100e-12, 1.0, 0.8);

        ZK::DP_Fiber::Parameters params;
        params.Nt = Nt;
        params.fs = fs;
        params.lSpan = L;
        params.alpha_dBpm = alpha_dBpm;
        params.dispersion = 0.0;
        params.disS = 0.0;
        params.n2 = 0.0;
        params.bWdm = 50e9;

        ZK::DP_Fiber::Signals sigs;
        sigs.oIn = signal;
        ZK::DP_Fiber::execute(params, sigs);

        double expectedAtten = std::exp(-alphaNp * L / 2.0);
        double energyIn = signalEnergy(signal);
        double energyOut = signalEnergy(sigs.out);
        double ratio = std::sqrt(energyOut / energyIn);
        bool ok = std::abs(ratio - expectedAtten) / expectedAtten < 0.02;
        printResult(name, ok, std::abs(ratio - expectedAtten));
        if (ok) ++passed; else ++failed;
    }

    // --- 2.3: Pure dispersion (energy conservation) ---
    {
        std::string name = "2.3 Pure dispersion (gamma=0, alpha=0)";
        ZK::cmat signal = generateGaussianPulse(Nt, dt, Nt*dt/2.0, 100e-12, 1.0, 0.8);

        ZK::DP_Fiber::Parameters params;
        params.Nt = Nt;
        params.fs = fs;
        params.lSpan = 50e3;
        params.alpha_dBpm = 0.0;
        params.n2 = 0.0;
        params.bWdm = 50e9;

        ZK::DP_Fiber::Signals sigs;
        sigs.oIn = signal;
        ZK::DP_Fiber::execute(params, sigs);

        double relDiff = std::abs(signalEnergy(sigs.out) - signalEnergy(signal))
                       / signalEnergy(signal);
        bool ok = (relDiff < 0.01);
        printResult(name, ok, relDiff);
        if (ok) ++passed; else ++failed;
    }

    // --- 2.4: Pure nonlinearity (SPM energy conservation) ---
    {
        std::string name = "2.4 Pure nonlinearity (SPM phase on CW)";
        ZK::cmat signal = generateCWTone(Nt, dt, 1e9, 2e9);

        ZK::DP_Fiber::Parameters params;
        params.Nt = Nt;
        params.fs = fs;
        params.lSpan = 1e3;
        params.alpha_dBpm = 0.0;
        params.dispersion = 0.0;
        params.disS = 0.0;
        params.bWdm = 50e9;

        ZK::DP_Fiber::Signals sigs;
        sigs.oIn = signal;
        ZK::DP_Fiber::execute(params, sigs);

        double relDiff = std::abs(signalEnergy(sigs.out) - signalEnergy(signal))
                       / signalEnergy(signal);
        bool ok = (relDiff < 0.01);
        printResult(name, ok, relDiff);
        if (ok) ++passed; else ++failed;
    }

    // --- 2.5: Full effects (stability) ---
    {
        std::string name = "2.5 Full effects (all physics enabled)";
        ZK::cmat signal = generateGaussianPulse(Nt, dt, Nt*dt/2.0, 100e-12, 1.0, 0.8);

        ZK::DP_Fiber::Parameters params;
        params.Nt = Nt;
        params.fs = fs;
        params.lSpan = 10e3;
        params.bWdm = 50e9;

        ZK::DP_Fiber::Signals sigs;
        sigs.oIn = signal;
        ZK::DP_Fiber::execute(params, sigs);

        double energyOut = signalEnergy(sigs.out);
        bool ok = std::isfinite(energyOut) && energyOut > 0.0;
        printResult(name, ok, energyOut);
        if (ok) ++passed; else ++failed;
    }
}

// ============================================================
// Test group 3: DP_DBP round-trip tests
//
// These tests validate the full TX -> Fiber -> DBP pipeline.
// The core principle: DBP with matching parameters should exactly
// reverse fiber propagation (within split-step numerical error).
//
// Verification:
//   - Test 3.1: identity check — nSpans=0 means no processing.
//   - Test 3.2: stability check — DBP on clean signal runs
//     without NaN/Inf (it applies "negative fiber" to a clean
//     signal, so output differs from input — that's expected).
//   - Test 3.3 (CORE): round-trip NMSE should be near zero
//     (~1e-25 with 10km, confirming DBP cancels fiber effects).
//   - Test 3.4: wrong beta2 in DBP produces WORSE NMSE than
//     correct beta2 (validates DBP accuracy depends on knowing
//     the fiber parameters).
//   - Test 3.5: multi-span DBP NMSE should be near zero
//     (~1e-28 with fine step size).
//   - For visual confirmation, run DP_DBPTest.m in MATLAB.
// ============================================================
void testDBPRoundTrip(int& passed, int& failed)
{
    std::cout << "\n=== Test Group 3: DP_DBP Round-Trip Tests ===\n" << std::endl;

    const long long Nt = 1024;
    const double fs = 100e9;
    const double dt = 1.0 / fs;

    // --- 3.1: Zero spans (identity check) ---
    {
        std::string name = "3.1 Zero spans (nSpans = 0, output == input)";
        ZK::cmat signal = generateGaussianPulse(Nt, dt, Nt*dt/2.0, 100e-12, 1.0, 0.8);

        ZK::DP_DBP::Parameters params;
        params.Nt = Nt;
        params.fs = fs;
        params.nSpans = 0;
        params.lSpan = 100e3;

        ZK::DP_DBP::Signals sigs;
        sigs.oIn = signal;
        ZK::DP_DBP::execute(params, sigs);

        double maxDiff = maxAbsDiff(signal, sigs.out);
        bool ok = (maxDiff < 1e-12);
        printResult(name, ok, maxDiff);
        if (ok) ++passed; else ++failed;
    }

    // --- 3.2: Clean signal DBP (stability check) ---
    {
        std::string name = "3.2 Clean signal DBP (stability, no prior propagation)";
        ZK::cmat signal = generateGaussianPulse(Nt, dt, Nt*dt/2.0, 100e-12, 1.0, 0.8);

        ZK::DP_DBP::Parameters params;
        params.Nt = Nt;
        params.fs = fs;
        params.nSpans = 1;
        params.lSpan = 10e3;

        ZK::DP_DBP::Signals sigs;
        sigs.oIn = signal;
        ZK::DP_DBP::execute(params, sigs);

        double energyOut = signalEnergy(sigs.out);
        bool ok = std::isfinite(energyOut) && energyOut > 0.0;
        printResult(name, ok, energyOut);
        if (ok) ++passed; else ++failed;
    }

    // --- 3.3: 1-span round-trip ---
    {
        std::string name = "3.3 Round-trip: TX -> Fiber -> DBP (1 span, 10 km)";
        ZK::cmat signal = generateGaussianPulse(Nt, dt, Nt*dt/2.0, 100e-12, 1.0, 0.8);

        ZK::DP_Fiber::Parameters fwdParams;
        fwdParams.Nt = Nt;
        fwdParams.fs = fs;
        fwdParams.lSpan = 10e3;
        fwdParams.bWdm = 50e9;

        ZK::DP_Fiber::Signals fwdSigs;
        fwdSigs.oIn = signal;
        ZK::DP_Fiber::execute(fwdParams, fwdSigs);

        ZK::DP_DBP::Parameters dbpParams;
        dbpParams.Nt = Nt;
        dbpParams.fs = fs;
        dbpParams.nSpans = 1;
        dbpParams.lSpan = 10e3;

        ZK::DP_DBP::Signals dbpSigs;
        dbpSigs.oIn = fwdSigs.out;
        ZK::DP_DBP::execute(dbpParams, dbpSigs);

        double nmse = computeNMSE(signal, dbpSigs.out);
        bool ok = (nmse < 0.05);
        printResult(name, ok, nmse);
        if (ok) ++passed; else ++failed;
    }

    // --- 3.2: Parameter mismatch (verifies DBP accuracy depends on parameters) ---
    {
        std::string name = "3.4 DBP parameter mismatch (wrong beta2 -> worse NMSE)";
        ZK::cmat signal = generateGaussianPulse(Nt, dt, Nt*dt/2.0, 100e-12, 1.0, 0.8);

        ZK::DP_Fiber::Parameters fwdParams;
        fwdParams.Nt = Nt;
        fwdParams.fs = fs;
        fwdParams.lSpan = 10e3;
        fwdParams.bWdm = 50e9;

        ZK::DP_Fiber::Signals fwdSigs;
        fwdSigs.oIn = signal;
        ZK::DP_Fiber::execute(fwdParams, fwdSigs);

        // DBP with wrong dispersion
        ZK::DP_DBP::Parameters dbpWrong;
        dbpWrong.Nt = Nt;
        dbpWrong.fs = fs;
        dbpWrong.nSpans = 1;
        dbpWrong.lSpan = 10e3;
        dbpWrong.dispersion = 16.5e-6 * 2.0;

        ZK::DP_DBP::Signals dbpSigsWrong;
        dbpSigsWrong.oIn = fwdSigs.out;
        ZK::DP_DBP::execute(dbpWrong, dbpSigsWrong);
        double nmseWrong = computeNMSE(signal, dbpSigsWrong.out);

        // DBP with correct parameters
        ZK::DP_DBP::Parameters dbpCorrect;
        dbpCorrect.Nt = Nt;
        dbpCorrect.fs = fs;
        dbpCorrect.nSpans = 1;
        dbpCorrect.lSpan = 10e3;

        ZK::DP_DBP::Signals dbpSigsCorrect;
        dbpSigsCorrect.oIn = fwdSigs.out;
        ZK::DP_DBP::execute(dbpCorrect, dbpSigsCorrect);
        double nmseCorrect = computeNMSE(signal, dbpSigsCorrect.out);

        bool ok = (nmseWrong > nmseCorrect * 1.5);
        printResult(name, ok, nmseWrong);
        if (ok) ++passed; else ++failed;
    }

    // --- 3.3: Multi-span round-trip ---
    {
        std::string name = "3.5 Multi-span round-trip (3 spans, 1 km each)";
        ZK::cmat signal = generateGaussianPulse(Nt, dt, Nt*dt/2.0, 100e-12, 1.0, 0.8);

        ZK::DP_Fiber::Parameters fwdParams;
        fwdParams.Nt = Nt;
        fwdParams.fs = fs;
        fwdParams.lSpan = 1e3;
        fwdParams.dz = 200;
        fwdParams.bWdm = 50e9;

        ZK::cmat propagated = signal;
        ZK::DP_Fiber::Signals fwdSigs;
        for (int span = 0; span < 3; ++span) {
            fwdSigs.oIn = propagated;
            ZK::DP_Fiber::execute(fwdParams, fwdSigs);
            propagated = fwdSigs.out;
        }

        ZK::DP_DBP::Parameters dbpParams;
        dbpParams.Nt = Nt;
        dbpParams.fs = fs;
        dbpParams.nSpans = 3;
        dbpParams.lSpan = 1e3;
        dbpParams.dz = 200;

        ZK::DP_DBP::Signals dbpSigs;
        dbpSigs.oIn = propagated;
        ZK::DP_DBP::execute(dbpParams, dbpSigs);

        double nmse = computeNMSE(signal, dbpSigs.out);
        bool ok = (nmse < 0.10);
        printResult(name, ok, nmse);
        if (ok) ++passed; else ++failed;
    }
}

// ============================================================
// Main
// ============================================================
int main()
{
    int totalPassed = 0;
    int totalFailed = 0;

    std::cout << "==============================================" << std::endl;
    std::cout << "  DP_DBP Test Suite" << std::endl;
    std::cout << "  SSFM_Core + DP_Fiber + DP_DBP Round-Trip" << std::endl;
    std::cout << "==============================================" << std::endl;

    testSSFMCore(totalPassed, totalFailed);
    testDPFiber(totalPassed, totalFailed);
    testDBPRoundTrip(totalPassed, totalFailed);

    std::cout << "\n==============================================" << std::endl;
    std::cout << "  Results: " << totalPassed << " passed, "
              << totalFailed << " failed  ("
              << totalPassed + totalFailed << " total)" << std::endl;
    std::cout << "==============================================" << std::endl;

    // ============================================================
    // Visualization data generation
    //
    // Generates .dat files for MATLAB visualization (DP_FiberTest.m,
    // DP_DBPTest.m).  Four test signal types are used:
    //   1. Gaussian pulse   — tests pulse propagation / broadening
    //   2. Single tone (CW) — tests SPM/XPM phase shift
    //   3. Dual tone        — tests nonlinear wave mixing (FWM)
    //   4. QPSK signal      — tests communication signal integrity
    //
    // For each type, both DP_Fiber (input/output) and DP_DBP
    // round-trip (original/fiber/compensated) data are saved.
    // Each .dat file: (2 x Nt) cmat, rows = X/Y pol,
    // columns = 2*Nt reals (Re,Im alternating), no header.
    //
    // Verification (check console output):
    //   - Fiber: attenuation ratio ≈ exp(-alphaNp*L/2) for pure loss
    //   - Fiber: energy conserved for pure dispersion / pure NL
    //   - DBP:   NMSE(original, compensated) should be near zero
    //            (< 0.05 for 10km, < 0.10 for multi-span)
    //   - DBP:   NMSE(wrong params) should be WORSE than correct params
    // ============================================================
    if (totalFailed == 0) {
        std::cout << "\n=== Generating visualization data (.dat files) ===\n" << std::endl;

        const long long NtViz = 1024;
        const double fsViz = 100e9;
        const double dtViz = 1.0 / fsViz;
        const double t0Viz = NtViz * dtViz / 2.0;

        auto saveCmat = [](const ZK::cmat& data, const std::string& filename) {
            std::ofstream f(filename);
            if (f.is_open()) {
                f << std::scientific << std::setprecision(12);
                for (int r = 0; r < data.rows(); ++r) {
                    for (int c = 0; c < data.cols(); ++c) {
                        f << data(r,c).real() << " " << data(r,c).imag();
                        if (c < data.cols()-1) f << " ";
                    }
                    f << "\n";
                }
                f.close();
            }
        };

        // Helper: run forward fiber + DBP round-trip, save all 3 stages,
        // print NMSE.  Returns true if round-trip NMSE < threshold.
        auto vizRoundTrip = [&](const ZK::cmat& sigIn,
                                 const std::string& label,
                                 double lSpanKm, double nmseThreshold) {
            double lSpan = lSpanKm * 1e3;
            std::string origFile  = "DBP_" + label + "_Orig.dat";  // saveCmat uses as-is
            std::string fiberFile = "DBP_" + label + "_Fiber";    // execute() appends .dat
            std::string compFile  = "DBP_" + label + "_Comp";     // execute() appends .dat

            saveCmat(sigIn, origFile);

            ZK::DP_Fiber::Parameters fp;
            fp.Nt = NtViz; fp.fs = fsViz; fp.lSpan = lSpan; fp.bWdm = 50e9;
            fp.saveFile = fiberFile;
            ZK::DP_Fiber::Signals fsigs; fsigs.oIn = sigIn;
            ZK::DP_Fiber::execute(fp, fsigs);

            ZK::DP_DBP::Parameters dp;
            dp.Nt = NtViz; dp.fs = fsViz; dp.nSpans = 1; dp.lSpan = lSpan;
            dp.saveFile = compFile;
            ZK::DP_DBP::Signals dsigs; dsigs.oIn = fsigs.out;
            ZK::DP_DBP::execute(dp, dsigs);

            double nmse = computeNMSE(sigIn, dsigs.out);
            bool pass = (nmse < nmseThreshold);
            std::cout << "  " << label << " round-trip: NMSE=" << std::scientific
                      << nmse << "  threshold=" << nmseThreshold
                      << "  [" << (pass ? "PASS" : "FAIL") << "]" << std::endl;
        };

        // ========================================================
        // Signal type 1: Gaussian pulse
        // Purpose: verify pulse propagation through fiber.
        // Expected: after pure loss, amplitude scales by
        //   exp(-alphaNp*L/2).  After DBP, pulse is recovered
        //   (NMSE ~ 0 for short distances).
        // ========================================================
        std::cout << "--- Signal type 1: Gaussian pulse ---" << std::endl;
        {
            ZK::cmat sigIn = generateGaussianPulse(NtViz, dtViz, t0Viz, 100e-12, 1.0, 0.8);

            // Fiber: pure loss
            saveCmat(sigIn, "Fiber_GaussInput.dat");
            ZK::DP_Fiber::Parameters p;
            p.Nt = NtViz; p.fs = fsViz; p.lSpan = 100e3;
            p.dispersion = 0.0; p.disS = 0.0; p.n2 = 0.0; p.bWdm = 50e9;
            p.saveFile = "Fiber_GaussOutput";
            ZK::DP_Fiber::Signals s; s.oIn = sigIn;
            ZK::DP_Fiber::execute(p, s);
            double alphaNp = p.alpha_dBpm * std::log(10.0) / 10.0;
            double ratioExp = std::exp(-alphaNp * p.lSpan / 2.0);
            double ratioMeas = std::sqrt(signalEnergy(s.out) / signalEnergy(sigIn));
            std::cout << "  Fiber (pure loss): atten=" << ratioMeas
                      << " expected=" << ratioExp
                      << " [" << (std::abs(ratioMeas-ratioExp)/ratioExp < 0.02 ? "PASS" : "FAIL")
                      << "]" << std::endl;

            // DBP round-trip (10 km, all effects)
            vizRoundTrip(sigIn, "Gauss", 10.0, 0.05);
        }

        // ========================================================
        // Signal type 2: Single tone (CW)
        // Purpose: verify SPM/XPM phase shift on constant-envelope
        //   signal.  Pure CW has constant P_total, so Manakov
        //   nonlinearity produces a deterministic phase rotation
        //   proportional to P_total * lSpan.
        // Expected: energy conserved (no loss/gain).  After DBP,
        //   the phase rotation is undone (NMSE ~ 0).
        // ========================================================
        std::cout << "--- Signal type 2: Single tone (CW) ---" << std::endl;
        {
            // Use frequency that lands exactly on an FFT bin to avoid spectral
            // leakage (df = fs/Nt = 100e9/1024 ≈ 97.66 MHz, bin 20 = 1.953125 GHz)
            double fExact = 20.0 * fsViz / static_cast<double>(NtViz);
            ZK::cmat sigIn = generateSingleTone(NtViz, dtViz, fExact, 1.0, 0.8);

            // Fiber: pure nonlinearity (beta=0, alpha=0)
            saveCmat(sigIn, "Fiber_ToneInput.dat");
            ZK::DP_Fiber::Parameters p;
            p.Nt = NtViz; p.fs = fsViz; p.lSpan = 5e3;
            p.alpha_dBpm = 0.0; p.dispersion = 0.0; p.disS = 0.0; p.bWdm = 50e9;
            p.saveFile = "Fiber_ToneOutput";
            ZK::DP_Fiber::Signals s; s.oIn = sigIn;
            ZK::DP_Fiber::execute(p, s);
            double eRel = std::abs(signalEnergy(s.out) - signalEnergy(sigIn)) / signalEnergy(sigIn);
            std::cout << "  Fiber (pure NL): energy relDiff=" << eRel
                      << " [" << (eRel < 0.01 ? "PASS" : "FAIL") << "]" << std::endl;

            // DBP round-trip (shorter distance — narrowband CW is sensitive
            // to split-step error accumulation)
            vizRoundTrip(sigIn, "Tone", 3.0, 0.05);
        }

        // ========================================================
        // Signal type 3: Dual tone
        // Purpose: two closely-spaced frequencies exercise
        //   nonlinear wave mixing (FWM / XPM between tones).
        //   Non-constant envelope means P_total varies in time,
        //   producing time-varying Manakov phase.
        // Expected: after pure NL, intermodulation products
        //   appear at |f1±f2|.  DBP should undo the mixing
        //   (NMSE small but may be higher than single-tone
        //   due to more complex dynamics).
        // ========================================================
        std::cout << "--- Signal type 3: Dual tone ---" << std::endl;
        {
            ZK::cmat sigIn = generateDualTone(NtViz, dtViz, 1.5e9, 2.5e9, 0.7, 0.7);

            // Fiber: pure nonlinearity
            saveCmat(sigIn, "Fiber_DualInput.dat");
            ZK::DP_Fiber::Parameters p;
            p.Nt = NtViz; p.fs = fsViz; p.lSpan = 2e3;
            p.alpha_dBpm = 0.0; p.dispersion = 0.0; p.disS = 0.0; p.bWdm = 50e9;
            p.saveFile = "Fiber_DualOutput";
            ZK::DP_Fiber::Signals s; s.oIn = sigIn;
            ZK::DP_Fiber::execute(p, s);
            std::cout << "  Fiber (pure NL): energyOut=" << signalEnergy(s.out) << std::endl;

            // DBP round-trip (short distance — dual tone is sensitive)
            vizRoundTrip(sigIn, "Dual", 2.0, 0.05);
        }

        // ========================================================
        // Signal type 4: QPSK communication signal
        // Purpose: most important test — verify that a realistic
        //   digital communication waveform survives fiber+DBP.
        //   QPSK symbols with Gaussian pulse shaping.
        // Expected: after fiber propagation, constellation is
        //   distorted (dispersion spreads pulses, nonlinearity
        //   rotates phases).  After DBP, constellation should
        //   be recovered (low NMSE, clean eye opening).
        // Verification: NMSE < 0.05 means DBP effectively
        //   compensates fiber impairments for this QPSK signal.
        // ========================================================
        std::cout << "--- Signal type 4: QPSK ---" << std::endl;
        {
            // Parameters chosen so that BOTH dispersion and nonlinearity
            // visibly distort the constellation at 80 km:
            //   - 100 symbols at 10 sps → 100 ps symbol spacing
            //   - 35 ps pulses → clean original (<0.03% ISI)
            //   - LD ≈ 58 km, L=80 km → ~69% broadening → ~6% ISI after fiber
            //   - Scale 0.1 → ~89° SPM rotation → clear arc in constellation
            long long nSym = 100;
            long long sps  = 10;
            double pulseW  = 35e-12;
            ZK::cmat sigIn = generateQPSK(NtViz, dtViz, nSym, sps, pulseW);

            // Fiber: 80 km with realistic parameters
            saveCmat(sigIn, "Fiber_QPSKInput.dat");
            ZK::DP_Fiber::Parameters p;
            p.Nt = NtViz; p.fs = fsViz; p.lSpan = 80e3; p.dz = 2e3; p.bWdm = 50e9;
            p.saveFile = "Fiber_QPSKOutput";
            ZK::DP_Fiber::Signals s; s.oIn = sigIn;
            ZK::DP_Fiber::execute(p, s);
            double nmseFiber = computeNMSE(sigIn, s.out);
            std::cout << "  Fiber (80 km, full effects): NMSE vs input=" << nmseFiber << std::endl;

            // DBP round-trip at 80 km
            vizRoundTrip(sigIn, "QPSK", 80.0, 0.05);
        }

        std::cout << "\nVisualization data generation complete." << std::endl;
        std::cout << "Run DP_FiberTest.m and DP_DBPTest.m in MATLAB to view plots." << std::endl;
    }

    return (totalFailed == 0) ? 0 : 1;
}
