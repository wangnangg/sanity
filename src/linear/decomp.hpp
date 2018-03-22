#pragma once
#include "lapack.hpp"
#include "matrix.hpp"
#include "permute.hpp"
#include "vector.hpp"
namespace sanity::linear
{
template <typename DataT, MatrixStorage st>
struct ResDecompQR
{
    Matrix<DataT, st> Q;
    Matrix<DataT, st> R;
};

// A = QR
template <MatrixStorage st, Mutability mt>
ResDecompQR<Real, st> decompQR(MatrixView<Real, General, st, NoConj, Mutable> A)
{
    int m = A.nRow();
    int k = std::min(A.nRow(), A.nCol());
    int n = A.nCol();
    auto tau = Vector<Real>(k);
    int result = lapack::geqrf(A, mutView(tau));
    assert(result == 0);

    ResDecompQR<Real, st> res;
    res.R = Matrix<Real, st>(k, n, Real());
    copy(viewportCast<Upper>(A), viewportCast<Upper>(mutView(res.R)));

    result = lapack::orgqr(A, tau);
    res.Q = Matrix<Real, st>(m, k);
    copy(blockView(A, 0, 0, m, k), mutView(res.Q));

    return res;
}
}
