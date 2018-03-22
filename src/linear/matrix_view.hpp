#pragma once
#include <cassert>
#include "type.hpp"
namespace sanity::linear
{
enum MatrixViewport
{
    General,
    Diagonal,
    StrictUpper,
    Upper,
    StrictLower,
    Lower
};

enum MatrixStorage
{
    RowMajor,
    ColMajor
};

constexpr bool subViewOf(MatrixViewport v1, MatrixViewport v2)
{
    switch (v1)
    {
        case General:
            return v2 == General;
        case Diagonal:
            return v2 == General || v2 == Diagonal || v2 == Upper ||
                   v2 == Lower;
        case StrictUpper:
            return v2 == General || v2 == StrictUpper || v2 == Upper;
        case Upper:
            return v2 == General || v2 == Upper;
        case StrictLower:
            return v2 == General || v2 == StrictLower || v2 == Lower;
        case Lower:
            return v2 == General || v2 == Lower;
    }
}

template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct,
          Mutability mt>
class MatrixViewBase;

template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct>
class MatrixViewBase<DataT, vt, st, ct, Const>
{
public:
    using DataType = DataT;
    static const MatrixViewport viewport = vt;
    static const MatrixStorage storage = st;
    static const Conjugation conjugation = ct;
    static const Mutability mutability = Const;

protected:
    const Real* _data;
    int _nrow;
    int _ncol;
    int _ldim;

public:
    MatrixViewBase(const Real* data, int nrow, int ncol, int ldim)
        : _data(data), _nrow(nrow), _ncol(ncol), _ldim(ldim)
    {
    }

    const Real* data() const { return _data; }
    int nRow() const { return _nrow; }
    int nCol() const { return _ncol; }
    int lDim() const { return _ldim; }
};

template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct>
class MatrixViewBase<DataT, vt, st, ct, Mutable>
{
public:
    using DataType = DataT;
    static const MatrixViewport viewport = vt;
    static const MatrixStorage storage = st;
    static const Conjugation conjugation = ct;
    static const Mutability mutability = Mutable;

protected:
    Real* _data;
    int _nrow;
    int _ncol;
    int _ldim;

public:
    MatrixViewBase(Real* data, int nrow, int ncol, int ldim)
        : _data(data), _nrow(nrow), _ncol(ncol), _ldim(ldim)
    {
    }

    Real* data() const { return _data; }
    int nRow() const { return _nrow; }
    int nCol() const { return _ncol; }
    int lDim() const { return _ldim; }
};

template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct,
          Mutability mt>
class MatrixView;

template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct>
class MatrixView<DataT, vt, st, ct, Const>
    : public MatrixViewBase<DataT, vt, st, ct, Const>
{
public:
    using MatrixViewBase<DataT, vt, st, ct, Const>::MatrixViewBase;
};

template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct>
class MatrixView<DataT, vt, st, ct, Mutable>
    : public MatrixViewBase<DataT, vt, st, ct, Mutable>
{
public:
    using MatrixViewBase<DataT, vt, st, ct, Mutable>::MatrixViewBase;
};

template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct,
          Mutability mt>
bool has(MatrixView<DataT, vt, st, ct, mt> mat, int i, int j);

template <typename DataT, MatrixStorage st, Conjugation ct, Mutability mt>
bool has(MatrixView<DataT, General, st, ct, mt> mat, int i, int j)
{
    return true;
}

template <typename DataT, MatrixStorage st, Conjugation ct, Mutability mt>
bool has(MatrixView<DataT, Diagonal, st, ct, mt> mat, int i, int j);
template <typename DataT, MatrixStorage st, Conjugation ct, Mutability mt>
bool has(MatrixView<DataT, StrictUpper, st, ct, mt> mat, int i, int j);
template <typename DataT, MatrixStorage st, Conjugation ct, Mutability mt>
bool has(MatrixView<DataT, Upper, st, ct, mt> mat, int i, int j);
template <typename DataT, MatrixStorage st, Conjugation ct, Mutability mt>
bool has(MatrixView<DataT, StrictLower, st, ct, mt> mat, int i, int j);
template <typename DataT, MatrixStorage st, Conjugation ct, Mutability mt>
bool has(MatrixView<DataT, Lower, st, ct, mt> mat, int i, int j);

template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct,
          Mutability mt>
decltype(auto) addressOf(MatrixView<DataT, vt, st, ct, mt> mat, int i, int j);

template <typename DataT, MatrixViewport vt, Conjugation ct, Mutability mt>
decltype(auto) addressOf(MatrixView<DataT, vt, RowMajor, ct, mt> mat, int i,
                         int j)
{
    assert(has(mat, i, j));
    return (mat.data() + i * mat.lDim() + j);
}

template <typename DataT, MatrixViewport vt, Conjugation ct, Mutability mt>
decltype(auto) addressOf(MatrixView<DataT, vt, ColMajor, ct, mt> mat, int i,
                         int j)
{
    assert(has(mat, i, j));
    return (mat.data() + j * mat.lDim() + i);
}

template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct,
          Mutability mt>
DataT get(MatrixView<DataT, vt, st, ct, mt> mat, int i, int j);

template <typename DataT, MatrixViewport vt, MatrixStorage st, Mutability mt>
DataT get(MatrixView<DataT, vt, st, NoConj, mt> mat, int i, int j)
{
    assert(has(mat, i, j));
    return *addressOf(mat, i, j);
}
template <typename DataT, MatrixViewport vt, MatrixStorage st, Mutability mt>
DataT get(MatrixView<DataT, vt, st, Conj, mt> mat, int i, int j)
{
    assert(has(mat, i, j));
    return std::conj(*addressOf(mat, i, j));
}

template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct>
void set(MatrixView<DataT, vt, st, ct, Mutable> mat, int i, int j, DataT val);

template <typename DataT, MatrixViewport vt, MatrixStorage st>
void set(MatrixView<DataT, vt, st, NoConj, Mutable> mat, int i, int j,
         DataT val)
{
    assert(has(mat, i, j));
    *addressOf(mat, i, j) = val;
}

template <typename DataT, MatrixViewport vt, MatrixStorage st>
void set(MatrixView<DataT, vt, st, Conj, Mutable> mat, int i, int j, DataT val)
{
    assert(has(mat, i, j));
    *addressOf(mat, i, j) = std::conj(val);
}

template <typename DataT, MatrixStorage st, Conjugation ct, Mutability mt>
MatrixView<DataT, General, st, ct, mt> blockView(
    MatrixView<DataT, General, st, ct, mt> mat, int st_row, int st_col,
    int ed_row = -1, int ed_col = -1)
{
    if (ed_row < 0)
    {
        ed_row += mat.nRow() + 1;
    }
    if (ed_col < 0)
    {
        ed_col += mat.nCol() + 1;
    }
    assert(st_row <= ed_row);
    assert(ed_row <= mat.nRow());
    assert(st_col <= ed_col);
    assert(ed_col <= mat.nCol());
    const DataT* start_p = addressOf(mat, st_row, st_col);
    return MatrixView<DataT, General, st, ct, mt>(start_p, ed_row - st_row,
                                                  ed_col - st_col, mat.lDim());
}

template <
    MatrixViewport target, MatrixViewport src, typename DataT, MatrixStorage st,
    Conjugation ct, Mutability mt,
    std::enable_if_t<
        (src == General) ||
            (src == Upper && (target == StrictUpper || target == Diagonal)) ||
            (src == Lower && (target == StrictLower || target == Diagonal)),
        int> = 0>
MatrixView<DataT, target, st, ct, mt> viewportCast(
    MatrixView<DataT, src, st, ct, mt> m)
{
    return MatrixView<DataT, target, st, ct, mt>(m.data(), m.nRow(), m.nCol(),
                                                 m.lDim());
}

template <typename DataT, MatrixViewport vt, Conjugation ct, MatrixStorage st>
MatrixView<DataT, vt, st, ct, Const> constView(
    MatrixView<DataT, vt, st, ct, Mutable> m)
{
    return MatrixView<DataT, vt, st, ct, Const>(m.data(), m.nRow(), m.nCol(),
                                                m.lDim());
}

constexpr MatrixViewport transViewport(MatrixViewport vt)
{
    switch (vt)
    {
        case Upper:
            return Lower;
        case Lower:
            return Upper;
        case StrictUpper:
            return StrictLower;
        case StrictLower:
            return StrictUpper;
        default:
            return vt;
    }
}

constexpr MatrixStorage invertStorage(MatrixStorage st)
{
    if (st == ColMajor)
    {
        return RowMajor;
    }
    else
    {
        return ColMajor;
    }
}

template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct,
          Mutability mt>
auto transpose(MatrixView<DataT, vt, st, ct, mt> mat)
{
    return MatrixView<DataT, transViewport(vt), invertStorage(st), ct, mt>(
        mat.data(), mat.nCol(), mat.nRow(), mat.lDim());
}


template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct,
          Mutability mt>
auto conjugate(MatrixView<DataT, vt, st, ct, mt> mat);

template <MatrixViewport vt, MatrixStorage st, Conjugation ct, Mutability mt>
auto conjugate(MatrixView<Real, vt, st, ct, mt> mat)
{
    return mat;
}

template <MatrixViewport vt, MatrixStorage st, Conjugation ct, Mutability mt>
auto conjugate(MatrixView<Complex, vt, st, ct, mt> mat)
{
    return MatrixView<Complex, vt, st, invertConj(ct), mt>(
        mat.data(), mat.nRow(), mat.nCol(), mat.lDim());
}
}
