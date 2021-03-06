#pragma once
#include "lapack.hpp"
#include "matrix.hpp"
#include "permute.hpp"
#include "vector.hpp"
namespace sanity::linear
{
struct ResDecompQR
{
    Matrix Q;
    Matrix R;
};

// A = QR
ResDecompQR decompQR(MatrixMutableView A);
ResDecompQR decompQR(MatrixConstView A);
}  // namespace sanity::linear
