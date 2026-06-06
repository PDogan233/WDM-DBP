/*******************************************************************************
 * Copyright (c) 2026 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software
 * is strictly prohibited.
 ******************************************************************************/

/**
 * @file: DP_DBPTest.cpp
 * @brief: Standalone test suite for DP_DBP Agent.
 *         Covers zero-span, clean-signal DBP, round-trip compensation,
 *         parameter mismatch, and multi-span back-propagation.
 *         Rename from .cpp_archive to .cpp to activate (and deactivate
 *         DP_FiberTest.cpp by renaming to .cpp_archive).
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

#include <common_values.h>   // ZK::MA_PI
#include <elem_math.h>       // ZK::sqr
#include <mat.h>             // ZK::cmat

#include <cmath>             // std::abs, std::exp, std::pow
#include <complex>           // std::complex, std::norm
#include <iostream>          // std::cout, std::endl
#include <iomanip>           // std::setprecision
#include <string>

// ============================================================
// Test helpers (same as DP_FiberTest.cpp — kept per file for
// Agent independence)
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
// DP_DBP test cases (5 tests)
// ============================================================
void testDPDBP(int& passed, int& failed)
{
    std::cout << "\n=== DP_DBP Test Suite ===\n" << std::endl;

    const long long Nt = 1024;
    const double fs = 100e9;
    const double dt = 1.0 / fs;

    // --- Test 1: Zero spans ---
    {
        std::string name = "1. Zero spans (nSpans = 0)";
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

    // --- Test 2: Clean signal DBP (no prior propagation) ---
    {
        std::string name = "2. Clean signal DBP (stability check)";
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

    // --- Test 3: Round-trip (core validation) ---
    {
        std::string name = "3. Round-trip: Fiber -> DBP -> compare";
        ZK::cmat signal = generateGaussianPulse(Nt, dt, Nt*dt/2.0, 100e-12, 1.0, 0.8);

        // Forward
        ZK::DP_Fiber::Parameters fwdParams;
        fwdParams.Nt = Nt;
        fwdParams.fs = fs;
        fwdParams.lSpan = 10e3;
        fwdParams.bWdm = 50e9;

        ZK::DP_Fiber::Signals fwdSigs;
        fwdSigs.oIn = signal;
        ZK::DP_Fiber::execute(fwdParams, fwdSigs);

        // DBP
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

    // --- Test 4: Parameter mismatch degradation ---
    {
        std::string name = "4. Parameter mismatch (wrong dispersion degrades NMSE)";
        ZK::cmat signal = generateGaussianPulse(Nt, dt, Nt*dt/2.0, 100e-12, 1.0, 0.8);

        ZK::DP_Fiber::Parameters fwdParams;
        fwdParams.Nt = Nt;
        fwdParams.fs = fs;
        fwdParams.lSpan = 10e3;
        fwdParams.bWdm = 50e9;

        ZK::DP_Fiber::Signals fwdSigs;
        fwdSigs.oIn = signal;
        ZK::DP_Fiber::execute(fwdParams, fwdSigs);

        // DBP with wrong dispersion (2x)
        ZK::DP_DBP::Parameters dbpWrong;
        dbpWrong.Nt = Nt;
        dbpWrong.fs = fs;
        dbpWrong.nSpans = 1;
        dbpWrong.lSpan = 10e3;
        dbpWrong.dispersion = 16.5e-6 * 2.0;

        ZK::DP_DBP::Signals dbpWrongSigs;
        dbpWrongSigs.oIn = fwdSigs.out;
        ZK::DP_DBP::execute(dbpWrong, dbpWrongSigs);

        // DBP with correct parameters
        ZK::DP_DBP::Parameters dbpCorrect;
        dbpCorrect.Nt = Nt;
        dbpCorrect.fs = fs;
        dbpCorrect.nSpans = 1;
        dbpCorrect.lSpan = 10e3;

        ZK::DP_DBP::Signals dbpCorrectSigs;
        dbpCorrectSigs.oIn = fwdSigs.out;
        ZK::DP_DBP::execute(dbpCorrect, dbpCorrectSigs);

        double nmseWrong = computeNMSE(signal, dbpWrongSigs.out);
        double nmseCorrect = computeNMSE(signal, dbpCorrectSigs.out);
        bool ok = (nmseWrong > nmseCorrect * 1.5);
        printResult(name, ok, nmseWrong);
        if (ok) ++passed; else ++failed;
    }

    // --- Test 5: Multi-span round-trip ---
    {
        std::string name = "5. Multi-span round-trip (3 spans)";
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
    std::cout << "==============================================" << std::endl;

    testDPDBP(totalPassed, totalFailed);

    std::cout << "\n==============================================" << std::endl;
    std::cout << "  Results: " << totalPassed << " passed, "
              << totalFailed << " failed  ("
              << totalPassed + totalFailed << " total)" << std::endl;
    std::cout << "==============================================" << std::endl;

    return (totalFailed == 0) ? 0 : 1;
}
