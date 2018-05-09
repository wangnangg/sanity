#include <cmath>
#include <type_traits>
#include "matrix_view.hpp"
#include "utils.hpp"
#include "vector_view.hpp"
namespace sanity::linear
{
void fill(Real a, VectorMutableView v);
Real norm2(VectorConstView v);
Real norm1(VectorConstView v);
void scale(Real a, VectorMutableView v);
void scale(Complex a, CVectorMutableView v);

void scale(Real a, MatrixMutableView m);
void scale(Complex a, CMatrixMutableView m);

void swap(VectorMutableView x, VectorMutableView y);

Real dot(VectorConstView x, VectorConstView y);

// x = Av
void dot(MatrixConstView A, VectorConstView v, VectorMutableView x);
void dot(CMatrixConstView A, CVectorConstView v, CVectorMutableView x);
Vector dot(MatrixConstView A, VectorConstView v);
CVector dot(CMatrixConstView A, CVectorConstView v);

// C = AB
void dot(MatrixConstView A, MatrixConstView B, MatrixMutableView C);
Matrix dot(MatrixConstView A, MatrixConstView B);

// x = v .* w
void pointwiseProd(VectorConstView v, VectorConstView w, VectorMutableView x);
void pointwiseProd(CVectorConstView v, CVectorConstView w,
                   CVectorMutableView x);

// x = conj(v)
void conjuage(CVectorConstView v, CVectorMutableView x);
CVector conjuage(CVectorConstView v);
}  // namespace sanity::linear
