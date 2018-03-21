#pragma once
#include <cassert>
#include <type_traits>
#include "type.hpp"
namespace sanity::linear
{
template <typename DataT>
class MatrixViewBase;

template <typename DataT>
class MatrixMutViewBase;

template <>
class MatrixViewBase<Real>
{
protected:
    const Real* _data;
    int _nrow;
    int _ncol;
    int _ldim;
    bool _col_major;

public:
    MatrixViewBase(const Real* data, int nrow, int ncol, int ldim,
                   bool col_major, bool)
        : _data(data),
          _nrow(nrow),
          _ncol(ncol),
          _ldim(ldim),
          _col_major(col_major)
    {
    }

    const Real* data() const { return _data; }
    int nRow() const { return _nrow; }
    int nCol() const { return _ncol; }
    int lDim() const { return _ldim; }
    bool colMajor() const { return _col_major; }
    bool conjugated() const { return false; }
};

template <>
class MatrixMutViewBase<Real>
{
protected:
    Real* _data;
    int _nrow;
    int _ncol;
    int _ldim;

public:
    MatrixMutViewBase(Real* data, int nrow, int ncol, int ldim)
        : _data(data), _nrow(nrow), _ncol(ncol), _ldim(ldim)
    {
    }

    Real* data() const { return _data; }
    int nRow() const { return _nrow; }
    int nCol() const { return _ncol; }
    int lDim() const { return _ldim; }
};

template <>
class MatrixViewBase<Complex>
{
protected:
    const Complex* _data;
    int _nrow;
    int _ncol;
    int _ldim;
    bool _col_major;
    bool _conj;

public:
    MatrixViewBase(const Complex* data, int nrow, int ncol, int ldim,
                   bool col_major, bool conj)
        : _data(data),
          _nrow(nrow),
          _ncol(ncol),
          _ldim(ldim),
          _col_major(col_major),
          _conj(conj)
    {
    }

    const Complex* data() const { return _data; }

    int nRow() const { return _nrow; }
    int nCol() const { return _ncol; }
    int lDim() const { return _ldim; }
    bool colMajor() const { return _col_major; }
    bool conjugated() const { return _conj; }
};

template <>
class MatrixMutViewBase<Complex>
{
protected:
    Complex* _data;
    int _nrow;
    int _ncol;
    int _ldim;

public:
    MatrixMutViewBase(Complex* data, int nrow, int ncol, int ldim)
        : _data(data), _nrow(nrow), _ncol(ncol), _ldim(ldim)
    {
    }

    Complex* data() const { return _data; }

    int nRow() const { return _nrow; }
    int nCol() const { return _ncol; }
    int lDim() const { return _ldim; }
};
}
