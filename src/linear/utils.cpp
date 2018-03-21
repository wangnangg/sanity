#include "utils.hpp"
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
template <typename DataT, MatrixViewport vt>
void copyImpl(MatrixView<DataT, vt> src, MatrixMutView<DataT, vt> dst)
{
    assert(src.nRow() == dst.nRow());
    assert(src.nCol() == dst.nCol());
    FOR_EACH_NONZERO(src, vt, i, j, (dst(i, j) = src(i, j)));
}
void copy(MatrixView<Real, General> src, MatrixMutView<Real, General> dst)
{
    copyImpl(src, dst);
}
void copy(MatrixView<Real, Upper> src, MatrixMutView<Real, Upper> dst)
{
    copyImpl(src, dst);
}
void copy(MatrixView<Real, Lower> src, MatrixMutView<Real, Lower> dst)
{
    copyImpl(src, dst);
}
void copy(MatrixView<Real, StrictUpper> src,
          MatrixMutView<Real, StrictUpper> dst)
{
    copyImpl(src, dst);
}
void copy(MatrixView<Real, StrictLower> src,
          MatrixMutView<Real, StrictLower> dst)
{
    copyImpl(src, dst);
}
void copy(MatrixView<Real, Diagonal> src, MatrixMutView<Real, Diagonal> dst)
{
    copyImpl(src, dst);
}
void copy(MatrixView<Complex, General> src, MatrixMutView<Complex, General> dst)
{
    copyImpl(src, dst);
}
void copy(MatrixView<Complex, Upper> src, MatrixMutView<Complex, Upper> dst)
{
    copyImpl(src, dst);
}
void copy(MatrixView<Complex, Lower> src, MatrixMutView<Complex, Lower> dst)
{
    copyImpl(src, dst);
}
void copy(MatrixView<Complex, StrictUpper> src,
          MatrixMutView<Complex, StrictUpper> dst)
{
    copyImpl(src, dst);
}
void copy(MatrixView<Complex, StrictLower> src,
          MatrixMutView<Complex, StrictLower> dst)
{
    copyImpl(src, dst);
}
void copy(MatrixView<Complex, Diagonal> src,
          MatrixMutView<Complex, Diagonal> dst)
{
    copyImpl(src, dst);
}
}
