#include "utils.hpp"
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include "blas.hpp"
namespace sanity::linear
{
Real maxAbs(VectorConstView v)
{
    int max_idx = blas::iamax(v);
    return std::abs(v((uint)max_idx));
}
Real maxDiff(VectorConstView v1, VectorConstView v2)
{
    assert(v1.size() == v2.size());
    uint n = v1.size();
    Real max = 0;
    for (uint i = 0; i < n; i++)
    {
        Real v = std::abs(v1(i) - v2(i));
        if (max < v || std::isnan(v))
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
    uint m = mat1.nrow();
    uint n = mat1.ncol();
    Real max = 0;
    for (uint i = 0; i < m; i++)
    {
        for (uint j = 0; j < n; j++)
        {
            Real v = std::abs(mat1(i, j) - mat2(i, j));
            if (max < v || std::isnan(v))
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
    uint n = v1.size();
    Real max = 0;
    for (uint i = 0; i < n; i++)
    {
        Real v = std::abs(v1(i) - v2(i));
        if (max < v || std::isnan(v))
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
    uint m = mat1.nrow();
    uint n = mat1.ncol();
    Real max = 0;
    for (uint i = 0; i < m; i++)
    {
        for (uint j = 0; j < n; j++)
        {
            Real v = std::abs(mat1(i, j) - mat2(i, j));
            if (max < v || std::isnan(v))
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
    uint m = src.nrow();
    uint n = src.ncol();
    for (uint i = 0; i < m; i++)
    {
        for (uint j = 0; j < n; j++)
        {
            dst(i, j) = src(i, j);
        }
    }
}

void copy(VectorConstView src, VectorMutableView dst)
{
    assert(src.size() == dst.size());
    uint n = src.size();
    for (uint i = 0; i < n; i++)
    {
        dst(i) = src(i);
    }
}

void copy(CVectorConstView src, CVectorMutableView dst)
{
    assert(src.size() == dst.size());
    uint n = src.size();
    for (uint i = 0; i < n; i++)
    {
        dst(i) = src(i);
    }
}

void copy(MatrixViewport port, MatrixConstView src, MatrixMutableView dst)
{
    assert(src.nrow() == dst.nrow());
    assert(src.ncol() == dst.ncol());
    uint m = src.nrow();
    uint n = src.ncol();
    uint k = std::min(m, n);
    switch (port)
    {
        case General:
            copy(src, dst);
            break;
        case Upper:
            for (uint i = 0; i < k; i++)
            {
                for (uint j = i; j < n; j++)
                {
                    dst(i, j) = src(i, j);
                }
            }
            break;
        case Lower:
            for (uint j = 0; j < k; j++)
            {
                for (uint i = j; i < m; i++)
                {
                    dst(i, j) = src(i, j);
                }
            }
            break;
        case StrictUpper:
            for (uint i = 0; i < k; i++)
            {
                for (uint j = i + 1; j < n; j++)
                {
                    dst(i, j) = src(i, j);
                }
            }
            break;
        case StrictLower:
            for (uint j = 0; j < k; j++)
            {
                for (uint i = j + 1; i < m; i++)
                {
                    dst(i, j) = src(i, j);
                }
            }
            break;
        case Diagonal:
            for (uint i = 0; i < k; i++)
            {
                dst(i, i) = src(i, i);
            }
            break;
    }
}

std::ostream& operator<<(std::ostream& os, MatrixConstView mat)
{
    os << "[" << std::endl;
    for (uint i = 0; i < mat.nrow(); i++)
    {
        for (uint j = 0; j < mat.ncol(); j++)
        {
            os << std::defaultfloat << std::setw(12) << std::setprecision(5)
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
    for (uint i = 0; i < mat.nrow(); i++)
    {
        for (uint j = 0; j < mat.ncol(); j++)
        {
            std::ostringstream buf;
            buf << std::setprecision(4) << mat(i, j).real();
            if (mat(i, j).imag() >= 0)
            {
                buf << "+";
            }
            buf << mat(i, j).imag() << "i";
            os << std::defaultfloat << std::setw(14) << buf.str();
        }
        os << std::endl;
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, VectorConstView vec)
{
    os << "[ ";
    for (uint i = 0; i < vec.size(); i++)
    {
        os << std::setprecision(3) << vec(i) << " ";
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, CVectorConstView vec)
{
    os << "[ ";
    for (uint i = 0; i < vec.size(); i++)
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

std::ostream& operator<<(std::ostream& os, const Permutation& p)
{
    std::cout << "forward:\n";
    for (uint i = 0; i <= p.maxOrgIdx(); i++)
    {
        std::cout << "(" << i << "-->" << p.forward(i) << ")" << std::endl;
    }
    std::cout << "backward:\n";
    for (uint i = 0; i <= p.maxPermutedIdx(); i++)
    {
        std::cout << "(" << i << "-->" << p.backward(i) << ")" << std::endl;
    }
    return os;
}

Matrix createMatrix(uint nrow, uint ncol, const std::vector<Real>& v)
{
    assert(v.size() == (nrow * ncol));
    auto mat = Matrix(nrow, ncol);
    for (uint i = 0; i < nrow; i++)
    {
        for (uint j = 0; j < ncol; j++)
        {
            mat(i, j) = v[rowMajorIndex(i, j, ncol)];
        }
    }
    return mat;
}

Vector createVector(uint size, const std::vector<Real>& v)
{
    assert(size == v.size());
    auto vec = Vector(size);
    for (uint i = 0; i < size; i++)
    {
        vec(i) = v[i];
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
