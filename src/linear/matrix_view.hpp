#pragma once
#include <cassert>
#include "type.hpp"
namespace sanity::linear
{
inline int rowMajorIndex(int i, int j, int ldim) { return i * ldim + j; }
inline int colMajorIndex(int i, int j, int ldim) { return i + j * ldim; }
class MatrixConstView
{
    const Real* _data;
    int _nrow;
    int _ncol;
    int _ldim;

public:
    MatrixConstView(const Real* data, int nrow, int ncol, int ldim)
        : _data(data), _nrow(nrow), _ncol(ncol), _ldim(ldim)
    {
    }
    const Real& operator()(int i, int j) const
    {
        assert(i < nrow() && j < ncol());
        assert(i >= 0 && j >= 0);
        return _data[rowMajorIndex(i, j, ldim())];
    }
    int nrow() const { return _nrow; }
    int ncol() const { return _ncol; }
    int ldim() const { return _ldim; }
};

class MatrixMutableView
{
    Real* _data;
    int _nrow;
    int _ncol;
    int _ldim;

public:
    MatrixMutableView(Real* data, int nrow, int ncol, int ldim)
        : _data(data), _nrow(nrow), _ncol(ncol), _ldim(ldim)
    {
    }
    Real& operator()(int i, int j) const
    {
        assert(i < nrow() && j < ncol());
        assert(i >= 0 && j >= 0);
        return _data[rowMajorIndex(i, j, ldim())];
    }
    int nrow() const { return _nrow; }
    int ncol() const { return _ncol; }
    int ldim() const { return _ldim; }
    operator MatrixConstView() const
    {
        return MatrixConstView(_data, _nrow, _ncol, _ldim);
    }
};

MatrixConstView blockView(MatrixConstView v, int srow, int scol, int nrow = -1,
                          int ncol = -1);
MatrixMutableView blockView(MatrixMutableView v, int srow, int scol,
                            int nrow = -1, int ncol = -1);
}
