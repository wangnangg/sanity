#include "matrix_view.hpp"
namespace sanity::linear
{
MatrixConstView blockView(MatrixConstView mat, int srow, int scol, int nrow,
                          int ncol)
{
    if (nrow < 0)
    {
        nrow += mat.nrow() - srow + 1;
    }
    if (ncol < 0)
    {
        ncol += mat.ncol() - scol + 1;
    }
    assert(nrow >= 0 && ncol >= 0);
    assert(srow + nrow <= mat.nrow());
    assert(scol + ncol <= mat.ncol());
    return MatrixConstView(&mat(srow, scol), nrow, ncol, mat.ldim());
}

MatrixMutableView blockView(MatrixMutableView mat, int srow, int scol, int nrow,
                            int ncol)
{
    if (nrow < 0)
    {
        nrow += mat.nrow() - srow + 1;
    }
    if (ncol < 0)
    {
        ncol += mat.ncol() - scol + 1;
    }
    assert(nrow >= 0 && ncol >= 0);
    assert(srow + nrow <= mat.nrow());
    assert(scol + ncol <= mat.ncol());
    return MatrixMutableView(&mat(srow, scol), nrow, ncol, mat.ldim());
}
}
