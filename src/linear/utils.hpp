#pragma once
#include <algorithm>
#include <cassert>
#include <iomanip>
#include "matrix.hpp"
#include "matrix_view.hpp"
#include "type_check.hpp"
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
template <typename DataT, Conjugation ct1, Conjugation ct2>
Real maxDiff(VectorView<DataT, ct1, Const> v1, VectorView<DataT, ct2, Const> v2)
{
    assert(v1.size() == v2.size());
    int n = v1.size();
    Real max = 0;
    for (int i = 0; i < n; i++)
    {
        Real v = std::abs(get(v1, i) - ge(v2, i));
        if (max < v)
        {
            max = v;
        }
    }
    return max;
}

template <typename DataT, MatrixViewport vt, MatrixStorage st1,
          MatrixStorage st2, Conjugation ct1, Conjugation ct2>
Real maxDiff(MatrixView<DataT, vt, st1, ct1, Const> mat1,
             MatrixView<DataT, vt, st2, ct2, Const> mat2)
{
    assert(mat1.nRow() == mat2.nRow());
    assert(mat1.nCol() == mat2.nCol());
    Real max = 0;
    FOR_EACH_NONZERO(vt, mat1.nRow(), mat1.nCol(), i, j, ({
                         Real v = std::abs(get(mat1, i, j) - get(mat2, i, j));
                         if (max < v) max = v;
                     }));
    return max;
}

template <typename DataT, MatrixViewport vt, MatrixStorage st1,
          MatrixStorage st2, Conjugation ct1, Conjugation ct2>
void copy(MatrixView<DataT, vt, st1, ct1, Const> src,
          MatrixView<DataT, vt, st1, ct1, Mutable> dst)
{
    assert(src.nRow() == dst.nRow());
    assert(src.nCol() == dst.nCol());
    FOR_EACH_NONZERO(vt, src.nRow(), src.nCol(), i, j,
                     (set(dst, i, j, get(src, i, j))));
}

template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct>
void setAll(DataT val, MatrixView<DataT, vt, st, ct, Mutable> mat);

template <typename DataT, MatrixViewport vt, MatrixStorage st>
void setAll(DataT val, MatrixView<DataT, vt, st, NoConj, Mutable> mat)
{
    FOR_EACH_NONZERO(vt, mat.nRow(), mat.nCol(), i, j, (set(mat, i, j, val)));
}

template <typename DataT, MatrixViewport vt, MatrixStorage st>
void setAll(DataT val, MatrixView<DataT, vt, st, Conj, Mutable> mat)
{
    auto cval = std::conj(val);
    auto cmat = conjugate(mat);
    setAll(cmat, cval);
}

template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct,
          Mutability mt>
DataT getWithDefault(MatrixView<DataT, vt, st, ct, mt> m, int i, int j,
                     DataT def = DataT())
{
    return has(m, i, j) ? get(m, i, j) : def;
}

template <typename DataT, MatrixViewport vt, MatrixStorage st, Conjugation ct,
          Mutability mt>
std::ostream& operator<<(std::ostream& os,
                         MatrixView<DataT, vt, st, ct, mt> mat)
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
