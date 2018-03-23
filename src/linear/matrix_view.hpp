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

template <typename DataT, MatrixViewport vt, Mutability mt>
class MatrixView;

template <MatrixViewport vt>
bool has(int i, int j)
{
    switch (vt)
    {
        case General:
            return true;
        case StrictUpper:
            if (i < j)
                return true;
            else
                return false;
        case Upper:
            if (i <= j)
                return true;
            else
                return false;
        case StrictLower:
            if (i > j)
                return true;
            else
                return false;
        case Lower:
            if (i >= j)
                return true;
            else
                return false;
        case Diagonal:
            if (i == j)
                return true;
            else
                return false;
    }
}

template <typename DataT, MatrixViewport vt, Mutability mt>
decltype(auto) addressOf(MatrixView<DataT, vt, mt> mat, int i, int j)
{
    return (mat.data() + i * mat.lDim() + j);
}

template <typename DataT, MatrixViewport vt>
const DataT& get(MatrixView<DataT, vt, Const> mat, int i, int j)
{
    assert(has<vt>(i, j));
    return *addressOf(mat, i, j);
}

template <typename DataT, MatrixViewport vt>
DataT& get(MatrixView<DataT, vt, Mutable> mat, int i, int j)
{
    assert(has<vt>(i, j));
    return *addressOf(mat, i, j);
}

template <typename DataT, typename DataT2, MatrixViewport vt>
void set(MatrixView<DataT, vt, Mutable> mat, int i, int j, DataT2 val)
{
    assert(has<vt>(i, j));
    *addressOf(mat, i, j) = DataT(val);
}

template <typename DataT, Mutability mt>
MatrixView<DataT, General, mt> blockView(MatrixView<DataT, General, mt> mat,
                                         int st_row, int st_col,
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
    return MatrixView<DataT, General, mt>(addressOf(mat, st_row, st_col),
                                          ed_row - st_row, ed_col - st_col,
                                          mat.lDim());
}

template <
    MatrixViewport target, MatrixViewport src, typename DataT, Mutability mt,
    std::enable_if_t<
        (src == General) ||
            (src == Upper && (target == StrictUpper || target == Diagonal)) ||
            (src == Lower && (target == StrictLower || target == Diagonal)),
        int> = 0>
auto viewportCast(MatrixView<DataT, src, mt> m)
{
    return MatrixView<DataT, target, mt>(m.data(), m.nRow(), m.nCol(),
                                         m.lDim());
}

template <typename DataT, MatrixViewport vt>
auto constView(MatrixView<DataT, vt, Mutable> m)
{
    return MatrixView<DataT, vt, Const>(m.data(), m.nRow(), m.nCol(), m.lDim());
}

template <typename DataT, MatrixViewport vt, Mutability mt>
class MatrixViewBase;

template <typename DataT, MatrixViewport vt>
class MatrixViewBase<DataT, vt, Const>
{
public:
    using DataType = DataT;
    static const MatrixViewport viewport = vt;
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

template <typename DataT, MatrixViewport vt>
class MatrixViewBase<DataT, vt, Mutable>
{
public:
    using DataType = DataT;
    static const MatrixViewport viewport = vt;
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

template <typename DataT, MatrixViewport vt, Mutability mt>
class MatrixView;

template <typename DataT, MatrixViewport vt>
class MatrixView<DataT, vt, Const> : public MatrixViewBase<DataT, vt, Const>
{
public:
    using MatrixViewBase<DataT, vt, Const>::MatrixViewBase;
    const DataT& operator()(int i, int j) const { return get(*this, i, j); }
};

template <typename DataT, MatrixViewport vt>
class MatrixView<DataT, vt, Mutable> : public MatrixViewBase<DataT, vt, Mutable>
{
public:
    using MatrixViewBase<DataT, vt, Mutable>::MatrixViewBase;
    operator MatrixView<DataT, vt, Const>() const { constView(*this); }
    DataT& operator()(int i, int j) const { return get(*this, i, j); }
};
}
