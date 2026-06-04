#ifndef SCHUR_H
#define SCHUR_H

#include "mat.h"
#include "tool.h"

namespace ZK
{

/*!
 * \ingroup matrixdecomp
 * \brief Schur decomposition of a real matrix
 *
 * This function computes the Schur form of a square real matrix
 * \f$ \mathbf{A} \f$. The Schur decomposition satisfies the
 * following equation:
 * \f[ \mathbf{U} \mathbf{T} \mathbf{U}^{T} = \mathbf{A} \f]
 * where: \f$ \mathbf{U} \f$ is a unitary, \f$ \mathbf{T} \f$ is upper
 * quasi-triangular, and \f$ \mathbf{U}^{T} \f$ is the transposed
 * \f$ \mathbf{U} \f$ matrix.
 *
 * The upper quasi-triangular matrix may have \f$ 2 \times 2 \f$ blocks on
 * its diagonal.
 *
 * Uses the LAPACK routine DGEES.
 */
ZKIC_DLL_EXPORT bool schur(const mat &A, mat &U, mat &T);

/*!
 * \ingroup matrixdecomp
 * \brief Schur decomposition of a real matrix
 *
 * This function computes the Schur form of a square real matrix
 * \f$ \mathbf{A} \f$. The Schur decomposition satisfies the
 * following equation:
 * \f[ \mathbf{U} \mathbf{T} \mathbf{U}^{T} = \mathbf{A} \f]
 * where: \f$ \mathbf{U} \f$ is a unitary, \f$ \mathbf{T} \f$ is upper
 * quasi-triangular, and \f$ \mathbf{U}^{T} \f$ is the transposed
 * \f$ \mathbf{U} \f$ matrix.
 *
 * The upper quasi-triangular matrix may have \f$ 2 \times 2 \f$ blocks on
 * its diagonal.
 *
 * \return  Real Schur matrix \f$ \mathbf{T} \f$
 *
 * uses the LAPACK routine DGEES.
 */
ZKIC_DLL_EXPORT mat schur(const mat &A);


/*!
 * \ingroup matrixdecomp
 * \brief Schur decomposition of a complex matrix
 *
 * This function computes the Schur form of a square complex matrix
 * \f$ \mathbf{A} \f$. The Schur decomposition satisfies
 * the following equation:
 * \f[ \mathbf{U} \mathbf{T} \mathbf{U}^{H} = \mathbf{A} \f]
 * where: \f$ \mathbf{U} \f$ is a unitary, \f$ \mathbf{T} \f$ is upper
 * triangular, and \f$ \mathbf{U}^{H} \f$ is the Hermitian
 * transposition of the \f$ \mathbf{U} \f$ matrix.
 *
 * Uses the LAPACK routine ZGEES.
 */
ZKIC_DLL_EXPORT bool schur(const cmat &A, cmat &U, cmat &T);

/*!
 * \ingroup matrixdecomp
 * \brief Schur decomposition of a complex matrix
 *
 * This function computes the Schur form of a square complex matrix
 * \f$ \mathbf{A} \f$. The Schur decomposition satisfies
 * the following equation:
 * \f[ \mathbf{U} \mathbf{T} \mathbf{U}^{H} = \mathbf{A} \f]
 * where: \f$ \mathbf{U} \f$ is a unitary, \f$ \mathbf{T} \f$ is upper
 * triangular, and \f$ \mathbf{U}^{H} \f$ is the Hermitian
 * transposition of the \f$ \mathbf{U} \f$ matrix.
 *
 * \return  Complex Schur matrix \f$ \mathbf{T} \f$
 *
 * Uses the LAPACK routine ZGEES.
 */
ZKIC_DLL_EXPORT cmat schur(const cmat &A);


} // namespace ZK

#endif // #ifndef SCHUR_H
