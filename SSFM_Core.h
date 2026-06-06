/*******************************************************************************
 * Copyright (c) 2026 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software
 * is strictly prohibited.
 ******************************************************************************/

/**
 * @file: SSFM_Core.h
 * @brief: Shared SSFM computation kernels for dual-polarization fiber propagation
 *         (Manakov equation). Provides nonlinear step, dispersion step, step-size
 *         control, and physical parameter conversion used by both DP_Fiber and DP_DBP.
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

#pragma once

#include <vec.h>
#include <mat.h>
#include <string>

namespace ZK {

// ============================================================
// FrequencyContext: precomputed frequency-domain parameters.
// Built once per execute() call and passed through the call
// chain via const reference.  No persistent state (Agent pattern).
// ============================================================
struct FrequencyContext {
    vec freq;        // frequency grid in fftshift order [Hz], length Nt
    vec omega;       // angular frequency grid [rad/s]
    vec betaOmega;  // beta(omega) = 0.5*beta2*omega^2 + beta3*omega^3/6
    double df;       // frequency spacing [Hz] = fs / Nt
};

// ============================================================
// FiberPhysics: derived physical parameters computed from
// raw fiber specifications (D, S, n2, Aeff, lambda0, ...).
// ============================================================
struct FiberPhysics {
    double alpha_dBpm;  // attenuation coefficient [dB/m]
    double beta1;       // 1st-order dispersion (group delay) [s/m]
    double beta2;       // 2nd-order dispersion (GVD) [s^2/m]
    double beta3;       // 3rd-order dispersion (dispersion slope) [s^3/m]
    double gamma;       // nonlinear coefficient [1/W/m]
    double alphaNp;     // attenuation coefficient [Neper/m] = ln(10^(alpha_dB/10))
};

// ============================================================
// SSFM_Core — Shared static computation kernels.
// NOT an Agent (no Parameters / Signals / execute).
// All methods are stateless; results are returned via reference
// parameters (Agent-pattern convention).
// ============================================================
class SSFM_Core {
public:
    // ----------------------------------------------------------
    // Compute beta1, beta2, beta3, gamma, alpha_np from
    // raw fiber physical parameters (D, S, n2, Aeff, lambda0).
    // ----------------------------------------------------------
    static void computeFiberPhysics(
        double alpha_dBpm,
        double groupRef,
        double dispersion,      // D  [s/m^2]
        double disS,            // S  [s/m^3]
        double n2,              // nonlinear refractive index [m^2/W]
        double aEff,            // effective area [m^2]
        double lambda0,         // reference wavelength [m]
        FiberPhysics& out);

    // ----------------------------------------------------------
    // Build the frequency grid and precompute beta(omega) for
    // every frequency bin.
    // ----------------------------------------------------------
    static void buildFrequencyContext(
        long long Nt,
        double fs,
        double beta2,
        double beta3,
        FrequencyContext& ctx);

    // ----------------------------------------------------------
    // Choose SSFM step size according to the specified method.
    // Returns h > 0 (caller clamps to remaining length).
    // ----------------------------------------------------------
    static double chooseStep(
        const std::string& method,
        double dz,              // default / fallback step [m]
        double L,               // total propagation length [m]
        double z,               // current position [m]
        double controlParam,    // target for adaptive methods
        const cmat& y,          // current field (2 x Nt) for Pmax
        double gamma,           // nonlinear coefficient [1/W/m]
        double beta2,           // GVD [s^2/m]
        double B_wdm);          // WDM bandwidth [Hz]

    // ----------------------------------------------------------
    // Apply Kerr nonlinearity — Manakov equation.
    // P_total = |A_x|^2 + |A_y|^2
    // phi = -(8/9) * gamma * P_total * h
    // yOut = yIn * exp(j * phi)
    // h > 0  -> forward,  h < 0  -> backward (DBP).
    // ----------------------------------------------------------
    static void applyNonlinearity(
        const cmat& yIn,
        double gamma,
        double h,
        cmat& yOut);

    // ----------------------------------------------------------
    // Apply dispersion + attenuation in the frequency domain.
    // H(omega) = exp((-alpha_np/2) * h  -  j * betaOmega * h)
    // h > 0  -> forward,  h < 0  -> backward (DBP).
    // ----------------------------------------------------------
    static void applyDispersion(
        const cmat& yIn,
        const vec& betaOmega,
        double alpha_np,
        double h,
        cmat& yOut);

private:
    // Manakov factor for randomly birefringent fibers
    static constexpr double MANAKOV_FACTOR = 8.0 / 9.0;
};

} // namespace ZK
