#pragma once
#include <algorithm>
#include <cassert>
#include <iomanip>
#include "matrix.hpp"
#include "matrix_view.hpp"

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
template <typename DataT, MatrixViewType vt>
void copyFrom(MatrixView<DataT, vt> src, MatrixMutView<DataT, vt> dst)
{
    assert(src.nRow() == dst.nRow());
    assert(src.nCol() == dst.nCol());
    FOR_EACH_NONZERO(src, vt, i, j, (dst(i, j) = src(i, j)));
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

template <typename DataT, MatrixViewType vt>
std::ostream& operator<<(std::ostream& os, MatrixView<DataT, vt> mat)
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

template <typename DataT, MatrixViewType vt>
std::ostream& operator<<(std::ostream& os, MatrixMutView<DataT, vt> mat)
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
}
