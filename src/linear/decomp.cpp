#include "decomp.hpp"
#include <algorithm>
#include <iostream>
#include "utils.hpp"

namespace sanity::linear
{
ResDecompQR decompQR(MatrixMutableView A)
{
    int m = A.nrow();
    int k = std::min(A.nrow(), A.ncol());
    int n = A.ncol();
    auto tau = Vector(k);
    int result = lapack::geqrf(A, tau.mut());
    assert(result == 0);

    ResDecompQR res;
    res.R = Matrix(k, n, 0.0);
    copy(Upper, A, res.R.mut());

    result = lapack::orgqr(A, tau);
    res.Q = Matrix(m, k, 0.0);
    copy(blockView(A, 0, 0, m, k), res.Q.mut());

    return res;
}

ResDecompQR decompQR(MatrixConstView A)
{
    auto work = createMatrix(A);
    return decompQR(work.mut());
}
}
