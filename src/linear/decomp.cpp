#include "decomp.hpp"
#include <algorithm>
#include <iostream>
#include "utils.hpp"

namespace sanity::linear
{
ResDecompQR<Real> decompQR(MatrixView<Real, General, Mutable> A)
{
    int m = A.nRow();
    int k = std::min(A.nRow(), A.nCol());
    int n = A.nCol();
    auto tau = Vector<Real>(k);
    int result = lapack::geqrf(A, mutView(tau));
    assert(result == 0);

    ResDecompQR<Real> res;
    res.R = Matrix<Real>(k, n, Real());
    copy(viewportCast<Upper>(A), viewportCast<Upper>(mutView(res.R)));

    result = lapack::orgqr(A, tau);
    res.Q = Matrix<Real>(m, k);
    copy(blockView(A, 0, 0, m, k), mutView(res.Q));

    return res;
}
}
