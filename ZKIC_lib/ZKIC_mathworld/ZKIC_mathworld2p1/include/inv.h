#ifndef INV_H
#define INV_H

#include "mat.h"
#include "tool.h"

namespace ZK
{

/*!
  \brief Inverse of real square matrix.
  \ingroup inverse

  Calculate the inverse of the real matrix \f$\mathbf{X}\f$

  Solves the equation system \f$ \mathbf{Y} \mathbf{X} = \mathbf{I}\f$ using LU-factorization.
  IT++ needs to be compiled with the LAPACK for the inverse to be available.
*/
ZKIC_DLL_EXPORT bool inv(const mat &X, mat &Y);

/*!
  \brief Inverse of real square matrix.
  \ingroup inverse

  Calculate the inverse of the real matrix \f$\mathbf{X}\f$

  Solves the equation system \f$ \mathbf{Y} \mathbf{X} = \mathbf{I}\f$ using LU-factorization.
  IT++ needs to be compiled with LAPACK support for the inverse to be available.
*/
ZKIC_DLL_EXPORT mat inv(const mat &X);


/*!
  \brief Inverse of complex square matrix.
  \ingroup inverse

  Calculate the inverse of the complex matrix \f$\mathbf{X}\f$

  Solves the equation system \f$ \mathbf{Y} \mathbf{X} = \mathbf{I}\f$ using LU-factorization.
  IT++ needs to be compiled with LAPACK support for the inverse to be available.
*/
ZKIC_DLL_EXPORT bool inv(const cmat &X, cmat &Y);

/*!
  \brief Inverse of real square matrix.
  \ingroup inverse

  Calculate the inverse of the complex matrix \f$\mathbf{X}\f$

  Solves the equation system \f$ \mathbf{Y} \mathbf{X} = \mathbf{I}\f$ using LU-factorization.
  IT++ needs to be compiled with the LAPACK for the inverse to be available.
*/
ZKIC_DLL_EXPORT cmat inv(const cmat &X);


} // namespace ZK

#endif // #ifndef INV_H
