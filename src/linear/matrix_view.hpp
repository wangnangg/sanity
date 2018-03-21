#pragma once
#include "matrix_view_base.hpp"
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

template <typename DataT, MatrixViewport vt>
class MatrixView;

template <typename DataT, MatrixViewport vt>
class MatrixMutView;

template <typename DataT, MatrixViewport vt>
DataT& get(MatrixMutView<DataT, vt> mat, int i, int j)
{
    assert(mat.has(i, j));
    return *(mat.data() + i * mat.lDim() + j);
}

template <typename DataT, MatrixViewport vt>
DataT get(MatrixView<DataT, vt> mat, int i, int j);

template <MatrixViewport vt>
Real get(MatrixView<Real, vt> mat, int i, int j)
{
    assert(mat.has(i, j));
    if (mat.colMajor())
    {
        return *(mat.data() + i + j * mat.lDim());
    }
    else
    {
        return *(mat.data() + i * mat.lDim() + j);
    }
}

template <MatrixViewport vt>
Complex get(MatrixView<Complex, vt> mat, int i, int j)
{
    assert(mat.has(i, j));
    if (mat.colMajor())
    {
        if (mat.conjugated())
        {
            return std::conj(*(mat.data() + i + j * mat.lDim()));
        }
        else
        {
            return *(mat.data() + i + j * mat.lDim());
        }
    }
    else
    {
        if (mat.conjugated())
        {
            return std::conj(*(mat.data() + i * mat.lDim() + j));
        }
        else
        {
            return *(mat.data() + i * mat.lDim() + j);
        }
    }
}

template <typename DataT>
MatrixView<DataT, General> blockView(MatrixView<DataT, General> mat, int st_row,
                                     int st_col, int ed_row = -1,
                                     int ed_col = -1)
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
    const DataT* start_p = mat.transposed()
                               ? (mat.data() + st_row + st_col * mat.lDim())
                               : (mat.data() + st_row * mat.lDim() + st_col);
    return MatrixView<DataT, General>(start_p, ed_row - st_row, ed_col - st_col,
                                      mat.lDim(), mat.tranposed(),
                                      mat.conjugated());
}

template <typename DataT>
MatrixMutView<DataT, General> blockView(MatrixMutView<DataT, General> mat,
                                        int st_row, int st_col, int ed_row = -1,
                                        int ed_col = -1)
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
    const DataT* start_p = mat.data() + st_row * mat.lDim() + st_col;
    return MatrixMutView<DataT, General>(start_p, ed_row - st_row,
                                         ed_col - st_col, mat.lDim(),
                                         mat.tranposed(), mat.conjugated());
}

template <
    MatrixViewport target, MatrixViewport src, typename DataT,
    std::enable_if_t<
        (src == General) ||
            (src == Upper && (target == StrictUpper || target == Diagonal)) ||
            (src == Lower && (target == StrictLower || target == Diagonal)),
        int> = 0>
MatrixMutView<DataT, target> viewport(MatrixMutView<DataT, src> m)
{
    return MatrixMutView<DataT, target>(m.data(), m.nRow(), m.nCol(), m.lDim());
}

template <
    MatrixViewport target, MatrixViewport src, typename DataT,
    std::enable_if_t<
        (src == General) ||
            (src == Upper && (target == StrictUpper || target == Diagonal)) ||
            (src == Lower && (target == StrictLower || target == Diagonal)),
        int> = 0>
MatrixView<DataT, target> viewport(MatrixView<DataT, src> m)
{
    return MatrixView<DataT, target>(m.data(), m.nRow(), m.nCol(), m.lDim(),
                                     false, false);
}

template <typename DataT, MatrixViewport vt>
MatrixView<DataT, vt> constView(MatrixMutView<DataT, vt> m)
{
    return MatrixView<DataT, vt>(m.data(), m.nRow(), m.nCol(), m.lDim(), false,
                                 false);
}

template <typename DataT>
class MatrixView<DataT, General> : public MatrixViewBase<DataT>
{
public:
    using MatrixViewBase<DataT>::MatrixViewBase;
    bool has(int, int) const { return true; }
    DataT operator()(int i, int j) { return get(*this, i, j); }
};

template <typename DataT>
class MatrixMutView<DataT, General> : public MatrixMutViewBase<DataT>
{
public:
    using MatrixMutViewBase<DataT>::MatrixMutViewBase;
    bool has(int, int) const { return true; }
    DataT& operator()(int i, int j) { return get(*this, i, j); }

    operator MatrixView<DataT, General>() const
    {
        return sanity::linear::constView(*this);
    }
};

template <typename DataT>
class MatrixView<DataT, Upper> : public MatrixViewBase<DataT>
{
public:
    using MatrixViewBase<DataT>::MatrixViewBase;
    bool has(int i, int j) const
    {
        if (i <= j)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    DataT operator()(int i, int j) { return get(*this, i, j); }
};

template <typename DataT>
class MatrixMutView<DataT, Upper> : public MatrixMutViewBase<DataT>
{
public:
    using MatrixMutViewBase<DataT>::MatrixMutViewBase;
    bool has(int i, int j) const
    {
        if (i <= j)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    DataT& operator()(int i, int j) { return get(*this, i, j); }

    operator MatrixView<DataT, Upper>() const
    {
        return sanity::linear::constView(*this);
    }
};

template <typename DataT>
class MatrixView<DataT, StrictUpper> : public MatrixViewBase<DataT>
{
public:
    using MatrixViewBase<DataT>::MatrixViewBase;
    bool has(int i, int j) const
    {
        if (i < j)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    DataT operator()(int i, int j) { return get(*this, i, j); }
};

template <typename DataT>
class MatrixMutView<DataT, StrictUpper> : public MatrixMutViewBase<DataT>
{
public:
    using MatrixMutViewBase<DataT>::MatrixMutViewBase;
    bool has(int i, int j) const
    {
        if (i < j)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    DataT& operator()(int i, int j) { return get(*this, i, j); }
    operator MatrixView<DataT, StrictUpper>() const
    {
        return sanity::linear::constView(*this);
    }
};

template <typename DataT>
class MatrixView<DataT, Lower> : public MatrixViewBase<DataT>
{
public:
    using MatrixViewBase<DataT>::MatrixViewBase;
    bool has(int i, int j) const
    {
        if (i >= j)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    DataT operator()(int i, int j) { return get(*this, i, j); }
};

template <typename DataT>
class MatrixMutView<DataT, Lower> : public MatrixMutViewBase<DataT>
{
public:
    using MatrixMutViewBase<DataT>::MatrixMutViewBase;
    bool has(int i, int j) const
    {
        if (i >= j)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    DataT& operator()(int i, int j) { return get(*this, i, j); }
    operator MatrixView<DataT, Lower>() const
    {
        return sanity::linear::constView(*this);
    }
};

template <typename DataT>
class MatrixView<DataT, StrictLower> : public MatrixViewBase<DataT>
{
public:
    using MatrixViewBase<DataT>::MatrixViewBase;
    bool has(int i, int j) const
    {
        if (i > j)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    DataT operator()(int i, int j) const { return get(*this, i, j); }
};

template <typename DataT>
class MatrixMutView<DataT, StrictLower> : public MatrixMutViewBase<DataT>
{
public:
    using MatrixMutViewBase<DataT>::MatrixMutViewBase;
    bool has(int i, int j) const
    {
        if (i > j)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    DataT& operator()(int i, int j) { return get(*this, i, j); }

    operator MatrixView<DataT, StrictLower>() const
    {
        return sanity::linear::constView(*this);
    }
};

template <typename DataT>
class MatrixView<DataT, Diagonal> : public MatrixViewBase<DataT>
{
public:
    using MatrixViewBase<DataT>::MatrixViewBase;
    bool has(int i, int j) const
    {
        if (i == j)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    DataT operator()(int i, int j) { return get(*this, i, j); }
};

template <typename DataT>
class MatrixMutView<DataT, Diagonal> : public MatrixMutViewBase<DataT>
{
public:
    using MatrixMutViewBase<DataT>::MatrixMutViewBase;
    bool has(int i, int j) const
    {
        if (i == j)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    DataT& operator()(int i, int j) { return get(*this, i, j); }
    operator MatrixView<DataT, Diagonal>() const
    {
        return sanity::linear::constView(*this);
    }
};
}
