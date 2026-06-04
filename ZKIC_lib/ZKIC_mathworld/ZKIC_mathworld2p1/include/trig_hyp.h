#ifndef TRIG_HYP_H
#define TRIG_HYP_H

#include "help_functions.h"
#include "tool.h"

namespace ZK
{

//!\addtogroup trifunc
//!@{

//! Sinc function: sinc(x) = sin(pi*x)/pi*x
inline double sinc(double x)
{
  if (x == 0) {
    return 1.0;
  }
  else {
    double pix = ZK::pi * x;
    return std::sin(pix) / pix;
  }
}

//! Sine function
inline vec sin(const vec &x) { return apply_function<double>(std::sin, x); }
//! Sine function
inline mat sin(const mat &x) { return apply_function<double>(std::sin, x); }
//! Cosine function
inline vec cos(const vec &x) { return apply_function<double>(std::cos, x); }
//! Cosine function
inline mat cos(const mat &x) { return apply_function<double>(std::cos, x); }
//! Tan function
inline vec tan(const vec &x) { return apply_function<double>(std::tan, x); }
//! Tan function
inline mat tan(const mat &x) { return apply_function<double>(std::tan, x); }
//! Inverse sine function
inline vec asin(const vec &x) { return apply_function<double>(std::asin, x); }
//! Inverse sine function
inline mat asin(const mat &x) { return apply_function<double>(std::asin, x); }
//! Inverse cosine function
inline vec acos(const vec &x) { return apply_function<double>(std::acos, x); }
//! Inverse cosine function
inline mat acos(const mat &x) { return apply_function<double>(std::acos, x); }
//! Inverse tan function
inline vec atan(const vec &x) { return apply_function<double>(std::atan, x); }
//! Inverse tan function
inline mat atan(const mat &x) { return apply_function<double>(std::atan, x); }
//! Sinc function, sin(pi*x)/(pi*x)
inline vec sinc(const vec &x) { return apply_function<double>(sinc, x); }
//! Sinc function, sin(pi*x)/(pi*x)
inline mat sinc(const mat &x) { return apply_function<double>(sinc, x); }

//!@}


//!\addtogroup hypfunc
//!@{

//! Sine hyperbolic function
inline vec sinh(const vec &x) { return apply_function<double>(std::sinh, x); }
//! Sine hyperbolic function
inline mat sinh(const mat &x) { return apply_function<double>(std::sinh, x); }
//! Cosine hyperbolic function
inline vec cosh(const vec &x) { return apply_function<double>(std::cosh, x); }
//! Cosine hyperbolic function
inline mat cosh(const mat &x) { return apply_function<double>(std::cosh, x); }
//! Tan hyperbolic function
inline vec tanh(const vec &x) { return apply_function<double>(std::tanh, x); }
//! Tan hyperbolic function
inline mat tanh(const mat &x) { return apply_function<double>(std::tanh, x); }
//! Inverse sine hyperbolic function
ZKIC_DLL_EXPORT vec asinh(const vec &x);
//! Inverse sine hyperbolic function
ZKIC_DLL_EXPORT mat asinh(const mat &x);
//! Inverse cosine hyperbolic function
ZKIC_DLL_EXPORT vec acosh(const vec &x);
//! Inverse cosine hyperbolic function
ZKIC_DLL_EXPORT mat acosh(const mat &x);
//! Inverse tan hyperbolic function
ZKIC_DLL_EXPORT vec atanh(const vec &x);
//! Inverse tan hyperbolic function
ZKIC_DLL_EXPORT mat atanh(const mat &x);

//!@}

} // namespace ZK

#endif // #ifndef TRIG_HYP_H
