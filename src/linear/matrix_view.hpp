#pragma once
#include <cassert>
#include <type_traits>
namespace sanity::linear
{
enum MatrixViewType
{
    General,
    Diagonal,
    StrictUpper,
    Upper,
    StrictLower,
    Lower,
};

template <typename DataT, MatrixViewType vt>
class MatrixView;
template <typename DataT, MatrixViewType vt>
DataT get(MatrixView<DataT, vt> view, int i, int j)
{
    assert(i < view.nRow() && j < view.nCol());
    assert(i >= 0 && j >= 0);
    auto zero = DataT();
    switch (vt)
    {
        case General:
            break;
        case Upper:
            if (i > j) return zero;
            break;
        case StrictUpper:
            if (i >= j) return zero;
            break;
        case Lower:
            if (i < j) return zero;
            break;
        case StrictLower:
            if (i <= j) return zero;
            break;
        case Diagonal:
            if (i != j) return zero;
            break;
        default:
            assert(false);
            break;
    }
    return *(view.isRowMajor() ? view.data() + i * view.lDim() + j
                               : view.data() + i + j * view.lDim());
}

template <typename DataT, MatrixViewType vt>
class MatrixView
{
protected:
    DataT* _data;
    int _nrow;
    int _ncol;
    int _ldim;
    bool _is_row_major;

public:
    MatrixView(const DataT* data, int nrow, int ncol, int ldim,
               bool is_row_major)
        : _data(const_cast<DataT*>(data)),
          _nrow(nrow),
          _ncol(ncol),
          _ldim(ldim),
          _is_row_major(is_row_major)
    {
    }
    const DataT* data() const { return _data; }
    int nRow() const { return _nrow; }
    int nCol() const { return _ncol; }
    int lDim() const { return _ldim; }
    bool isRowMajor() const { return _is_row_major; }

    DataT operator()(int i, int j) const { return get(*this, i, j); }

    template <MatrixViewType target,
              std::enable_if_t<(vt == General && target != General) ||        //
                                   (vt == Diagonal && (target == Upper ||     //
                                                       target == Lower)) ||   //
                                   (vt == StrictUpper && target == Upper) ||  //
                                   (vt == StrictLower && target == Lower)     //
                               ,
                               int> = 0>
    operator MatrixView<DataT, target>() const
    {
        return MatrixView<DataT, target>{data(), nRow(), nCol(), lDim(),
                                         isRowMajor()};
    }

    template <MatrixViewType target,
              std::enable_if_t<(vt == General && target != General) ||        //
                                   (vt == Diagonal && (target == Upper ||     //
                                                       target == Lower)) ||   //
                                   (vt == StrictUpper && target == Upper) ||  //
                                   (vt == StrictLower && target == Lower)     //
                               ,
                               int> = 0>
    MatrixView<DataT, target> constView() const
    {
        return MatrixView<DataT, target>{data(), nRow(), nCol(), lDim(),
                                         isRowMajor()};
    }

    MatrixView<DataT, vt> blockView(int st_row, int st_col, int nrow = -1,
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
        return MatrixView<DataT, vt>(&get(*this, st_row, st_col), nrow, ncol,
                                     lDim(), isRowMajor());
    }
};

template <typename DataT, MatrixViewType vt>
class MatrixMutView;
template <typename DataT, MatrixViewType vt>
DataT& get(MatrixMutView<DataT, vt> view, int i, int j)
{
    assert(i < view.nRow() && j < view.nCol());
    assert(i >= 0 && j >= 0);
    switch (vt)
    {
        case General:
            break;
        case Upper:
            assert(i <= j);
            break;
        case StrictUpper:
            assert(i < j);
            break;
        case Lower:
            assert(i >= j);
            break;
        case StrictLower:
            assert(i > j);
            break;
        case Diagonal:
            assert(i == j);
            break;
        default:
            assert(false);
            break;
    }
    return *(view.isRowMajor() ? view.data() + i * view.lDim() + j
                               : view.data() + i + j * view.lDim());
}
template <typename DataT, MatrixViewType vt>
class MatrixMutView : public MatrixView<DataT, vt>
{
public:
    MatrixMutView(DataT* data, int nrow, int ncol, int ldim, bool is_row_major)
        : MatrixView<DataT, vt>(data, nrow, ncol, ldim, is_row_major)
    {
    }

    DataT* data() const { return MatrixView<DataT, vt>::_data; }
    DataT& operator()(int i, int j) const { return get(*this, i, j); }

    template <MatrixViewType target,
              std::enable_if_t<(vt == General && target != General) ||        //
                                   (vt == Diagonal && (target == Upper ||     //
                                                       target == Lower)) ||   //
                                   (vt == StrictUpper && target == Upper) ||  //
                                   (vt == StrictLower && target == Lower)     //
                               ,
                               int> = 0>
    operator MatrixMutView<DataT, target>() const
    {
        return MatrixMutView<DataT, target>{
            data(), MatrixView<DataT, vt>::nRow(),
            MatrixView<DataT, vt>::nCol(), MatrixView<DataT, vt>::lDim(),
            MatrixView<DataT, vt>::isRowMajor()};
    }

    template <MatrixViewType target,
              std::enable_if_t<(vt == General && target != General) ||        //
                                   (vt == Diagonal && (target == Upper ||     //
                                                       target == Lower)) ||   //
                                   (vt == StrictUpper && target == Upper) ||  //
                                   (vt == StrictLower && target == Lower)     //
                               ,
                               int> = 0>
    MatrixMutView<DataT, vt> mutView() const
    {
        return MatrixMutView<DataT, target>{
            data(), MatrixView<DataT, vt>::nRow(),
            MatrixView<DataT, vt>::nCol(), MatrixView<DataT, vt>::lDim(),
            MatrixView<DataT, vt>::isRowMajor()};
    }

    MatrixMutView<DataT, vt> blockMutView(int st_row, int st_col, int nrow = -1,
                                          int ncol = -1) const
    {
        if (nrow < 0)
        {
            nrow += MatrixView<DataT, vt>::nRow() - st_row + 1;
        }
        if (ncol < 0)
        {
            ncol += MatrixView<DataT, vt>::nCol() - st_col + 1;
        }
        return MatrixMutView<DataT, vt>(&get(*this, st_row, st_col), nrow, ncol,
                                        MatrixView<DataT, vt>::lDim(),
                                        MatrixView<DataT, vt>::isRowMajor());
    }
};
}
