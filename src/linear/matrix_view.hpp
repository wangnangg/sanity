#pragma once
#include <cassert>
#include "type.hpp"
#include "vector_view.hpp"
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

inline MatrixConstView constView(MatrixMutableView mat)
{
    return MatrixConstView(&mat(0, 0), mat.nrow(), mat.ncol(), mat.ldim());
}

inline MatrixMutableView mutableView(MatrixMutableView mat) { return mat; }

MatrixConstView blockView(MatrixConstView v, int srow, int scol,
                          int nrow = -1, int ncol = -1);
MatrixMutableView blockView(MatrixMutableView v, int srow, int scol,
                            int nrow = -1, int ncol = -1);

inline VectorConstView rowView(MatrixConstView m, int rowIdx)
{
    return VectorConstView(&m(rowIdx, 0), 1, m.ncol());
}
inline VectorConstView colView(MatrixConstView m, int colIdx)
{
    return VectorConstView(&m(0, colIdx), m.ldim(), m.nrow());
}
inline VectorMutableView rowView(MatrixMutableView m, int rowIdx)
{
    return VectorMutableView(&m(rowIdx, 0), 1, m.ncol());
}
inline VectorMutableView colView(MatrixMutableView m, int colIdx)
{
    return VectorMutableView(&m(0, colIdx), m.ldim(), m.nrow());
}

class CMatrixConstView
{
    const Complex* _data;
    int _nrow;
    int _ncol;
    int _ldim;

public:
    CMatrixConstView(const Complex* data, int nrow, int ncol, int ldim)
        : _data(data), _nrow(nrow), _ncol(ncol), _ldim(ldim)
    {
    }
    const Complex& operator()(int i, int j) const
    {
        assert(i < nrow() && j < ncol());
        assert(i >= 0 && j >= 0);
        return _data[rowMajorIndex(i, j, ldim())];
    }
    int nrow() const { return _nrow; }
    int ncol() const { return _ncol; }
    int ldim() const { return _ldim; }
};

class CMatrixMutableView
{
    Complex* _data;
    int _nrow;
    int _ncol;
    int _ldim;

public:
    CMatrixMutableView(Complex* data, int nrow, int ncol, int ldim)
        : _data(data), _nrow(nrow), _ncol(ncol), _ldim(ldim)
    {
    }
    Complex& operator()(int i, int j) const
    {
        assert(i < nrow() && j < ncol());
        assert(i >= 0 && j >= 0);
        return _data[rowMajorIndex(i, j, ldim())];
    }
    int nrow() const { return _nrow; }
    int ncol() const { return _ncol; }
    int ldim() const { return _ldim; }
    operator CMatrixConstView() const
    {
        return CMatrixConstView(_data, _nrow, _ncol, _ldim);
    }
};

inline CMatrixConstView constView(CMatrixMutableView mat)
{
    return CMatrixConstView(&mat(0, 0), mat.nrow(), mat.ncol(), mat.ldim());
}

inline CMatrixMutableView mutableView(CMatrixMutableView mat) { return mat; }

CMatrixConstView blockView(CMatrixConstView v, int srow, int scol,
                           int nrow = -1, int ncol = -1);
CMatrixMutableView blockView(CMatrixMutableView v, int srow, int scol,
                             int nrow = -1, int ncol = -1);

inline CVectorConstView rowView(CMatrixConstView m, int rowIdx)
{
    return CVectorConstView(&m(rowIdx, 0), 1, m.ncol());
}
inline CVectorConstView colView(CMatrixConstView m, int colIdx)
{
    return CVectorConstView(&m(0, colIdx), m.ldim(), m.nrow());
}
inline CVectorMutableView rowView(CMatrixMutableView m, int rowIdx)
{
    return CVectorMutableView(&m(rowIdx, 0), 1, m.ncol());
}
inline CVectorMutableView colView(CMatrixMutableView m, int colIdx)
{
    return CVectorMutableView(&m(0, colIdx), m.ldim(), m.nrow());
}
}  // namespace sanity::linear
