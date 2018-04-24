#pragma once
#include "matrix.hpp"
#include "matrix_view.hpp"
#include "vector.hpp"
namespace sanity::linear::blas
{
enum Oper
{
    NoTranspose,
    Transpose,
    ConjTranspose
};
Real dot(VectorConstView x, VectorConstView y);
Real nrm2(VectorConstView x);
Real asum(VectorConstView x);

int iamax(VectorConstView x);

void swap(VectorMutableView x, VectorMutableView y);
void copy(VectorConstView x, VectorMutableView y);
void axpy(Real a, VectorConstView x, VectorMutableView y);

// rotation omitted

void scal(Real a, VectorMutableView x);
void scal(Complex a, CVectorMutableView x);

// y:=a*op(A)*x+b*y
void gemv(Real a, MatrixConstView A, Oper op, VectorConstView x, Real b,
          VectorMutableView y);
void gemv(Complex a, CMatrixConstView A, Oper op, CVectorConstView x,
          Complex b, CVectorMutableView y);

// C := a * op(A) * op(B) + b * C
void gemm(Real a, MatrixConstView A, Oper opA, MatrixConstView B, Oper opB,
          Real b, MatrixMutableView C);

}  // namespace sanity::linear::blas
