#include "utils.hpp"
#include <algorithm>
#include <cassert>
#include <iomanip>
#include "blas.hpp"
namespace sanity::linear
{
Real maxAbs(VectorConstView v)
{
    int max_idx = blas::iamax(v);
    return std::abs(v(max_idx));
}
Real maxDiff(VectorConstView v1, VectorConstView v2)
{
    assert(v1.size() == v2.size());
    int n = v1.size();
    Real max = 0;
    for (int i = 0; i < n; i++)
    {
        Real v = std::abs(v1(i) - v2(i));
        if (max < v)
        {
            max = v;
        }
    }
    return max;
}

Real maxDiff(MatrixConstView mat1, MatrixConstView mat2)
{
    assert(mat1.nrow() == mat2.nrow());
    assert(mat1.ncol() == mat2.ncol());
    int m = mat1.nrow();
    int n = mat1.ncol();
    Real max = 0;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            Real v = std::abs(mat1(i, j) - mat2(i, j));
            if (max < v)
            {
                max = v;
            }
        }
    }
    return max;
}

Real maxDiff(CVectorConstView v1, CVectorConstView v2)
{
    assert(v1.size() == v2.size());
    int n = v1.size();
    Real max = 0;
    for (int i = 0; i < n; i++)
    {
        Real v = std::abs(v1(i) - v2(i));
        if (max < v)
        {
            max = v;
        }
    }
    return max;
}
Real maxDiff(CMatrixConstView mat1, CMatrixConstView mat2)
{
    assert(mat1.nrow() == mat2.nrow());
    assert(mat1.ncol() == mat2.ncol());
    int m = mat1.nrow();
    int n = mat1.ncol();
    Real max = 0;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            Real v = std::abs(mat1(i, j) - mat2(i, j));
            if (max < v)
            {
                max = v;
            }
        }
    }
    return max;
}

void copy(MatrixConstView src, MatrixMutableView dst)
{
    assert(src.nrow() == dst.nrow());
    assert(src.ncol() == dst.ncol());
    int m = src.nrow();
    int n = src.ncol();
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            dst(i, j) = src(i, j);
        }
    }
}

void copy(VectorConstView src, VectorMutableView dst)
{
    assert(src.size() == dst.size());
    int n = src.size();
    for (int i = 0; i < n; i++)
    {
        dst(i) = src(i);
    }
}

void copy(CVectorConstView src, CVectorMutableView dst)
{
    assert(src.size() == dst.size());
    int n = src.size();
    for (int i = 0; i < n; i++)
    {
        dst(i) = src(i);
    }
}

void copy(MatrixViewport port, MatrixConstView src, MatrixMutableView dst)
{
    assert(src.nrow() == dst.nrow());
    assert(src.ncol() == dst.ncol());
    int m = src.nrow();
    int n = src.ncol();
    int k = std::min(m, n);
    switch (port)
    {
        case General:
            copy(src, dst);
            break;
        case Upper:
            for (int i = 0; i < k; i++)
            {
                for (int j = i; j < n; j++)
                {
                    dst(i, j) = src(i, j);
                }
            }
            break;
        case Lower:
            for (int j = 0; j < k; j++)
            {
                for (int i = j; i < m; i++)
                {
                    dst(i, j) = src(i, j);
                }
            }
            break;
        case StrictUpper:
            for (int i = 0; i < k; i++)
            {
                for (int j = i + 1; j < n; j++)
                {
                    dst(i, j) = src(i, j);
                }
            }
            break;
        case StrictLower:
            for (int j = 0; j < k; j++)
            {
                for (int i = j + 1; i < m; i++)
                {
                    dst(i, j) = src(i, j);
                }
            }
            break;
        case Diagonal:
            for (int i = 0; i < k; i++)
            {
                dst(i, i) = src(i, i);
            }
            break;
    }
}

std::ostream& operator<<(std::ostream& os, MatrixConstView mat)
{
    os << "[" << std::endl;
    for (int i = 0; i < mat.nrow(); i++)
    {
        for (int j = 0; j < mat.ncol(); j++)
        {
            os << std::fixed << std::setw(12) << std::setprecision(5)
               << std::setfill(' ') << mat(i, j);
        }
        os << std::endl;
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, CMatrixConstView mat)
{
    os << "[" << std::endl;
    for (int i = 0; i < mat.nrow(); i++)
    {
        for (int j = 0; j < mat.ncol(); j++)
        {
            std::ostringstream buf;
            buf << std::setprecision(4) << mat(i, j).real();
            if (mat(i, j).imag() >= 0)
            {
                buf << "+";
            }
            buf << mat(i, j).imag() << "i";
            os << std::fixed << std::setw(14) << buf.str();
        }
        os << std::endl;
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, VectorConstView vec)
{
    os << "[ ";
    for (int i = 0; i < vec.size(); i++)
    {
        os << std::setprecision(3) << vec(i) << " ";
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, CVectorConstView vec)
{
    os << "[ ";
    for (int i = 0; i < vec.size(); i++)
    {
        os << std::setprecision(3) << vec(i).real();
        if (vec(i).imag() >= 0)
        {
            os << "+";
        }
        os << vec(i).imag() << "i ";
    }
    os << "]";
    return os;
}

Matrix createMatrix(int nrow, int ncol, const std::vector<Real>& v)
{
    assert(v.size() == (unsigned int)(nrow * ncol));
    auto mat = Matrix(nrow, ncol);
    for (int i = 0; i < nrow; i++)
    {
        for (int j = 0; j < ncol; j++)
        {
            mat(i, j) = v[(unsigned int)rowMajorIndex(i, j, ncol)];
        }
    }
    return mat;
}

Vector createVector(int size, const std::vector<Real>& v)
{
    assert((unsigned int)size == v.size());
    auto vec = Vector(size);
    for (int i = 0; i < size; i++)
    {
        vec(i) = v[(unsigned int)i];
    }
    return vec;
}

Matrix createMatrix(MatrixConstView mat)
{
    auto newmat = Matrix(mat.nrow(), mat.ncol());
    copy(mat, mutableView(newmat));
    return newmat;
}
}  // namespace sanity::linear
