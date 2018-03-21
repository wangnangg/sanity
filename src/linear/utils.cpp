#include "utils.hpp"
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
