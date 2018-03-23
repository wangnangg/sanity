#pragma once
#include "matrix.hpp"
#include "matrix_view.hpp"
#include "vector.hpp"
namespace sanity::linear::blas
{
Real dot(VectorView<Real, Const> x, VectorView<Real, Const> y);
Complex dotu(VectorView<Complex, Const> x, VectorView<Complex, Const> y);
Complex dotc(VectorView<Complex, Const> x, VectorView<Complex, Const> y);

Real nrm2(VectorView<Real, Const> x);
Real nrm2(VectorView<Complex, Const> x);
Real asum(VectorView<Real, Const> x);
Real asum(VectorView<Complex, Const> x);

int iamax(VectorView<Real, Const> x);
int iamax(VectorView<Complex, Const> x);

void swap(VectorView<Real, Mutable> x, VectorView<Real, Mutable> y);
void swap(VectorView<Complex, Mutable> x, VectorView<Complex, Mutable> y);
void copy(VectorView<Real, Const> x, VectorView<Real, Mutable> y);
void copy(VectorView<Complex, Const> x, VectorView<Complex, Mutable> y);
void axpy(Real a, VectorView<Real, Const> x, VectorView<Real, Mutable> y);
void axpy(Complex a, VectorView<Complex, Const> x,
          VectorView<Complex, Mutable> y);

// rotation omitted

void scal(Real a, VectorView<Real, Mutable> x);
void scal(Real a, VectorView<Complex, Mutable> x);
void scal(Complex a, VectorView<Complex, Mutable> x);

// y:=alpha*A*x+beta*y
void gemv(Real a, MatrixView<Real, General, Const> A, VectorView<Real, Const> x,
          Real b, VectorView<Real, Mutable> y);

void gemv(Complex a, MatrixView<Complex, General, Const> A,
          VectorView<Complex, Const> x, Complex b,
          VectorView<Complex, Mutable> y);

// // x := A * x
// void trmv(MatrixView<Real, Upper> A, VectorMutView<Real> x);
// void trmv(MatrixView<Real, Lower> A, VectorMutView<Real> x);
// // unit diagonal
// void trmv(MatrixView<Real, StrictUpper> A, VectorMutView<Real> x);
// void trmv(MatrixView<Real, StrictLower> A, VectorMutView<Real> x);
// // x := A * x
// void trmv(MatrixView<Complex, Upper> A, VectorMutView<Complex> x);
// void trmv(MatrixView<Complex, Lower> A, VectorMutView<Complex> x);
// // unit diagonal
// void trmv(MatrixView<Complex, StrictUpper> A, VectorMutView<Complex> x);
// void trmv(MatrixView<Complex, StrictLower> A, VectorMutView<Complex> x);

// // solve A x = b
// void trsv(MatrixView<Real, Upper> A, VectorMutView<Real> x);
// void trsv(MatrixView<Real, Lower> A, VectorMutView<Real> x);
// // unit diagonal
// void trsv(MatrixView<Real, StrictUpper> A, VectorMutView<Real> x);
// void trsv(MatrixView<Real, StrictLower> A, VectorMutView<Real> x);
// // solve A x = b
// void trsv(MatrixView<Complex, Upper> A, VectorMutView<Complex> x);
// void trsv(MatrixView<Complex, Lower> A, VectorMutView<Complex> x);
// // unit diagonal
// void trsv(MatrixView<Complex, StrictUpper> A, VectorMutView<Complex> x);
// void trsv(MatrixView<Complex, StrictLower> A, VectorMutView<Complex> x);

// // y := a * A * x + b * y
// void symv(Real a, MatrixView<Real, Upper> A, VectorView<Real> x, Real b,
//           VectorMutView<Real> y);
// void symv(Real a, MatrixView<Real, Lower> A, VectorView<Real> x, Real b,
//           VectorMutView<Real> y);

// // A := a*x*y'+ A
// void ger(Real a, VectorView<Real> x, VectorView<Real> y,
//          MatrixMutView<Real, General> A);

// // A := a*x*x' + A
// void syr(Real a, VectorView<Real> x, MatrixMutView<Real, Upper> A);
// void syr(Real a, VectorView<Real> x, MatrixMutView<Real, Lower> A);

// // A := a*x*y'+ a*y*x' + A
// void syr2(Real a, VectorView<Real> x, VectorView<Real> y,
//           MatrixMutView<Real, Upper> A);
// void syr2(Real a, VectorView<Real> x, VectorView<Real> y,
//           MatrixMutView<Real, Lower> A);

// // y := a * A * x + b * y
// void hemv(Complex a, MatrixView<Complex, Upper> A, VectorView<Complex> x,
//           Complex b, VectorMutView<Complex> y);
// void hemv(Complex a, MatrixView<Complex, Lower> A, VectorView<Complex> x,
//           Complex b, VectorMutView<Complex> y);

// // A := a*x*y' + A
// void ger(Complex a, VectorView<Complex> x, VectorView<Complex> y,
//          MatrixMutView<Complex, General> A);

// // A := a*x*conj(x') + A
// void her(Complex a, VectorView<Complex> x, MatrixMutView<Complex, Upper> A);
// void her(Complex a, VectorView<Complex> x, MatrixMutView<Complex, Lower> A);

// // A := a *x*conjg(y') + conjg(a)*y *conjg(x') + A,
// void her2(Complex a, VectorView<Complex> x, VectorView<Complex> y,
//           MatrixMutView<Complex, Upper> A);
// void her2(Complex a, VectorView<Complex> x, VectorView<Complex> y,
//           MatrixMutView<Complex, Lower> A);
//
}
