#include "matrix_view.hpp"
namespace sanity::linear
{
MatrixConstView blockView(MatrixConstView mat, uint srow, uint scol,
                          uint nrow, uint ncol)
{
    assert(srow + nrow <= mat.nrow());
    assert(scol + ncol <= mat.ncol());
    return MatrixConstView(&mat(srow, scol), nrow, ncol, mat.ldim());
}

MatrixMutableView blockView(MatrixMutableView mat, uint srow, uint scol,
                            uint nrow, uint ncol)
{
    assert(srow + (uint)nrow <= mat.nrow());
    assert(scol + (uint)ncol <= mat.ncol());
    return MatrixMutableView(&mat(srow, scol), nrow, ncol, mat.ldim());
}

CMatrixConstView blockView(CMatrixConstView mat, uint srow, uint scol,
                           uint nrow, uint ncol)
{
    assert(srow + (uint)nrow <= mat.nrow());
    assert(scol + (uint)ncol <= mat.ncol());
    return CMatrixConstView(&mat(srow, scol), nrow, ncol, mat.ldim());
}
CMatrixMutableView blockView(CMatrixMutableView mat, uint srow, uint scol,
                             uint nrow, uint ncol)
{
    assert(srow + (uint)nrow <= mat.nrow());
    assert(scol + (uint)ncol <= mat.ncol());
    return CMatrixMutableView(&mat(srow, scol), nrow, ncol, mat.ldim());
}

}  // namespace sanity::linear
