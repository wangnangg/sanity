#pragma once
#include "linear.hpp"
#include "matrix.hpp"
#include "type.hpp"

namespace sanity::splinear
{
IterationResult solveSor(const Spmatrix& A, linear::VectorMutableView x,
                         linear::VectorConstView b, Real w, Real tol,
                         uint max_iter);
}
