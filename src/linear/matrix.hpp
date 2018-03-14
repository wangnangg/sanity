#pragma once
#include <cassert>
#include <type.hpp>
#include <vector>
#include "matrix_view.hpp"

namespace sanity::linear
{
template <typename DataT>
class Matrix
{
    std::vector<DataT> _data;
    int _nrow;
    int _ncol;
    bool _is_row_major;

public:
    Matrix() = default;
    Matrix(int nrow, int ncol, std::vector<DataT> data,
           bool is_row_major = false)
        : _data(std::move(data)),
          _nrow(nrow),
          _ncol(ncol),
          _is_row_major(is_row_major)
    {
        assert(_data.size() == static_cast<unsigned int>(nrow * ncol));
    }
    Matrix(int nrow, int ncol, DataT val = DataT(), bool is_row_major = false)
        : _data(static_cast<unsigned int>(nrow * ncol), val),
          _nrow(nrow),
          _ncol(ncol),
          _is_row_major(is_row_major)
    {
    }

    template <MatrixViewType vt>
    explicit Matrix(MatrixView<DataT, vt> v)
        : Matrix(v.nRow(), v.nCol(), DataT(), v.isRowMajor())
    {
        copyFrom(v, this->mutView());
    }

    const DataT* data() const { return &_data.front(); }
    DataT* data() { return &_data.front(); }
    int nRow() const { return _nrow; }
    int nCol() const { return _ncol; }
    bool isRowMajor() const { return _is_row_major; }

    int lDim() const { return isRowMajor() ? nCol() : nRow(); }

    const DataT& operator()(int i, int j) const { return get(*this, i, j); }

    DataT& operator()(int i, int j) { get(*this, i, j); }

    template <MatrixViewType vt = General>
    operator MatrixView<DataT, vt>() const
    {
        return MatrixView<DataT, vt>{data(), nRow(), nCol(), lDim(),
                                     isRowMajor()};
    }

    template <MatrixViewType vt = General>
    MatrixView<DataT, vt> constView() const
    {
        return MatrixView<DataT, vt>{data(), nRow(), nCol(), lDim(),
                                     isRowMajor()};
    }

    template <MatrixViewType vt = General>
    MatrixMutView<DataT, vt> mutView()
    {
        return MatrixMutView<DataT, vt>{data(), nRow(), nCol(), lDim(),
                                        isRowMajor()};
    }

    MatrixView<DataT, General> blockView(int st_row, int st_col, int nrow = -1,
                                         int ncol = -1) const
    {
        if (nrow < 0)
        {
            nrow += nRow() - st_row + 1;
        }
        if (ncol < 0)
        {
            ncol += nCol() - st_col + 1;
        }
        return MatrixView<DataT, General>(&get(*this, st_row, st_col), nrow,
                                          ncol, lDim(), isRowMajor());
    }
    MatrixMutView<DataT, General> blockMutView(int st_row, int st_col,
                                               int nrow = -1, int ncol = -1)
    {
        if (nrow < 0)
        {
            nrow += nRow() - st_row + 1;
        }
        if (ncol < 0)
        {
            ncol += nCol() - st_col + 1;
        }
        return MatrixMutView<DataT, General>(&get(*this, st_row, st_col), nrow,
                                             ncol, lDim(), isRowMajor());
    }
};
template <typename DataT>
const DataT& get(const Matrix<DataT>& m, int i, int j)
{
    assert(i < m.nRow() && j < m.nCol());
    assert(i >= 0 && j >= 0);
    return *(m.isRowMajor() ? m.data() + i * m.nRow() + j
                            : m.data() + i + j * m.nCol());
}

template <typename DataT>
DataT& get(Matrix<DataT>& m, int i, int j)
{
    assert(i < m.nRow() && j < m.nCol());
    assert(i >= 0 && j >= 0);
    return m.isRowMajor() ? m.data() + i * m.nRow() + j
                          : m.data() + i + j * m.nCol();
}
}
