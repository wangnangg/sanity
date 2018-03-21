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
    int result = lapack::geqrf(mutView(work), mutView(tau));
    assert(result == 0);

    ResDecompQR<Real> res;
    res.R = Matrix<Real>(k, n, Real());
    copy(viewportCast<Upper>(work), viewportCast<Upper>(mutView(res.R)));

    result = lapack::orgqr(mutView(work), tau);
    res.Q = Matrix<Real>(m, k);
    copy(blockView(work, 0, 0, m, k), mutView(res.Q));

    return res;
}
}
