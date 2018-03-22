#pragma once
#include <cassert>
#include <type.hpp>
#include <vector>
#include "matrix_view.hpp"

namespace sanity::linear
{

template <typename DataT, MatrixStorage st>
class Matrix
{
public:
    using DataType = DataT;
    static const MatrixViewport viewport = General;
    static const MatrixStorage storage = st;
    static const Conjugation conjuation = NoConj;

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

    template <MatrixViewport vt, MatrixStorage vst, Conjugation ct,
              Mutability mt>
    explicit Matrix(MatrixView<DataT, vt, vst, ct, mt> v)
        : Matrix(v.nRow(), v.nCol(), DataT())
    {
        copy(v, viewportCast<vt>(mutView(*this)));
    }

    const DataT* data() const { return &_data.front(); }
    DataT* data() { return &_data.front(); }
    int nRow() const { return _nrow; }
    int nCol() const { return _ncol; }
};

template <typename DataT, MatrixStorage st>
MatrixView<DataT, General, st, NoConj, Const> constView(
    const Matrix<DataT, st>& m)
{
    return MatrixView<DataT, General, st, NoConj, Const>(
        m.data(), m.nRow(), m.nCol(), m.nCol());
}

template <typename DataT, MatrixStorage st>
MatrixView<DataT, General, st, NoConj, Mutable> mutView(Matrix<DataT, st>& m)
{
    return MatrixView<DataT, General, st, NoConj, Mutable>(
        m.data(), m.nRow(), m.nCol(), m.nCol());
}
}
