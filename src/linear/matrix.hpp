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

    MatrixMutableView mut()
    {
        return MatrixMutableView(&_data.front(), _nrow, _ncol, _ncol);
    }
};
}
