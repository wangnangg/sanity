#pragma once
#include "matrix.hpp"
#include "matrix_view.hpp"
#include "vector.hpp"
namespace sanity::linear::blas
{
Real dot(VectorView<Real, NoConj, Const> x, VectorView<Real, NoConj, Const> y);
Complex dot(VectorView<Complex, NoConj, Const> x,
            VectorView<Complex, NoConj, Const> y);
Complex dot(VectorView<Complex, Conj, Const> x,
            VectorView<Complex, NoConj, Const> y);

Real nrm2(VectorView<Real, NoConj, Const> x);
Real nrm2(VectorView<Complex, NoConj, Const> x);
Real nrm2(VectorView<Complex, Conj, Const> x);
Real asum(VectorView<Real, NoConj, Const> x);
Real asum(VectorView<Complex, NoConj, Const> x);
Real asum(VectorView<Complex, Conj, Const> x);

int iamax(VectorView<Real, NoConj, Const> x);
int iamax(VectorView<Complex, NoConj, Const> x);
int iamax(VectorView<Complex, Conj, Const> x);

void swap(VectorView<Real, NoConj, Mutable> x,
          VectorView<Real, NoConj, Mutable> y);
void swap(VectorView<Complex, NoConj, Mutable> x,
          VectorView<Complex, NoConj, Mutable> y);
void swap(VectorView<Complex, Conj, Mutable> x,
          VectorView<Complex, Conj, Mutable> y);
void copy(VectorView<Real, NoConj, Const> x,
          VectorView<Real, NoConj, Mutable> y);
void copy(VectorView<Complex, NoConj, Const> x,
          VectorView<Complex, NoConj, Mutable> y);
void copy(VectorView<Complex, Conj, Const> x,
          VectorView<Complex, Conj, Mutable> y);
void axpy(Real a, VectorView<Real, NoConj, Const> x,
          VectorView<Real, NoConj, Mutable> y);
void axpy(Complex a, VectorView<Complex, NoConj, Const> x,
          VectorView<Complex, NoConj, Mutable> y);

// rotation omitted

void scal(Real a, VectorView<Real, NoConj, Mutable> x);
void scal(Real a, VectorView<Complex, NoConj, Mutable> x);
void scal(Real a, VectorView<Complex, Conj, Mutable> x);
void scal(Complex a, VectorView<Complex, NoConj, Mutable> x);

// y:=alpha*A*x+beta*y
void gemv(Real a, MatrixView<Real, General, RowMajor, NoConj, Const> A,
          VectorView<Real, NoConj, Const> x, Real b,
          VectorView<Real, NoConj, Mutable> y);
void gemv(Real a, MatrixView<Real, General, ColMajor, NoConj, Const> A,
          VectorView<Real, NoConj, Const> x, Real b,
          VectorView<Real, NoConj, Mutable> y);

void gemv(Complex a, MatrixView<Complex, General, RowMajor, NoConj, Const> A,
          VectorView<Complex, NoConj, Const> x, Complex b,
          VectorView<Complex, NoConj, Mutable> y);
void gemv(Complex a, MatrixView<Complex, General, RowMajor, Conj, Const> A,
          VectorView<Complex, NoConj, Const> x, Complex b,
          VectorView<Complex, NoConj, Mutable> y);
void gemv(Complex a, MatrixView<Complex, General, ColMajor, NoConj, Const> A,
          VectorView<Complex, NoConj, Const> x, Complex b,
          VectorView<Complex, NoConj, Mutable> y);
void gemv(Complex a, MatrixView<Complex, General, ColMajor, Conj, Const> A,
          VectorView<Complex, NoConj, Const> x, Complex b,
          VectorView<Complex, NoConj, Mutable> y);

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
