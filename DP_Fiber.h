/*******************************************************************************
 * Copyright (c) 2026 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software
 * is strictly prohibited.
 ******************************************************************************/

/**
 * @file: DP_Fiber.h
 * @brief: Forward dual-polarization fiber propagation Agent.
 *         Implements the symmetric Split-Step Fourier Method (SSFM) for the
 *         Manakov equation.  Supports constant, local_error, and global_error
 *         step-size control.  No PMD, no EDFA.
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

#include <mat.h>     // ZK::cmat
#include <string>

namespace ZK {

/*
 * Agent class functional description:
 *     Implements forward dual-polarization fiber propagation using the
 *     symmetric SSFM (Strang splitting) for the Manakov equation.
 *     Supports X/Y dual-polarization with cross-phase modulation.
 *     Three step-size control modes: constant, local_error, global_error.
 *     No PMD, no EDFA in this phase.
 *
 * Example output after execution:
 *     Propagates the input dual-polarization optical field signals.oIn
 *     over the specified distance, returning the propagated field in
 *     signals.out.
 */

class DP_Fiber {
public:
    struct Parameters {
        // ---- physical (fiber) parameters ----
        double lSpan = 100e3;           // fiber span length [m]
        double alpha_dBpm = 0.2e-3;     // attenuation coefficient [dB/m]
        double groupRef = 1.47;         // group refractive index
        double dispersion = 16.5e-6;    // dispersion parameter D [s/m^2]
        double disS = 0.08e3;           // dispersion slope S [s/m^3]
        double n2 = 2.6e-20;            // nonlinear refractive index [m^2/W]
        double aEff = 80e-12;           // effective area [m^2]
        double lambda0 = 1550e-9;       // reference wavelength [m]

        // ---- numerical parameters ----
        double dz = 2e3;                // default SSFM step size [m]
        std::string method = "constant"; // step control: "constant"|"local_error"|"global_error"
        double controlParam = 1e3;      // target value for adaptive step methods

        // ---- sampling parameters ----
        long long Nt = 0;               // number of time samples
        double fs = 0.0;                // sampling rate [Hz]
        double bWdm = 0.0;              // total WDM bandwidth [Hz] (for step control)

        // ---- file output ----
        std::string saveFile = "noSave"; // output .dat file path (skip if "noSave" or "")
    };

    struct Signals {
        cmat oIn;   // (2 x Nt) input optical field: row0 = X-pol, row1 = Y-pol
        cmat out;   // (2 x Nt) output optical field after propagation
    };

    // === Do not modify the following statement ===
    static void execute(const Parameters& params, Signals& signals);

    // === Auxiliary functions (must be static) ===
    static void checkParam(const Parameters& params);
};

} // namespace ZK
