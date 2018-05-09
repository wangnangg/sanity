#pragma once
#include <type.hpp>
#include <vector>
#include "matrix_view.hpp"

namespace sanity::linear
{
class Matrix
{
    std::vector<Real> _data;
    uint _nrow;
    uint _ncol;

public:
    Matrix() : _data(), _nrow(0), _ncol(0) {}
    Matrix(uint nrow, uint ncol, Real val = 0.0)
        : _data(nrow * ncol, val), _nrow(nrow), _ncol(ncol)
    {
    }
    Real& operator()(uint i, uint j)
    {
        assert(i < nrow() && j < ncol());
        return _data[rowMajorIndex(i, j, ncol())];
    }
    const Real& operator()(uint i, uint j) const
    {
        assert(i < nrow() && j < ncol());
        return _data[rowMajorIndex(i, j, ncol())];
    }
    uint nrow() const { return _nrow; }
    uint ncol() const { return _ncol; }

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
    uint _nrow;
    uint _ncol;

public:
    CMatrix() : _data(), _nrow(0), _ncol(0) {}
    CMatrix(uint nrow, uint ncol, Complex val = Complex())
        : _data(nrow * ncol, val), _nrow(nrow), _ncol(ncol)
    {
    }
    Complex& operator()(uint i, uint j)
    {
        assert(i < nrow() && j < ncol());
        return _data[rowMajorIndex(i, j, ncol())];
    }
    const Complex& operator()(uint i, uint j) const
    {
        assert(i < nrow() && j < ncol());
        return _data[(unsigned int)rowMajorIndex(i, j, ncol())];
    }
    uint nrow() const { return _nrow; }
    uint ncol() const { return _ncol; }

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
