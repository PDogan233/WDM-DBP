#ifndef ERROR_H
#define ERROR_H

#include "help_functions.h"
#include "tool.h"

namespace ZK
{

//!\addtogroup errorfunc
//!@{

/*!
 * \brief Error function for complex argument
 * \author Adam Piatyszek
 *
 * This function calculates a well known error function \c erf(z)
 * for complex \c z. The implementation is based on unofficial
 * implementation for Octave. Here is a part of the author's note
 * from original sources:
 *
 * Put together by John Smith john at arrows dot demon dot co dot uk,
 * using ideas by others.
 *
 * Calculate \c erf(z) for complex \c z.
 * Three methods are implemented; which one is used depends on z.
 *
 * The code includes some hard coded constants that are intended to
 * give about 14 decimal places of accuracy. This is appropriate for
 * 64-bit floating point numbers.
 */
ZKIC_DLL_EXPORT std::complex<double> erf(const std::complex<double>& z);

//! Inverse of error function
ZKIC_DLL_EXPORT double erfinv(double x);

//! Q-function
ZKIC_DLL_EXPORT double Qfunc(double x);


// ----------------------------------------------------------------------
// functions for matrices and vectors
// ----------------------------------------------------------------------

//! Error function
ZKIC_DLL_EXPORT vec erf(const vec &x);
//! Error function
ZKIC_DLL_EXPORT mat erf(const mat &x);
//! Error function
ZKIC_DLL_EXPORT cvec erf(const cvec &x);
//! Error function
ZKIC_DLL_EXPORT cmat erf(const cmat &x);

//! Inverse of error function
ZKIC_DLL_EXPORT vec erfinv(const vec &x);
//! Inverse of error function
ZKIC_DLL_EXPORT mat erfinv(const mat &x);

//! Complementary error function
ZKIC_DLL_EXPORT vec erfc(const vec &x);
//! Complementary error function
ZKIC_DLL_EXPORT mat erfc(const mat &x);

//! Q-function
ZKIC_DLL_EXPORT vec Qfunc(const vec &x);
//! Q-function
ZKIC_DLL_EXPORT mat Qfunc(const mat &x);
//!@}

} // namespace ZK

#endif // #ifndef ERROR_H




