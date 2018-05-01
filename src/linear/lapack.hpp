#pragma once
#include "matrix.hpp"
#include "matrix_view.hpp"
#include "permute.hpp"
#include "vector.hpp"

namespace sanity::linear::lapack
{
int geqrf(MatrixMutableView A, VectorMutableView tau);
int orgqr(MatrixMutableView mA, VectorConstView tau);

int gesv(MatrixMutableView A, MatrixMutableView B,
         std::vector<int> *perm = nullptr);

}  // namespace sanity::linear::lapack
