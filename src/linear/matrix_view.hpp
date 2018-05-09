#pragma once
#include <cassert>
#include "type.hpp"
#include "vector_view.hpp"
namespace sanity::linear
{
inline uint rowMajorIndex(uint i, uint j, uint ldim) { return i * ldim + j; }
inline uint colMajorIndex(uint i, uint j, uint ldim) { return i + j * ldim; }
class MatrixConstView
{
    const Real* _data;
    uint _nrow;
    uint _ncol;
    uint _ldim;

public:
    MatrixConstView(const Real* data, uint nrow, uint ncol, uint ldim)
        : _data(data), _nrow(nrow), _ncol(ncol), _ldim(ldim)
    {
    }
    const Real& operator()(uint i, uint j) const
    {
        assert(i < nrow() && j < ncol());
        return _data[rowMajorIndex(i, j, ldim())];
    }
    uint nrow() const { return _nrow; }
    uint ncol() const { return _ncol; }
    uint ldim() const { return _ldim; }
};

class MatrixMutableView
{
    Real* _data;
    uint _nrow;
    uint _ncol;
    uint _ldim;

public:
    MatrixMutableView(Real* data, uint nrow, uint ncol, uint ldim)
        : _data(data), _nrow(nrow), _ncol(ncol), _ldim(ldim)
    {
    }
    Real& operator()(uint i, uint j) const
    {
        assert(i < nrow() && j < ncol());
        return _data[rowMajorIndex(i, j, ldim())];
    }
    uint nrow() const { return _nrow; }
    uint ncol() const { return _ncol; }
    uint ldim() const { return _ldim; }
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

MatrixConstView blockView(MatrixConstView v, uint srow, uint scol, uint nrow,
                          uint ncol);
MatrixMutableView blockView(MatrixMutableView v, uint srow, uint scol,
                            uint nrow, uint ncol);

inline VectorConstView rowView(MatrixConstView m, uint rowIdx)
{
    return VectorConstView(&m(rowIdx, 0), 1, m.ncol());
}
inline VectorConstView colView(MatrixConstView m, uint colIdx)
{
    return VectorConstView(&m(0, colIdx), m.ldim(), m.nrow());
}
inline VectorMutableView rowView(MatrixMutableView m, uint rowIdx)
{
    return VectorMutableView(&m(rowIdx, 0), 1, m.ncol());
}
inline VectorMutableView colView(MatrixMutableView m, uint colIdx)
{
    return VectorMutableView(&m(0, colIdx), m.ldim(), m.nrow());
}

class CMatrixConstView
{
    const Complex* _data;
    uint _nrow;
    uint _ncol;
    uint _ldim;

public:
    CMatrixConstView(const Complex* data, uint nrow, uint ncol, uint ldim)
        : _data(data), _nrow(nrow), _ncol(ncol), _ldim(ldim)
    {
    }
    const Complex& operator()(uint i, uint j) const
    {
        assert(i < nrow() && j < ncol());
        return _data[rowMajorIndex(i, j, ldim())];
    }
    uint nrow() const { return _nrow; }
    uint ncol() const { return _ncol; }
    uint ldim() const { return _ldim; }
};

class CMatrixMutableView
{
    Complex* _data;
    uint _nrow;
    uint _ncol;
    uint _ldim;

public:
    CMatrixMutableView(Complex* data, uint nrow, uint ncol, uint ldim)
        : _data(data), _nrow(nrow), _ncol(ncol), _ldim(ldim)
    {
    }
    Complex& operator()(uint i, uint j) const
    {
        assert(i < nrow() && j < ncol());
        return _data[rowMajorIndex(i, j, ldim())];
    }
    uint nrow() const { return _nrow; }
    uint ncol() const { return _ncol; }
    uint ldim() const { return _ldim; }
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

CMatrixConstView blockView(CMatrixConstView v, uint srow, uint scol,
                           uint nrow, uint ncol);
CMatrixMutableView blockView(CMatrixMutableView v, uint srow, uint scol,
                             uint nrow, uint ncol);

inline CVectorConstView rowView(CMatrixConstView m, uint rowIdx)
{
    return CVectorConstView(&m(rowIdx, 0), 1, m.ncol());
}
inline CVectorConstView colView(CMatrixConstView m, uint colIdx)
{
    return CVectorConstView(&m(0, colIdx), m.ldim(), m.nrow());
}
inline CVectorMutableView rowView(CMatrixMutableView m, uint rowIdx)
{
    return CVectorMutableView(&m(rowIdx, 0), 1, m.ncol());
}
inline CVectorMutableView colView(CMatrixMutableView m, uint colIdx)
{
    return CVectorMutableView(&m(0, colIdx), m.ldim(), m.nrow());
}
}  // namespace sanity::linear
