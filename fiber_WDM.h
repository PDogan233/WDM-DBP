#ifndef FIBER_WDM_H
#define FIBER_WDM_H

#include <vector>
#include <complex>
#include <string>

/*
 * fiber_WDM.h
 *
 * FiberWDM class for WDM propagation.
 *
 * Key ideas:
 *  - Keep class focused on fiber physics and SSF propagation.
 *  - Constructor takes physical params (with '_' suffix) and sampling (Nt, fs).
 *  - Build frequency and omega grid (Nt, fs) and precompute beta(omega) vector.
 */

 // -----------------------------
 // Type aliases for brevity
 // -----------------------------
using VecReal = std::vector<double>;
using VecComplex = std::vector<std::complex<double>>;
using Complex = std::complex<double>;

class FiberWDM {
public:
    // -----------------------------
    // Constructor and Destructor
    // -----------------------------
    // Constructor accepts physical parameters (use '_' suffix for arguments)
    FiberWDM(double L_span_,
        double alpha_dBpm_,
        double GroupRef_,
        double Dispersion_,
        double Dis_S_,
        double n2_,
        double Aeff_,
        double lambda0_,
        int Nt_,
        double fs_,
        const std::string& step_method_ = "constant",
        double control_param_ = 1e3);

    // Destructor (called automatically when object goes out of scope)
    ~FiberWDM();

    // -----------------------------
    // Public API
    // -----------------------------
    // Recompute NLSE derived parameters from physical parameters
    void computeNLSEParameters();

    // Build frequency and omega grid (must be called after Nt/fs set)
    void buildFrequencyGrid();

    // Main propagation routine
    // - x: input time-domain complex envelope (length Nt)
    // - L: propagation length (m), e.g. span length
    // - dz: fallback step size (m)
    // - control_param & method: step chooser controls
    // - B_wdm: total occupied bandwidth (Hz), used by step chooser
    VecComplex propagate(const VecComplex& x,
        double L,
        double dz,
        double control_param,
        const std::string& method,
        double B_wdm);

    // -----------------------------
    // Public members (for inspection)
    // -----------------------------
    double L_span;
    double alpha_dBpm;
    double GroupRef;
    double Dispersion;
    double Dis_S;
    double n2;
    double Aeff;
    double lambda0;

    // Derived NLSE params (public so tests can read them)
    double beta1;
    double beta2;
    double beta3;
    double gamma;
    double alpha_np; // attenuation in Neper/m

    // Sampling / FFT related
    int Nt;          // number of samples
    double fs;       // sampling frequency [Hz]
    double df;       // frequency spacing
    VecReal freq;    // frequency grid (fftshift ordering)
    VecReal omega;   // angular frequency grid (fftshift ordering)

    // -----------------------------
    // Static helper functions (FFT related)
    // -----------------------------
    // Simple wrappers using FFTW: behavior matches MATLAB's fft/ifft conventions:
    // - fft: forward transform without 1/N normalization
    // - ifft: inverse transform with 1/N normalization
    static VecComplex fft(const VecComplex& in);
    static VecComplex ifft(const VecComplex& in);

    // fftshift / ifftshift helpers
    static VecComplex fftshift(const VecComplex& in);
    static VecComplex ifftshift(const VecComplex& in);

private:
    // -----------------------------
    // Private helper methods (core of SSF)
    // -----------------------------
    VecComplex applyDispersion(const VecComplex& y_in, double h);
    VecComplex applyNonlinearity(const VecComplex& y_in, double h);
    double chooseStep(const std::string& method,
        double dz,
        double L,
        double z,
        double control_param,
        const VecComplex& y,
        double gamma_local,
        double beta2_local,
        double B_wdm);

    // Precomputed beta(omega) vector in fftshift order
    VecReal beta_omega;

    // Internal cleanup placeholder (if persistent resources added later)
    void cleanup();
};

#endif // FIBER_WDM_H
