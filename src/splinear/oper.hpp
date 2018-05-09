#pragma once
#include "linear.hpp"
#include "matrix.hpp"
namespace sanity::splinear
{
enum Oper
{
    NoTranspose,
    Transpose,
    ConjTranspose
};

void dot(const Spmatrix& A, linear::VectorConstView v,
         linear::VectorMutableView x);
void dotpx(const Spmatrix& A, linear::VectorConstView v,
           linear::VectorMutableView x);

}  // namespace sanity::splinear
