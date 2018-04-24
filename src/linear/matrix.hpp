#pragma once
#include <type.hpp>
#include <vector>
#include "matrix_view.hpp"

namespace sanity::linear
{
class Matrix
{
    std::vector<Real> _data;
    int _nrow;
    int _ncol;

public:
    Matrix() : _data(), _nrow(0), _ncol(0) {}
    Matrix(int nrow, int ncol, Real val = 0.0)
        : _data(static_cast<unsigned int>(nrow * ncol), val),
          _nrow(nrow),
          _ncol(ncol)
    {
    }
    Real& operator()(int i, int j)
    {
        assert(i < nrow() && j < ncol());
        assert(i >= 0 && j >= 0);
        return _data[(unsigned int)rowMajorIndex(i, j, ncol())];
    }
    const Real& operator()(int i, int j) const
    {
        assert(i < nrow() && j < ncol());
        assert(i >= 0 && j >= 0);
        return _data[(unsigned int)rowMajorIndex(i, j, ncol())];
    }
    int nrow() const { return _nrow; }
    int ncol() const { return _ncol; }

    operator MatrixConstView() const
    {
        return MatrixConstView(&_data.front(), _nrow, _ncol, _ncol);
    }
};

inline MatrixMutableView mutableView(Matrix& mat)
{
    return MatrixMutableView(&mat(0, 0), mat.nrow(), mat.ncol(), mat.ncol());
}
inline MatrixConstView constView(const Matrix& mat)
{
    return MatrixConstView(&mat(0, 0), mat.nrow(), mat.ncol(), mat.ncol());
}

class CMatrix
{
    std::vector<Complex> _data;
    int _nrow;
    int _ncol;

public:
    CMatrix() : _data(), _nrow(0), _ncol(0) {}
    CMatrix(int nrow, int ncol, Complex val = Complex())
        : _data(static_cast<unsigned int>(nrow * ncol), val),
          _nrow(nrow),
          _ncol(ncol)
    {
    }
    Complex& operator()(int i, int j)
    {
        assert(i < nrow() && j < ncol());
        assert(i >= 0 && j >= 0);
        return _data[(unsigned int)rowMajorIndex(i, j, ncol())];
    }
    const Complex& operator()(int i, int j) const
    {
        assert(i < nrow() && j < ncol());
        assert(i >= 0 && j >= 0);
        return _data[(unsigned int)rowMajorIndex(i, j, ncol())];
    }
    int nrow() const { return _nrow; }
    int ncol() const { return _ncol; }

    operator CMatrixConstView() const
    {
        return CMatrixConstView(&_data.front(), _nrow, _ncol, _ncol);
    }
};

inline CMatrixMutableView mutableView(CMatrix& mat)
{
    return CMatrixMutableView(&mat(0, 0), mat.nrow(), mat.ncol(), mat.ncol());
}
inline CMatrixConstView constView(const CMatrix& mat)
{
    return CMatrixConstView(&mat(0, 0), mat.nrow(), mat.ncol(), mat.ncol());
}

}  // namespace sanity::linear
