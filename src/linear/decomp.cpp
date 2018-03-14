#include "decomp.hpp"
#include <algorithm>
#include <iostream>
#include "lapack.hpp"
#include "utils.hpp"

namespace sanity::linear
{
ResDecompQR<Real> decompQR(MatrixView<Real, General> A)
{
    int m = A.nRow();
    int k = std::min(A.nRow(), A.nCol());
    int n = A.nCol();
    auto work = Matrix<Real>(A);
    auto tau = Vector<Real>(k);
    int result = lapack::geqrf(work.mutView(), tau.mutView());
    assert(result == 0);

    ResDecompQR<Real> res;
    res.R = Matrix<Real>(k, n, Real());
    copyFrom(work.constView<Upper>(), res.R.mutView<Upper>());

    result = lapack::orgqr(work.mutView(), tau);
    res.Q = Matrix<Real>(m, k);
    copyFrom(work.blockView(0, 0, m, k), res.Q.mutView());

    return res;
}
}
