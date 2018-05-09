#include "decomp.hpp"
#include <algorithm>
#include <iostream>
#include "utils.hpp"

namespace sanity::linear
{
ResDecompQR decompQR(MatrixMutableView A)
{
    uint m = A.nrow();
    uint k = std::min(A.nrow(), A.ncol());
    uint n = A.ncol();
    auto tau = Vector(k);
    int result = lapack::geqrf(A, mutableView(tau));
    assert(result == 0);

    ResDecompQR res;
    res.R = Matrix(k, n, 0.0);
    copy(Upper, A, mutableView(res.R));

    result = lapack::orgqr(A, tau);
    res.Q = Matrix(m, k, 0.0);
    copy(blockView(A, 0, 0, m, k), mutableView(res.Q));

    return res;
}

ResDecompQR decompQR(MatrixConstView A)
{
    auto work = createMatrix(A);
    return decompQR(mutableView(work));
}
}  // namespace sanity::linear
