#pragma once
#include "matrix.hpp"
#include "matrix_view.hpp"
#include "vector.hpp"
namespace sanity::linear::blas
{
Real dot(VectorConstView x, VectorConstView y);
Real nrm2(VectorConstView x);
Real asum(VectorConstView x);

int iamax(VectorConstView x);

void swap(VectorMutableView x, VectorMutableView y);
void copy(VectorConstView x, VectorMutableView y);
void axpy(Real a, VectorConstView x, VectorMutableView y);

// rotation omitted

void scal(Real a, VectorMutableView x);

// y:=alpha*A*x+beta*y
void gemv(Real a, MatrixConstView A, VectorConstView x, Real b,
          VectorMutableView y);
}
