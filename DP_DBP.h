/*******************************************************************************
 * Copyright (c) 2026 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software
 * is strictly prohibited.
 ******************************************************************************/

/**
 * @file: DP_DBP.h
 * @brief: Full-band Digital Back-Propagation (DBP) Agent.
 *         Reverses fiber propagation by running the SSFM with negative step size
 *         (h < 0) over the same physical parameters, canceling nonlinear and
 *         dispersive impairments.  Operates on the full wideband signal (no
 *         subband extraction).  No PMD.
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
 *     Implements full-band Digital Back-Propagation (DBP) for nonlinearity
 *     compensation.  Reverses the SSFM by using negative step size (h < 0),
 *     which automatically inverts dispersion and nonlinear phase signs.
 *     Processes spans in reverse order.  Operates on the full wideband signal.
 *
 * Example output after execution:
 *     Propagates the distorted dual-polarization optical field signals.oIn
 *     backward through Nspans spans, returning the compensated field in
 *     signals.out.
 */

class DP_DBP {
public:
    struct Parameters {
        // ---- fiber parameters (same physical model as DP_Fiber) ----
        double lSpan = 100e3;           // fiber span length [m]
        double alpha_dBpm = 0.2e-3;     // attenuation coefficient [dB/m]
        double groupRef = 1.47;         // group refractive index
        double dispersion = 16.5e-6;    // dispersion parameter D [s/m^2]
        double disS = 0.08e3;           // dispersion slope S [s/m^3]
        double n2 = 2.6e-20;            // nonlinear refractive index [m^2/W]
        double aEff = 80e-12;           // effective area [m^2]
        double lambda0 = 1550e-9;       // reference wavelength [m]

        // ---- propagation parameters ----
        long long nSpans = 1;           // number of spans to back-propagate
        double dz = 2e3;                // DBP step size [m] (magnitude)
        double gLin = 1.0;              // EDFA linear gain (1.0 = transparent, no EDFA)

        // ---- sampling parameters ----
        long long Nt = 0;               // number of time samples
        double fs = 0.0;                // sampling rate [Hz]

        // ---- file output ----
        std::string saveFile = "noSave"; // output .dat file path (skip if "noSave" or "")
    };

    struct Signals {
        cmat oIn;   // (2 x Nt) distorted optical field input
        cmat out;   // (2 x Nt) compensated optical field output
    };

    // === Do not modify the following statement ===
    static void execute(const Parameters& params, Signals& signals);

    // === Auxiliary functions (must be static) ===
    static void checkParam(const Parameters& params);
};

} // namespace ZK
