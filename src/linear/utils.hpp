#pragma once
#include <ostream>
#include "matrix.hpp"
#include "matrix_view.hpp"
#include "vector.hpp"
#include "vector_view.hpp"

namespace sanity::linear
{
enum MatrixViewport
{
    General,
    Upper,
    Lower,
    StrictUpper,
    StrictLower,
    Diagonal
};
Real maxDiff(VectorConstView v1, VectorConstView v2);
Real maxDiff(MatrixConstView mat1, MatrixConstView mat2);

Matrix createMatrix(int nrow, int ncol, const std::vector<Real>& v);
Matrix createMatrix(MatrixConstView mat);
Vector createVector(int size, const std::vector<Real>& v);

void copy(VectorConstView src, VectorMutableView dst);
void copy(MatrixConstView src, MatrixMutableView dst);
void copy(MatrixViewport port, MatrixConstView src, MatrixMutableView dst);

std::ostream& operator<<(std::ostream& os, MatrixConstView mat);
std::ostream& operator<<(std::ostream& os, VectorConstView vec);
}
