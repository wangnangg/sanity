#pragma once
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

// PA = QR
ResDecompQR<Real> decompQR(MatrixView<Real, General> A);
ResDecompQR<Complex> decompQR(MatrixView<Complex, General> A);
}
