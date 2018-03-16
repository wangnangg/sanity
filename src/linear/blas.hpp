#pragma once
#include "matrix.hpp"
#include "matrix_view.hpp"
#include "vector.hpp"
namespace sanity::linear::blas
{
Real dot(VectorView<Real> x, VectorView<Real> y);
Complex dotc(VectorView<Complex> x, VectorView<Complex> y);
Complex dotu(VectorView<Complex> x, VectorView<Complex> y);

Real nrm2(VectorView<Real> x);
Real nrm2(VectorView<Complex> x);
Real asum(VectorView<Real> x);
Real asum(VectorView<Complex> x);

int iamax(VectorView<Real> x);
int iamax(VectorView<Complex> x);

void swap(VectorMutView<Real> x, VectorMutView<Real> y);
void swap(VectorMutView<Complex> x, VectorMutView<Complex> y);
void copy(VectorView<Real> x, VectorMutView<Real> y);
void copy(VectorView<Complex> x, VectorMutView<Complex> y);
void axpy(Real a, VectorView<Real> x, VectorMutView<Real> y);
void axpy(Complex a, VectorView<Complex> x, VectorMutView<Complex> y);

// rotation omitted

void scal(Real a, VectorMutView<Real> x);
void scal(Real a, VectorMutView<Complex> x);
void scal(Complex a, VectorMutView<Complex> x);

// y:=alpha*A*x+beta*y
void gemv(Real a, MatrixView<Real, General> A, VectorView<Real> x, Real b,
          VectorMutView<Real> y);
}
