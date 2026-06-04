#ifndef BESSEL_H
#define BESSEL_H

#include "vec.h"


namespace ZK
{

/*! \addtogroup besselfunctions
 */

/*!
  \ingroup besselfunctions
  \brief Bessel function of first kind of order \a nu for \a nu integer

  The bessel function of first kind is defined as:
  \f[
  J_{\nu}(x) = \sum_{k=0}^{\infty} \frac{ (-1)^{k} }{k! \Gamma(\nu+k+1) } \left(\frac{x}{2}\right)^{\nu+2k}
  \f]
  where \f$\nu\f$ is the order and \f$ 0 < x < \infty \f$.
*/
ZKIC_DLL_EXPORT double besselj(int nu, double x);

/*!
  \ingroup besselfunctions
  \brief Bessel function of first kind of order \a nu for \a nu integer
*/
ZKIC_DLL_EXPORT vec besselj(int nu, const vec &x);

/*!
  \ingroup besselfunctions
  \brief Bessel function of first kind of order \a nu. \a nu is real.
*/
ZKIC_DLL_EXPORT double besselj(double nu, double x);

/*!
  \ingroup besselfunctions
  \brief Bessel function of first kind of order \a nu. \a nu is real.
*/
ZKIC_DLL_EXPORT vec besselj(double nu, const vec &x);

/*!
  \ingroup besselfunctions
  \brief Bessel function of second kind of order \a nu. \a nu is integer.

  The Bessel function of second kind is defined as:
  \f[
  Y_{\nu}(x) = \frac{J_{\nu}(x) \cos(\nu\pi) - J_{-\nu}(x)}{\sin(\nu\pi)}
  \f]
  where \f$\nu\f$ is the order and \f$ 0 < x < \infty \f$.
*/
ZKIC_DLL_EXPORT double bessely(int nu, double x);

/*!
  \ingroup besselfunctions
  \brief Bessel function of second kind of order \a nu. \a nu is integer.
*/
ZKIC_DLL_EXPORT vec bessely(int nu, const vec &x);

/*!
  \ingroup besselfunctions
  \brief Bessel function of second kind of order \a nu. \a nu is real.
*/
ZKIC_DLL_EXPORT double bessely(double nu, double x);

/*!
  \ingroup besselfunctions
  \brief Bessel function of second kind of order \a nu. \a nu is real.
*/
ZKIC_DLL_EXPORT vec bessely(double nu, const vec &x);

/*!
  \ingroup besselfunctions
  \brief Modified Bessel function of first kind of order \a nu. \a nu is \a double. \a x is \a double.

  The Modified Bessel function of first kind is defined as:
  \f[
  I_{\nu}(x) = i^{-\nu} J_{\nu}(ix)
  \f]
  where \f$\nu\f$ is the order and \f$ 0 < x < \infty \f$.
*/
ZKIC_DLL_EXPORT double besseli(double nu, double x);

/*!
  \ingroup besselfunctions
  \brief Modified Bessel function of first kind of order \a nu. \a nu is \a double. \a x is \a double.
*/
ZKIC_DLL_EXPORT vec besseli(double nu, const vec &x);

/*!
  \ingroup besselfunctions
  \brief Modified Bessel function of second kind of order \a nu. \a nu is double. \a x is double.

  The Modified Bessel function of second kind is defined as:
  \f[
  K_{\nu}(x) = \frac{\pi}{2} i^{\nu+1} [J_{\nu}(ix) + i Y_{\nu}(ix)]
  \f]
  where \f$\nu\f$ is the order and \f$ 0 < x < \infty \f$.
*/
ZKIC_DLL_EXPORT double besselk(int nu, double x);

/*!
  \ingroup besselfunctions
  \brief Modified Bessel function of second kind of order \a nu. \a nu is double. \a x is double.
*/
ZKIC_DLL_EXPORT vec besselk(int nu, const vec &x);

} //namespace ZK

#endif // #ifndef BESSEL_H
