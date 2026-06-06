/*******************************************************************************
 * Copyright (c) 2026 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software
 * is strictly prohibited.
 ******************************************************************************/

/**
 * @file: DP_DBP.cpp
 * @brief: Implementation of the DP_DBP Agent — full-band Digital Back-Propagation
 *         for nonlinearity compensation.  Reverses the SSFM with h < 0.
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

#include "DP_DBP.h"
#include "SSFM_Core.h"

#include <cmath>       // std::sqrt
#include <fstream>     // std::ofstream
#include <iomanip>     // std::scientific, std::setprecision
#include <stdexcept>   // std::invalid_argument

namespace ZK {

// ============================================================
// Static member function name:
//   void DP_DBP::checkParam(const Parameters&)
//
// Input:
//   params  // DP_DBP::Parameters, fiber and propagation parameters
//
// Output: void (throws std::invalid_argument on invalid input)
//
// Functional description:
//   Validates required parameters: Nt > 0, fs > 0, L_span >= 0, Nspans >= 0,
//   G_lin > 0.
// ============================================================
void DP_DBP::checkParam(const Parameters& params)
{
    if (params.Nt <= 0) {
        throw std::invalid_argument("DP_DBP: Nt must be > 0");
    }
    if (params.fs <= 0.0) {
        throw std::invalid_argument("DP_DBP: fs must be > 0");
    }
    if (params.lSpan < 0.0) {
        throw std::invalid_argument("DP_DBP: lSpan must be >= 0");
    }
    if (params.nSpans < 0) {
        throw std::invalid_argument("DP_DBP: nSpans must be >= 0");
    }
    if (params.gLin <= 0.0) {
        throw std::invalid_argument("DP_DBP: gLin must be > 0");
    }
}

// ============================================================
// Static member function name:
//   void DP_DBP::execute(const Parameters&, Signals&)
//
// Input:
//   params  // DP_DBP::Parameters — fiber physics, propagation, sampling
//   signals // DP_DBP::Signals — oIn: (2 x Nt) distorted optical field
//
// Output: void, compensated field stored in signals.out
//
// Functional description:
//   Executes full-band DBP by reversing the SSFM.
//   Processes spans in reverse order (Nspans-1 down to 0).
//   Within each span: remove EDFA gain, then run SSFM with negative h.
//   h < 0 automatically inverts dispersion (gain instead of loss) and
//   nonlinear phase (compensation instead of impairment).
//   Uses the same SSFM_Core kernels as DP_Fiber — only h differs.
// ============================================================
void DP_DBP::execute(const Parameters& params, Signals& signals)
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

    // ---- Step 4: reverse span loop ----
    const double sqrtG = std::sqrt(params.gLin);
    const double L = params.lSpan;

    for (long long spanIdx = params.nSpans - 1; spanIdx >= 0; --spanIdx) {
        // Remove EDFA gain (if G_lin == 1.0, this is a no-op)
        for (int r = 0; r < signals.out.rows(); ++r) {
            for (int c = 0; c < signals.out.cols(); ++c) {
                signals.out(r, c) = signals.out(r, c) / sqrtG;
            }
        }

        // Backward SSFM loop with negative step size
        double z = 0.0;
        while (z < L - 1e-12) {
            double h_step = params.dz;
            if (z + h_step > L) {
                h_step = L - z;
            }

            // h < 0 for back-propagation: inverts dispersion and nonlinear signs
            double h = -h_step;

            // Symmetric Strang splitting: NL(h/2) -> Disp(h) -> NL(h/2)
            SSFM_Core::applyNonlinearity(signals.out, phys.gamma, h / 2.0, signals.out);
            SSFM_Core::applyDispersion(signals.out, fctx.betaOmega, phys.alphaNp, h, signals.out);
            SSFM_Core::applyNonlinearity(signals.out, phys.gamma, h / 2.0, signals.out);

            z += h_step;
        }
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
