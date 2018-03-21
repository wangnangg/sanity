#pragma once
#include <algorithm>
#include <cassert>
#include <iomanip>
#include "matrix.hpp"
#include "matrix_view.hpp"
#include "type_check.hpp"
#include "vector.hpp"
#include "vector_view.hpp"

#define FOR_EACH_NONZERO(view, view_type, row_idx, col_idx, statement)         \
    ({                                                                         \
        switch (view_type)                                                     \
        {                                                                      \
            case General:                                                      \
            {                                                                  \
                for (int row_idx = 0; row_idx < view.nRow(); row_idx++)        \
                {                                                              \
                    for (int col_idx = 0; col_idx < view.nCol(); col_idx++)    \
                    {                                                          \
                        statement;                                             \
                    }                                                          \
                }                                                              \
                break;                                                         \
            }                                                                  \
            case Diagonal:                                                     \
            {                                                                  \
                int n = std::min(view.nRow(), view.nCol());                    \
                for (int row_idx = 0; row_idx < n; row_idx++)                  \
                {                                                              \
                    int col_idx = row_idx;                                     \
                    {                                                          \
                        statement;                                             \
                    }                                                          \
                }                                                              \
                break;                                                         \
            }                                                                  \
            case StrictUpper:                                                  \
            {                                                                  \
                int n = std::min(view.nRow(), view.nCol());                    \
                for (int row_idx = 0; row_idx < n; row_idx++)                  \
                {                                                              \
                    for (int col_idx = row_idx + 1; col_idx < view.nCol();     \
                         col_idx++)                                            \
                    {                                                          \
                        statement;                                             \
                    }                                                          \
                }                                                              \
                break;                                                         \
            }                                                                  \
            case Upper:                                                        \
            {                                                                  \
                {                                                              \
                    int n = std::min(view.nRow(), view.nCol());                \
                    for (int row_idx = 0; row_idx < n; row_idx++)              \
                    {                                                          \
                        for (int col_idx = row_idx; col_idx < view.nCol();     \
                             col_idx++)                                        \
                        {                                                      \
                            statement;                                         \
                        }                                                      \
                    }                                                          \
                    break;                                                     \
                }                                                              \
            }                                                                  \
            case StrictLower:                                                  \
            {                                                                  \
                {                                                              \
                    int n = std::min(view.nRow(), view.nCol());                \
                    for (int col_idx = 0; col_idx < n; col_idx++)              \
                    {                                                          \
                        for (int row_idx = col_idx + 1; row_idx < view.nRow(); \
                             row_idx++)                                        \
                        {                                                      \
                            statement;                                         \
                        }                                                      \
                    }                                                          \
                    break;                                                     \
                }                                                              \
            }                                                                  \
            case Lower:                                                        \
            {                                                                  \
                {                                                              \
                    int n = std::min(view.nRow(), view.nCol());                \
                    for (int col_idx = 0; col_idx < n; col_idx++)              \
                    {                                                          \
                        for (int row_idx = col_idx; row_idx < view.nRow();     \
                             row_idx++)                                        \
                        {                                                      \
                            statement;                                         \
                        }                                                      \
                    }                                                          \
                    break;                                                     \
                }                                                              \
            }                                                                  \
            default:                                                           \
                assert(false);                                                 \
        }                                                                      \
    })

namespace sanity::linear
{
template <typename V1, typename V2,
          std::enable_if_t<isVectorType<V1> && isVectorType<V2>, int> = 0>
Real maxDiff(const V1& v1, const V2& v2)
{
    assert(v1.size() == v2.size());
    int n = v1.size();
    Real max = 0;
    for (int i = 0; i < n; i++)
    {
        Real v = std::abs(v1(i) - v2(i));
        if (max < v)
        {
            max = v;
        }
    }
    return max;
}

template <typename M1, typename M2,
          std::enable_if_t<isMatrixType<M1>         //
                               && isMatrixType<M2>  //
                               && M1::Viewport == M2::Viewport,
                           int> = 0>
Real maxDiff(const M1& mat1, const M2& mat2)
{
    assert(mat1.nRow() == mat2.nRow());
    assert(mat1.nCol() == mat2.nCol());
    Real max = 0;
    FOR_EACH_NONZERO(mat1, M1::Viewport, i, j, ({
                         Real v = std::abs(mat1(i, j) - mat2(i, j));
                         if (max < v) max = v;
                     }));
    return max;
}

template <typename M1, typename M2,
          std::enable_if_t<isMatrixType<M1> && isMatrixType<M2> &&
                               M1::Viewport == M2::Viewport && isMutable<M2>,
                           int> = 0>
void copy(const M1& src, const M2& dst)
{
    assert(src.nRow() == dst.nRow());
    assert(src.nCol() == dst.nCol());
    FOR_EACH_NONZERO(src, M1::Viewport, i, j, (dst(i, j) = src(i, j)));
}

template <MatrixViewport vt>
void setValue(Real val, const MatrixMutView<Real, vt> mat)
{
    FOR_EACH_NONZERO(mat, vt, i, j, (mat(i, j) = val));
}

template <MatrixViewport vt>
void setValue(Complex val, const MatrixMutView<Complex, vt> mat)
{
    FOR_EACH_NONZERO(mat, vt, i, j, (mat(i, j) = val));
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

template <typename DataT, MatrixViewport vt>
MatrixView<DataT, transViewport(vt)> transpose(MatrixView<DataT, vt> mat)
{
    return MatrixView<DataT, transViewport(vt)>(
        mat.data(), mat.nCol(), mat.nRow(), mat.lDim(), !mat.colMajor(),
        mat.conjugated());
}

template <typename DataT>
MatrixView<DataT, General> transpose(const Matrix<DataT>& mat)
{
    return MatrixView<DataT, General>(mat.data(), mat.nCol(), mat.nRow(),
                                      mat.nCol(), true, false);
}

template <typename DataT, MatrixViewport vt>
DataT getWithDefault(MatrixView<DataT, vt> m, int i, int j, DataT def = DataT())
{
    return m.has(i, j) ? m(i, j) : def;
}

template <typename DataT, MatrixViewport vt>
DataT getWithDefault(MatrixMutView<DataT, vt> m, int i, int j,
                     DataT def = DataT())
{
    return m.has(i, j) ? m(i, j) : def;
}

template <typename DataT>
std::ostream& operator<<(std::ostream& os, const Matrix<DataT>& mat)
{
    for (int i = 0; i < mat.nRow(); i++)
    {
        for (int j = 0; j < mat.nCol(); j++)
        {
            os << std::fixed << std::setw(8) << std::setprecision(3)
               << std::setfill(' ') << mat(i, j);
        }
        os << std::endl;
    }
    return os;
}

template <typename DataT, MatrixViewport vt>
std::ostream& operator<<(std::ostream& os, MatrixView<DataT, vt> mat)
{
    for (int i = 0; i < mat.nRow(); i++)
    {
        for (int j = 0; j < mat.nCol(); j++)
        {
            os << std::fixed << std::setw(8) << std::setprecision(3)
               << std::setfill(' ') << getWithDefault(mat, i, j);
        }
        os << std::endl;
    }
    return os;
}

template <typename DataT, MatrixViewport vt>
std::ostream& operator<<(std::ostream& os, MatrixMutView<DataT, vt> mat)
{
    for (int i = 0; i < mat.nRow(); i++)
    {
        for (int j = 0; j < mat.nCol(); j++)
        {
            os << std::fixed << std::setw(8) << std::setprecision(3)
               << std::setfill(' ') << getWithDefault(mat, i, j);
        }
        os << std::endl;
    }
    return os;
}
}
