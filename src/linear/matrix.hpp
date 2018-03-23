#pragma once
#include <cassert>
#include <type.hpp>
#include <vector>
#include "matrix_view.hpp"

namespace sanity::linear
{
template <typename DataT>
class Matrix;

template <typename DataT>
bool has(const Matrix<DataT>& mat, int i, int j)
{
    return true;
}

template <typename DataT>
DataT* addressOf(Matrix<DataT>& mat, int i, int j)
{
    return (mat.data() + i * mat.nCol() + j);
}

template <typename DataT>
const DataT* addressOf(const Matrix<DataT>& mat, int i, int j)
{
    return (mat.data() + i * mat.nCol() + j);
}

template <typename DataT>
const DataT& get(const Matrix<DataT>& mat, int i, int j)
{
    return *addressOf(mat, i, j);
}

template <typename DataT>
DataT& get(Matrix<DataT>& mat, int i, int j)
{
    return *addressOf(mat, i, j);
}

template <typename DataT, typename DataT2>
void set(Matrix<DataT>& mat, int i, int j, DataT2 val)
{
    *addressOf(mat, i, j) = DataT(val);
}

template <typename DataT>
auto constView(const Matrix<DataT>& m)
{
    return MatrixView<DataT, General, Const>(m.data(), m.nRow(), m.nCol(),
                                             m.nCol());
}

template <typename DataT>
auto mutView(Matrix<DataT>& m)
{
    return MatrixView<DataT, General, Mutable>(m.data(), m.nRow(), m.nCol(),
                                               m.nCol());
}

template <typename DataT>
class Matrix
{
public:
    using DataType = DataT;
    static const MatrixViewport viewport = General;

private:
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

    const DataT* data() const { return &_data.front(); }
    DataT* data() { return &_data.front(); }
    int nRow() const { return _nrow; }
    int nCol() const { return _ncol; }
    DataT& operator()(int i, int j) { return get(*this, i, j); }
    const DataT& operator()(int i, int j) const { return get(*this, i, j); }

    operator MatrixView<DataT, General, Const>() const
    {
        return constView(*this);
    }
};
}
