#pragma once
#include "matrix.hpp"
#include "vector.hpp"

namespace sanity::linear
{
// b will be rewritten
void solveLU(MatrixMutableView A, VectorMutableView x);

}  // namespace sanity::linear
