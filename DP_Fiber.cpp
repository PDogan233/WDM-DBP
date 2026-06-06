/*******************************************************************************
 * Copyright (c) 2026 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software
 * is strictly prohibited.
 ******************************************************************************/

/**
 * @file: DP_Fiber.cpp
 * @brief: Implementation of the DP_Fiber Agent — forward dual-polarization
 *         fiber propagation using the symmetric SSFM (Strang splitting) for
 *         the Manakov equation.
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

#include "DP_Fiber.h"
#include "SSFM_Core.h"

#include <fstream>    // std::ofstream
#include <iomanip>    // std::scientific, std::setprecision
#include <stdexcept>  // std::invalid_argument

namespace ZK {

// ============================================================
// Static member function name:
//   void DP_Fiber::checkParam(const Parameters&)
//
// Input:
//   params  // DP_Fiber::Parameters, fiber and numerical parameters
//
// Output: void (throws std::invalid_argument on invalid input)
//
// Functional description:
//   Validates that required parameters are physically meaningful.
//   Checks: Nt > 0, fs > 0, L_span >= 0, B_wdm >= 0.
// ============================================================
void DP_Fiber::checkParam(const Parameters& params)
{
    if (params.Nt <= 0) {
        throw std::invalid_argument("DP_Fiber: Nt must be > 0");
    }
    if (params.fs <= 0.0) {
        throw std::invalid_argument("DP_Fiber: fs must be > 0");
    }
    if (params.lSpan < 0.0) {
        throw std::invalid_argument("DP_Fiber: lSpan must be >= 0");
    }
    if (params.bWdm < 0.0) {
        throw std::invalid_argument("DP_Fiber: bWdm must be >= 0");
    }
}

// ============================================================
// Static member function name:
//   void DP_Fiber::execute(const Parameters&, Signals&)
//
// Input:
//   params  // DP_Fiber::Parameters — fiber physics, numerical settings, sampling
//   signals // DP_Fiber::Signals — oIn: (2 x Nt) input optical field
//
// Output: void, propagated field stored in signals.out
//
// Functional description:
//   Executes forward fiber propagation using the symmetric SSFM.
//   1. Compute derived physical parameters (beta2/3, gamma, alphaNp).
//   2. Build frequency context (grid + beta(omega)).
//   3. Main SSFM loop: NL(h/2) -> Disp(h) -> NL(h/2) (Strang splitting).
//   4. Optionally save output to a .dat file.
// ============================================================
void DP_Fiber::execute(const Parameters& params, Signals& signals)
{
    // ---- Step 0: validate parameters ----
    checkParam(params);

    // ---- Step 1: compute derived fiber physics ----
    FiberPhysics phys;
    SSFM_Core::computeFiberPhysics(
        params.alpha_dBpm, params.groupRef, params.dispersion,
        params.disS, params.n2, params.aEff, params.lambda0,
        phys);

    // ---- Step 2: build frequency context ----
    FrequencyContext fctx;
    SSFM_Core::buildFrequencyContext(
        params.Nt, params.fs, phys.beta2, phys.beta3,
        fctx);

    // ---- Step 3: copy input signal to output ----
    long long Nt = signals.oIn.cols();
    signals.out.set_size(2, Nt, false);
    signals.out = signals.oIn;

    // ---- Step 4: SSFM main loop ----
    double z = 0.0;
    const double L = params.lSpan;

    while (z < L - 1e-12) {
        // Choose step size
        double h = SSFM_Core::chooseStep(
            params.method, params.dz, L, z, params.controlParam,
            signals.out, phys.gamma, phys.beta2, params.bWdm);

        // Clamp to remaining length
        if (h <= 0.0 || z + h > L) {
            h = L - z;
        }

        // Symmetric Strang splitting: NL(h/2) -> Disp(h) -> NL(h/2)
        SSFM_Core::applyNonlinearity(signals.out, phys.gamma, h / 2.0, signals.out);
        SSFM_Core::applyDispersion(signals.out, fctx.betaOmega, phys.alphaNp, h, signals.out);
        SSFM_Core::applyNonlinearity(signals.out, phys.gamma, h / 2.0, signals.out);

        z += h;
    }

    // ---- Step 5: save output if requested ----
    if (params.saveFile != "noSave" && !params.saveFile.empty()) {
        std::ofstream outFile(params.saveFile + ".dat");
        if (outFile.is_open()) {
            outFile << std::scientific << std::setprecision(12);
            for (int r = 0; r < signals.out.rows(); ++r) {
                for (int c = 0; c < signals.out.cols(); ++c) {
                    outFile << signals.out(r, c).real() << " "
                            << signals.out(r, c).imag();
                    if (c < signals.out.cols() - 1) outFile << " ";
                }
                outFile << "\n";
            }
            outFile.close();
        }
    }
}

} // namespace ZK
