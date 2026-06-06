/*******************************************************************************
 * Copyright (c) 2026 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software
 * is strictly prohibited.
 ******************************************************************************/

/**
 * @file: SSFM_Core.cpp
 * @brief: Implementation of shared SSFM computation kernels (Manakov nonlinearity,
 *         frequency-domain dispersion, step-size control, and physical parameter
 *         conversion).
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

#include <common_values.h>   // ZK::PHY_C, ZK::MA_PI
#include <converters.h>      // ZK::to_cvec
#include <elem_math.h>       // ZK::sqr
#include <fft.h>             // ZK::fftC2C, ZK::ifftC2C, ZK::fftShift, ZK::fftShiftInPlace
#include <log_exp.h>         // ZK::exp
#include <min_max.h>         // ZK::max
#include <specmat.h>         // ZK::linspace
#include <trig_hyp.h>        // ZK::cos, ZK::sin
#include <vec.h>             // ZK::vec, ZK::cvec, ZK::elem_mult
#include <mat.h>             // ZK::cmat

#include <cmath>             // std::sqrt, std::pow, std::exp, std::log
#include <algorithm>         // std::min

namespace ZK {

// ============================================================
// Static member function name:
//   void SSFM_Core::computeFiberPhysics(double, double, double, double,
//        double, double, double, FiberPhysics&)
//
// Input:
//   alpha_dBpm   // attenuation coefficient [dB/m]
//   groupRef     // group refractive index
//   dispersion   // dispersion parameter D [s/m^2]
//   disS        // dispersion slope S [s/m^3]
//   n2           // nonlinear refractive index [m^2/W]
//   aEff         // effective area [m^2]
//   lambda0      // reference wavelength [m]
//
// Output: void, result returned via out (FiberPhysics struct)
//
// Functional description:
//   Computes derived physical parameters (beta1, beta2, beta3, gamma, alphaNp)
//   from raw fiber specifications using standard fiber optics formulas.
// ============================================================
void SSFM_Core::computeFiberPhysics(
    double alpha_dBpm,
    double groupRef,
    double dispersion,
    double disS,
    double n2,
    double aEff,
    double lambda0,
    FiberPhysics& out)
{
    out.alpha_dBpm = alpha_dBpm;

    // beta1 = GroupRef / c
    out.beta1 = groupRef / PHY_C;

    // beta2 = -D * lambda0^2 / (2 * pi * c)
    out.beta2 = -dispersion * lambda0 * lambda0 / (2.0 * MA_PI * PHY_C);

    // beta3 = (lambda0^2 / (2*pi*c))^2 * (S + 2*D/lambda0)
    double factor = lambda0 * lambda0 / (2.0 * MA_PI * PHY_C);
    out.beta3 = factor * factor * (disS + 2.0 * dispersion / lambda0);

    // gamma = 2 * pi * n2 / (lambda0 * aEff)
    out.gamma = 2.0 * MA_PI * n2 / (lambda0 * aEff);

    // alphaNp = ln(10^(alpha_dBpm/10)) = (alpha_dBpm / 10) * ln(10)
    out.alphaNp = alpha_dBpm * std::log(10.0) / 10.0;
}

// ============================================================
// Static member function name:
//   void SSFM_Core::buildFrequencyContext(long long, double, double,
//        double, FrequencyContext&)
//
// Input:
//   Nt     // number of time samples
//   fs     // sampling rate [Hz]
//   beta2  // GVD [s^2/m]
//   beta3  // 3rd-order dispersion [s^3/m]
//
// Output: void, result returned via ctx (FrequencyContext struct)
//
// Functional description:
//   Builds the frequency grid in fftshift ordering and precomputes beta(omega)
//   for every frequency bin.  ZKIC does not provide fftfreq, so the grid is
//   built manually via linspace.
// ============================================================
void SSFM_Core::buildFrequencyContext(
    long long Nt,
    double fs,
    double beta2,
    double beta3,
    FrequencyContext& ctx)
{
    ctx.df = fs / static_cast<double>(Nt);

    // Build frequency grid in fftshift order: [-fs/2, -fs/2+df, ..., fs/2-df]
    ctx.freq = linspace(-fs / 2.0, fs / 2.0 - ctx.df, Nt);

    // Angular frequency grid
    ctx.omega = 2.0 * MA_PI * ctx.freq;

    // beta(omega) = 0.5 * beta2 * omega^2 + (1/6) * beta3 * omega^3
    ctx.betaOmega.set_size(Nt, false);
    for (long long i = 0; i < Nt; ++i) {
        double w = ctx.omega(i);
        ctx.betaOmega(i) = 0.5 * beta2 * w * w + (1.0 / 6.0) * beta3 * w * w * w;
    }
}

// ============================================================
// Static member function name:
//   double SSFM_Core::chooseStep(const std::string&, double, double,
//        double, double, const cmat&, double, double, double)
//
// Input:
//   method        // "constant" | "local_error" | "global_error"
//   dz            // default / fallback step size [m]
//   L             // total propagation length [m]
//   z             // current position [m] (unused, reserved for future use)
//   controlParam  // target value for adaptive methods (LE_target or GE_target)
//   y             // current optical field (2 x Nt), used for Pmax calculation
//   gamma         // nonlinear coefficient [1/W/m]
//   beta2         // GVD [s^2/m]
//   B_wdm         // total WDM bandwidth [Hz]
//
// Output: step size h [m] (always > 0)
//
// Functional description:
//   Selects the SSFM step size according to the specified method.
//   constant:        returns dz directly.
//   local_error:     h = (LE_target / denom)^(1/3)
//   global_error:    h = sqrt(GE_target / (L * denom))
//   where denom = gamma_eff * Pmax * 4*pi^2 * beta2^2 * B_wdm^4.
//   Falls back to dz when beta2 == 0 or Pmax == 0.
// ============================================================
double SSFM_Core::chooseStep(
    const std::string& method,
    double dz,
    double L,
    double z,
    double controlParam,
    const cmat& y,
    double gamma,
    double beta2,
    double B_wdm)
{
    // constant mode: always use the default step size
    if (method == "constant") {
        return dz;
    }

    // Pmax = max over time samples of (|A_x|^2 + |A_y|^2)
    vec P_total = sqr(y.get_row(0)) + sqr(y.get_row(1));
    double Pmax = max(P_total);
    if (Pmax <= 0.0) {
        return dz;          // zero input power, fall back to default
    }

    // Guard against zero dispersion (e.g., at zero-dispersion wavelength)
    if (beta2 == 0.0) {
        return dz;
    }

    double gamma_eff = MANAKOV_FACTOR * gamma;
    double four_pi2 = 4.0 * MA_PI * MA_PI;
    double denom = gamma_eff * Pmax * four_pi2 * (beta2 * beta2)
                 * std::pow(B_wdm, 4.0);

    if (denom <= 0.0) {
        return dz;
    }

    double h;
    if (method == "local_error") {
        // LE_target = controlParam
        h = std::pow(controlParam / denom, 1.0 / 3.0);
    } else if (method == "global_error") {
        // GE_target = controlParam
        h = std::sqrt(controlParam / (L * denom));
    } else {
        // unknown method, fall back to constant
        h = dz;
    }

    return h;
}

// ============================================================
// Static member function name:
//   void SSFM_Core::applyNonlinearity(const cmat&, double, double, cmat&)
//
// Input:
//   yIn    // input optical field (2 x Nt), row0 = X-pol, row1 = Y-pol
//   gamma  // nonlinear coefficient [1/W/m]
//   h      // step size [m] (h > 0 forward, h < 0 backward / DBP)
//
// Output: void, result returned via yOut (cmat&)
//
// Functional description:
//   Applies the Kerr nonlinear phase rotation according to the Manakov equation.
//   P_total(t) = |A_x(t)|^2 + |A_y(t)|^2
//   phi(t)     = -(8/9) * gamma * P_total(t) * h
//   A_{x,y}    = A_{x,y} * exp(j * phi)
//   The Manakov factor 8/9 accounts for randomly birefringent fibers.
//   h can be negative for DBP (back-propagation).
// ============================================================
void SSFM_Core::applyNonlinearity(
    const cmat& yIn,
    double gamma,
    double h,
    cmat& yOut)
{
    long long Nt = yIn.cols();

    // Ensure output has correct dimensions
    if (yOut.rows() != 2 || yOut.cols() != Nt) {
        yOut.set_size(2, Nt, false);
    }

    // Extract rows before modifying yOut (safe for in-place when &yIn == &yOut)
    cvec row0 = yIn.get_row(0);
    cvec row1 = yIn.get_row(1);

    // Total power per time sample: P_total = |A_x|^2 + |A_y|^2
    vec P_total = sqr(row0) + sqr(row1);

    // Nonlinear phase: phi = -(8/9) * gamma * P_total * h
    vec phase = -MANAKOV_FACTOR * gamma * h * P_total;

    // exp(j * phi) = cos(phi) + j * sin(phi)
    cvec expPhase = to_cvec(cos(phase), sin(phase));

    // Apply the same phase shift to both polarizations
    yOut.set_row(0, elem_mult(row0, expPhase));
    yOut.set_row(1, elem_mult(row1, expPhase));
}

// ============================================================
// Static member function name:
//   void SSFM_Core::applyDispersion(const cmat&, const vec&, double,
//        double, cmat&)
//
// Input:
//   yIn        // input optical field (2 x Nt)
//   betaOmega  // beta(omega) at each frequency bin [rad^2/m]
//   alphaNp   // attenuation coefficient [Neper/m]
//   h          // step size [m] (h > 0 forward, h < 0 backward / DBP)
//
// Output: void, result returned via yOut (cmat&)
//
// Functional description:
//   Applies dispersion and attenuation in the frequency domain.
//   Transfer function H(omega) = exp((-alphaNp/2) * h  -  j * betaOmega * h).
//   For each polarization row: FFT -> FFTSHIFT -> multiply by H -> IFFTSHIFT -> IFFT.
//   h can be negative (DBP), which automatically reverses dispersion and loss.
// ============================================================
void SSFM_Core::applyDispersion(
    const cmat& yIn,
    const vec& betaOmega,
    double alphaNp,
    double h,
    cmat& yOut)
{
    long long Nt = yIn.cols();

    // Ensure output has correct dimensions
    if (yOut.rows() != 2 || yOut.cols() != Nt) {
        yOut.set_size(2, Nt, false);
    }

    // Build transfer function H(omega) = exp((-alphaNp/2)*h  -  j*betaOmega*h)
    //   Real part: exp((-alphaNp/2)*h) *  cos(betaOmega * h)   (cos is even)
    //   Imag part: exp((-alphaNp/2)*h) * -sin(betaOmega * h)   (sin is odd)
    double atten = std::exp(-alphaNp * h / 2.0);
    vec theta = betaOmega * h;
    cvec H = to_cvec(atten * cos(theta), -atten * sin(theta));

    // Process each polarization independently (dispersion is scalar)
    for (int pol = 0; pol < 2; ++pol) {
        cvec row    = yIn.get_row(pol);     // extract one polarization
        cvec X      = fftC2C(row);          // forward FFT (no normalization)
        cvec Xs     = fftShift(X);          // shift DC to center
        cvec Ys     = elem_mult(Xs, H);     // apply transfer function
        fftShiftInPlace(Ys);                // inverse shift (= ifftshift, identity for even N)
        cvec result = ifftC2C(Ys);          // inverse FFT (auto /N normalization)
        yOut.set_row(pol, result);          // write back
    }
}

} // namespace ZK
