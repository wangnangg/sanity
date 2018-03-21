#pragma once
#include <cassert>
#include <type.hpp>
#include <vector>
#include "matrix_view.hpp"

namespace sanity::linear
{
template <typename DataT = Real>
class Matrix;

template <typename DataT>
DataT get(const Matrix<DataT>& m, int i, int j)
{
    assert(i < m.nRow() && j < m.nCol());
    assert(i >= 0 && j >= 0);
    return *(m.data() + i * m.nRow() + j);
}

template <typename DataT>
DataT& get(Matrix<DataT>& m, int i, int j)
{
    assert(i < m.nRow() && j < m.nCol());
    assert(i >= 0 && j >= 0);
    return *(m.data() + i * m.nRow() + j);
}

template <MatrixViewport vt = General, typename DataT>
MatrixView<DataT, vt> constView(const Matrix<DataT>& m)
{
    return MatrixView<DataT, vt>(m.data(), m.nRow(), m.nCol(), m.nCol(), false,
                                 false);
}

template <MatrixViewport vt = General, typename DataT>
MatrixMutView<DataT, vt> mutView(Matrix<DataT>& m)
{
    return MatrixMutView<DataT, vt>(m.data(), m.nRow(), m.nCol(), m.nCol());
}

template <MatrixViewport target, typename DataT>
MatrixMutView<DataT, target> viewport(Matrix<DataT>& m)
{
    return MatrixMutView<DataT, target>(m.data(), m.nRow(), m.nCol(), m.nCol());
}

template <MatrixViewport target, typename DataT>
MatrixView<DataT, target> viewport(const Matrix<DataT>& m)
{
    return MatrixView<DataT, target>(m.data(), m.nRow(), m.nCol(), m.nCol(),
                                     false, false);
}

template <typename DataT>
MatrixView<DataT, General> blockView(const Matrix<DataT>& mat, int st_row,
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
    const DataT* start_p = (mat.data() + st_row * mat.nCol() + st_col);
    return MatrixView<DataT, General>(start_p, ed_row - st_row, ed_col - st_col,
                                      mat.nCol(), false, false);
}
template <typename DataT>
class Matrix
{
    std::vector<DataT> _data;
    int _nrow;
    int _ncol;

public:
    Matrix() = default;
    Matrix(int nrow, int ncol, std::vector<DataT> data)
        : _data(std::move(data)), _nrow(nrow), _ncol(ncol)
    {
        assert(_data.size() == static_cast<unsigned int>(nrow * ncol));
    }
    Matrix(int nrow, int ncol, DataT val = DataT())
        : _data(static_cast<unsigned int>(nrow * ncol), val),
          _nrow(nrow),
          _ncol(ncol)
    {
    }

    template <MatrixViewport vt>
    explicit Matrix(MatrixView<DataT, vt> v)
        : Matrix(v.nRow(), v.nCol(), DataT())
    {
        copy(v, viewport<vt>(this->mut()));
    }

    const DataT* data() const { return &_data.front(); }
    DataT* data() { return &_data.front(); }
    int nRow() const { return _nrow; }
    int nCol() const { return _ncol; }

    DataT operator()(int i, int j) const { return get(*this, i, j); }

    DataT& operator()(int i, int j) { get(*this, i, j); }

    template <MatrixViewport vt = General>
    operator MatrixView<DataT, vt>() const
    {
        return constView<vt, DataT>(*this);
    }

    MatrixMutView<DataT, General> mut()
    {
        return sanity::linear::mutView<General, DataT>(*this);
    }
};
}
