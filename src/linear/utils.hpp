#pragma once
#include <ostream>
#include "matrix.hpp"
#include "matrix_view.hpp"
#include "permute.hpp"
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
Real maxDiff(CVectorConstView v1, CVectorConstView v2);
Real maxDiff(CMatrixConstView mat1, CMatrixConstView mat2);

Real maxAbs(VectorConstView v);
Real maxAbs(CVectorConstView v);

Matrix createMatrix(int nrow, int ncol, const std::vector<Real>& v);
Matrix createMatrix(MatrixConstView mat);
Vector createVector(int size, const std::vector<Real>& v);

void copy(VectorConstView src, VectorMutableView dst);
void copy(CVectorConstView src, CVectorMutableView dst);
void copy(MatrixConstView src, MatrixMutableView dst);
void copy(MatrixViewport port, MatrixConstView src, MatrixMutableView dst);

std::ostream& operator<<(std::ostream& os, MatrixConstView mat);
std::ostream& operator<<(std::ostream& os, CMatrixConstView mat);
std::ostream& operator<<(std::ostream& os, VectorConstView vec);
std::ostream& operator<<(std::ostream& os, CVectorConstView vec);

std::ostream& operator<<(std::ostream& os, const Permutation& p);

}  // namespace sanity::linear
