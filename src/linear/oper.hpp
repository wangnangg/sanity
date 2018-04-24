#include <cmath>
#include <type_traits>
#include "matrix_view.hpp"
#include "utils.hpp"
#include "vector_view.hpp"
namespace sanity::linear
{
void scale(Real a, VectorMutableView v);
void scale(Complex a, CVectorMutableView v);

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
