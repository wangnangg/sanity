#pragma once
#include <algorithm>
#include <cassert>
#include <iomanip>
#include "matrix.hpp"
#include "matrix_view.hpp"
#include "vector.hpp"
#include "vector_view.hpp"

#define FOR_EACH_NONZERO(view_type, rowCount, colCount, row_idx, col_idx,   \
                         statement)                                         \
    ({                                                                      \
        switch (view_type)                                                  \
        {                                                                   \
            case General:                                                   \
            {                                                               \
                for (int row_idx = 0; row_idx < rowCount; row_idx++)        \
                {                                                           \
                    for (int col_idx = 0; col_idx < colCount; col_idx++)    \
                    {                                                       \
                        statement;                                          \
                    }                                                       \
                }                                                           \
                break;                                                      \
            }                                                               \
            case Diagonal:                                                  \
            {                                                               \
                int n = std::min(rowCount, colCount);                       \
                for (int row_idx = 0; row_idx < n; row_idx++)               \
                {                                                           \
                    int col_idx = row_idx;                                  \
                    {                                                       \
                        statement;                                          \
                    }                                                       \
                }                                                           \
                break;                                                      \
            }                                                               \
            case StrictUpper:                                               \
            {                                                               \
                int n = std::min(rowCount, colCount);                       \
                for (int row_idx = 0; row_idx < n; row_idx++)               \
                {                                                           \
                    for (int col_idx = row_idx + 1; col_idx < colCount;     \
                         col_idx++)                                         \
                    {                                                       \
                        statement;                                          \
                    }                                                       \
                }                                                           \
                break;                                                      \
            }                                                               \
            case Upper:                                                     \
            {                                                               \
                {                                                           \
                    int n = std::min(rowCount, colCount);                   \
                    for (int row_idx = 0; row_idx < n; row_idx++)           \
                    {                                                       \
                        for (int col_idx = row_idx; col_idx < colCount;     \
                             col_idx++)                                     \
                        {                                                   \
                            statement;                                      \
                        }                                                   \
                    }                                                       \
                    break;                                                  \
                }                                                           \
            }                                                               \
            case StrictLower:                                               \
            {                                                               \
                {                                                           \
                    int n = std::min(rowCount, colCount);                   \
                    for (int col_idx = 0; col_idx < n; col_idx++)           \
                    {                                                       \
                        for (int row_idx = col_idx + 1; row_idx < rowCount; \
                             row_idx++)                                     \
                        {                                                   \
                            statement;                                      \
                        }                                                   \
                    }                                                       \
                    break;                                                  \
                }                                                           \
            }                                                               \
            case Lower:                                                     \
            {                                                               \
                {                                                           \
                    int n = std::min(rowCount, colCount);                   \
                    for (int col_idx = 0; col_idx < n; col_idx++)           \
                    {                                                       \
                        for (int row_idx = col_idx; row_idx < rowCount;     \
                             row_idx++)                                     \
                        {                                                   \
                            statement;                                      \
                        }                                                   \
                    }                                                       \
                    break;                                                  \
                }                                                           \
            }                                                               \
            default:                                                        \
                assert(false);                                              \
        }                                                                   \
    })

namespace sanity::linear
{
Real maxDiff(VectorView<Real, Const> v1, VectorView<Real, Const> v2);
Real maxDiff(VectorView<Complex, Const> v1, VectorView<Complex, Const> v2);

template <typename DataT, MatrixViewport vt, Mutability mt>
Real maxDiff(MatrixView<DataT, vt, mt> m1, MatrixView<DataT, vt, mt> m2)
{
    Real max = 0;
    assert(m1.nRow() == m2.nRow());
    assert(m1.nCol() == m2.nCol());
    FOR_EACH_NONZERO(vt, m1.nRow(), m1.nCol(), i, j, ({
                         Real diff = std::abs(get(m1, i, j) - get(m2, i, j));
                         if (max < diff)
                         {
                             max = diff;
                         }
                     }));
    return max;
}

template <typename DataT, MatrixViewport vt, Mutability mt>
Real maxDiff(const Matrix<DataT>& m1, MatrixView<DataT, vt, mt> m2)
{
    Real max = 0;
    assert(m1.nRow() == m2.nRow());
    assert(m1.nCol() == m2.nCol());
    FOR_EACH_NONZERO(vt, m1.nRow(), m1.nCol(), i, j, ({
                         Real diff = std::abs(get(m1, i, j) - get(m2, i, j));
                         if (max < diff)
                         {
                             max = diff;
                         }
                     }));
    return max;
}

template <typename DataT, MatrixViewport vt, Mutability mt>
Real maxDiff(MatrixView<DataT, vt, mt> m1, const Matrix<DataT>& m2)
{
    Real max = 0;
    assert(m1.nRow() == m2.nRow());
    assert(m1.nCol() == m2.nCol());
    FOR_EACH_NONZERO(vt, m1.nRow(), m1.nCol(), i, j, ({
                         Real diff = std::abs(get(m1, i, j) - get(m2, i, j));
                         if (max < diff)
                         {
                             max = diff;
                         }
                     }));
    return max;
}

template <typename DataT, MatrixViewport vt, Mutability mt>
void copy(MatrixView<DataT, vt, mt> src, MatrixView<DataT, vt, Mutable> dst)
{
    assert(src.nRow() == dst.nRow());
    assert(src.nCol() == dst.nCol());
    FOR_EACH_NONZERO(vt, src.nRow(), src.nCol(), i, j,
                     (set(dst, i, j, get(src, i, j))));
}

template <typename DataT, MatrixViewport vt>
void copy(const Matrix<DataT>& src, MatrixView<DataT, vt, Mutable> dst)
{
    assert(src.nRow() == dst.nRow());
    assert(src.nCol() == dst.nCol());
    FOR_EACH_NONZERO(vt, src.nRow(), src.nCol(), i, j,
                     (set(dst, i, j, get(src, i, j))));
}

template <typename DataT, typename DataT2, MatrixViewport vt>
void setAll(MatrixView<DataT, vt, Mutable> mat, DataT2 val)
{
    FOR_EACH_NONZERO(vt, mat.nRow(), mat.nCol(), i, j,
                     (set(mat, i, j, DataT(val))));
}

template <typename DataT, MatrixViewport vt, Mutability mt>
std::ostream& operator<<(std::ostream& os, MatrixView<DataT, vt, mt> mat)
{
    for (int i = 0; i < mat.nRow(); i++)
    {
        for (int j = 0; j < mat.nCol(); j++)
        {
            if (has<vt>(i, j))
            {
                os << std::fixed << std::setw(8) << std::setprecision(3)
                   << std::setfill(' ') << get(mat, i, j);
            }
            else
            {
                os << std::fixed << std::setw(8) << std::setprecision(3)
                   << std::setfill(' ') << '-';
            }
        }
        os << std::endl;
    }
    return os;
}

template <typename DataT>
std::ostream& operator<<(std::ostream& os, const Matrix<DataT>& mat)
{
    for (int i = 0; i < mat.nRow(); i++)
    {
        for (int j = 0; j < mat.nCol(); j++)
        {
            os << std::fixed << std::setw(8) << std::setprecision(3)
               << std::setfill(' ') << get(mat, i, j);
        }
        os << std::endl;
    }
    return os;
}
}
