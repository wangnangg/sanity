#pragma once
#include "lapack.hpp"
#include "matrix.hpp"
#include "permute.hpp"
#include "vector.hpp"
namespace sanity::linear
{
template <typename DataT>
struct ResDecompQR
{
    Matrix<DataT> Q;
    Matrix<DataT> R;
};

// A = QR
ResDecompQR<Real> decompQR(MatrixView<Real, General, Mutable> A);
}
