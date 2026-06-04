#ifndef CHOLESKY_H
#define CHOLESKY_H

#include "mat.h"
#include "tool.h"

namespace ZK
{

/*! \addtogroup matrixdecomp
 */
//!@{

/*!
  \brief Cholesky factorisation of real symmetric and positive definite matrix

  The Cholesky factorisation of a real symmetric positive-definite matrix \f$\mathbf{X}\f$
  of size \f$n \times n\f$ is given by
  \f[
  \mathbf{X} = \mathbf{F}^T \mathbf{F}
  \f]
  where \f$\mathbf{F}\f$ is an upper triangular \f$n \times n\f$ matrix.

  Returns true if calculation succeeded. False otherwise.
*/
ZKIC_DLL_EXPORT bool chol(const mat &X, mat &F);

/*!
  \brief Cholesky factorisation of real symmetric and positive definite matrix

  The Cholesky factorisation of a real symmetric positive-definite matrix \f$\mathbf{X}\f$
  of size \f$n \times n\f$ is given by
  \f[
  \mathbf{X} = \mathbf{F}^T \mathbf{F}
  \f]
  where \f$\mathbf{F}\f$ is an upper triangular \f$n \times n\f$ matrix.
*/
ZKIC_DLL_EXPORT mat chol(const mat &X);


/*!
  \brief Cholesky factorisation of complex hermitian and positive-definite matrix

  The Cholesky factorisation of a hermitian positive-definite matrix \f$\mathbf{X}\f$
  of size \f$n \times n\f$ is given by
  \f[
  \mathbf{X} = \mathbf{F}^H \mathbf{F}
  \f]
  where \f$\mathbf{F}\f$ is an upper triangular \f$n \times n\f$ matrix.

  Returns true if calculation succeeded. False otherwise.

  If \c X is positive definite, true is returned and \c F=chol(X)
  produces an upper triangular \c F. If also \c X is symmetric then \c F'*F = X.
  If \c X is not positive definite, false is returned.
*/
ZKIC_DLL_EXPORT bool chol(const cmat &X, cmat &F);

/*!
  \brief Cholesky factorisation of complex hermitian and positive-definite matrix

  The Cholesky factorisation of a hermitian positive-definite matrix \f$\mathbf{X}\f$
  of size \f$n \times n\f$ is given by
  \f[
  \mathbf{X} = \mathbf{F}^H \mathbf{F}
  \f]
  where \f$\mathbf{F}\f$ is an upper triangular \f$n \times n\f$ matrix.
*/
ZKIC_DLL_EXPORT cmat chol(const cmat &X);

//!@}

} // namespace ZK

#endif // #ifndef CHOLESKY_H
