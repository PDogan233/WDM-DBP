#ifndef DET_H
#define DET_H

#include "mat.h"
#include "tool.h"

namespace ZK
{

/*!
  \brief Determinant of real square matrix.
  \ingroup determinant

  Calculate determinant of the real matrix \f$\mathbf{X}\f$

  Uses LU-factorisation.
  \f[
  \det(\mathbf{X}) = \det(\mathbf{P}^T \mathbf{L}) \det(\mathbf{U}) = \det(\mathbf{P}^T) \prod(\mathrm{diag}(\mathbf{U}))
  \f]
  and the determinant of the permuation matrix is \f$ \pm 1\f$ depending on the number of row permutations
*/
ZKIC_DLL_EXPORT double det(const mat &X);


/*!
  \brief Determinant of complex square matrix.
  \ingroup determinant

  Calculate determinant of the complex matrix \f$\mathbf{X}\f$

  Uses LU-factorisation.
  \f[
  \det(\mathbf{X}) = \det(\mathbf{P}^T \mathbf{L}) \det(\mathbf{U}) = \det(\mathbf{P}^T) \prod(\mathrm{diag}(\mathbf{U}))
  \f]
  and the determinant of the permuation matrix is \f$ \pm 1\f$ depending on the number of row permutations
*/
ZKIC_DLL_EXPORT std::complex<double> det(const cmat &X);


} // namespace ZK

#endif // #ifndef DET_H
