#include "blas.hpp"
#include "blas/cblas.h"
namespace sanity::linear::blas
{
Real dot(VectorView<Real, NoConj, Const> x, VectorView<Real, NoConj, Const> y)
{
    assert(x.size() == y.size());
    return cblas_ddot(x.size(), x.data(), x.inc(), y.data(), y.inc());
}

Complex dot(VectorView<Complex, NoConj, Const> x,
            VectorView<Complex, NoConj, Const> y)
{
    assert(x.size() == y.size());
    Complex res;
    cblas_zdotu_sub(x.size(), x.data(), x.inc(), y.data(), y.inc(), &res);
    return res;
}
Complex dot(VectorView<Complex, Conj, Const> x,
            VectorView<Complex, NoConj, Const> y)
{
    assert(x.size() == y.size());
    Complex res;
    cblas_zdotc_sub(x.size(), x.data(), x.inc(), y.data(), y.inc(), &res);
    return res;
}

Real nrm2(VectorView<Real, NoConj, Const> x)
{
    return cblas_dnrm2(x.size(), x.data(), x.inc());
}
Real nrm2(VectorView<Complex, NoConj, Const> x)
{
    return cblas_dznrm2(x.size(), x.data(), x.inc());
}
Real nrm2(VectorView<Complex, Conj, Const> x)
{
    return cblas_dznrm2(x.size(), x.data(), x.inc());
}

Real asum(VectorView<Real, NoConj, Const> x)
{
    return cblas_dasum(x.size(), x.data(), x.inc());
}
Real asum(VectorView<Complex, NoConj, Const> x)
{
    return cblas_dzasum(x.size(), x.data(), x.inc());
}
Real asum(VectorView<Complex, Conj, Const> x)
{
    return cblas_dzasum(x.size(), x.data(), x.inc());
}

int iamax(VectorView<Real, NoConj, Const> x)
{
    return cblas_idamax(x.size(), x.data(), x.inc());
}
int iamax(VectorView<Complex, NoConj, Const> x)
{
    return cblas_izamax(x.size(), x.data(), x.inc());
}
int iamax(VectorView<Complex, Conj, Const> x)
{
    return cblas_izamax(x.size(), x.data(), x.inc());
}

void swap(VectorView<Real, NoConj, Mutable> x,
          VectorView<Real, NoConj, Mutable> y)
{
    assert(x.size() == y.size());
    cblas_dswap(x.size(), x.data(), x.inc(), y.data(), y.inc());
}
void swap(VectorView<Complex, NoConj, Mutable> x,
          VectorView<Complex, NoConj, Mutable> y)
{
    assert(x.size() == y.size());
    cblas_zswap(x.size(), x.data(), x.inc(), y.data(), y.inc());
}
void swap(VectorView<Complex, Conj, Mutable> x,
          VectorView<Complex, Conj, Mutable> y)
{
    assert(x.size() == y.size());
    cblas_zswap(x.size(), x.data(), x.inc(), y.data(), y.inc());
}
void copy(VectorView<Real, NoConj, Const> x,
          VectorView<Real, NoConj, Mutable> y)
{
    assert(x.size() == y.size());
    cblas_dcopy(x.size(), x.data(), x.inc(), y.data(), y.inc());
}
void copy(VectorView<Complex, NoConj, Const> x,
          VectorView<Complex, NoConj, Mutable> y)
{
    assert(x.size() == y.size());
    cblas_zcopy(x.size(), x.data(), x.inc(), y.data(), y.inc());
}
void copy(VectorView<Complex, Conj, Const> x,
          VectorView<Complex, Conj, Mutable> y)
{
    assert(x.size() == y.size());
    cblas_zcopy(x.size(), x.data(), x.inc(), y.data(), y.inc());
}

void axpy(Real a, VectorView<Real, NoConj, Const> x,
          VectorView<Real, NoConj, Mutable> y)
{
    assert(x.size() == y.size());
    cblas_daxpy(x.size(), a, x.data(), x.inc(), y.data(), y.inc());
}
void axpy(Complex a, VectorView<Complex, NoConj, Const> x,
          VectorView<Complex, NoConj, Mutable> y)
{
    assert(x.size() == y.size());
    cblas_zaxpy(x.size(), &a, x.data(), x.inc(), y.data(), y.inc());
}

void scal(Real a, VectorView<Real, NoConj, Mutable> x)
{
    cblas_dscal(x.size(), a, x.data(), x.inc());
}

void scal(Real a, VectorView<Complex, NoConj, Mutable> x)
{
    cblas_zdscal(x.size(), a, x.data(), x.inc());
}
void scal(Real a, VectorView<Complex, Conj, Mutable> x)
{
    cblas_zdscal(x.size(), a, x.data(), x.inc());
}
void scal(Complex a, VectorView<Complex, NoConj, Mutable> x)
{
    cblas_zscal(x.size(), &a, x.data(), x.inc());
}

template <typename M>
static constexpr enum CBLAS_ORDER order(const M& A)
{
    if (M::storage == RowMajor)
    {
        return CblasRowMajor;
    }
    else
    {
        return CblasColMajor;
    }
}

void gemv(Real a, MatrixView<Real, General, RowMajor, NoConj, Const> A,
          VectorView<Real, NoConj, Const> x, Real b,
          VectorView<Real, NoConj, Mutable> y)
{
    assert(A.nCol() == x.size());
    assert(A.nRow() == y.size());
    cblas_dgemv(order(A), CblasNoTrans, A.nRow(), A.nCol(), a, A.data(),
                A.lDim(), x.data(), x.inc(), b, y.data(), y.inc());
}

void gemv(Real a, MatrixView<Real, General, ColMajor, NoConj, Const> A,
          VectorView<Real, NoConj, Const> x, Real b,
          VectorView<Real, NoConj, Mutable> y)
{
    assert(A.nCol() == x.size());
    assert(A.nRow() == y.size());
    cblas_dgemv(order(A), CblasNoTrans, A.nRow(), A.nCol(), a, A.data(),
                A.lDim(), x.data(), x.inc(), b, y.data(), y.inc());
}

struct ComplexOrderTrans
{
    CBLAS_ORDER order;
    CBLAS_TRANSPOSE trans;
};

template <typename M>
static ComplexOrderTrans co(const M& A)
{
    ComplexOrderTrans res;
    if (M::storage == ColMajor)
    {
        if (M::conjugation == Conj)
        {
            res.order = CblasRowMajor;
            res.trans = CblasConjTrans;
        }
        else
        {
            res.order = CblasColMajor;
            res.trans = CblasNoTrans;
        }
    }
    else
    {
        if (M::conjugation == Conj)
        {
            res.order = CblasColMajor;
            res.trans = CblasConjTrans;
        }
        else
        {
            res.order = CblasRowMajor;
            res.trans = CblasNoTrans;
        }
    }
    return res;
}

void gemv(Complex a, MatrixView<Complex, General, RowMajor, NoConj, Const> A,
          VectorView<Complex, NoConj, Const> x, Complex b,
          VectorView<Complex, NoConj, Mutable> y)
{
    assert(A.nCol() == x.size());
    assert(A.nRow() == y.size());
    auto[order, trans] = co(A);
    cblas_zgemv(order, trans, A.nRow(), A.nCol(), &a, A.data(), A.lDim(),
                x.data(), x.inc(), &b, y.data(), y.inc());
}

void gemv(Complex a, MatrixView<Complex, General, RowMajor, Conj, Const> A,
          VectorView<Complex, NoConj, Const> x, Complex b,
          VectorView<Complex, NoConj, Mutable> y)
{
    assert(A.nCol() == x.size());
    assert(A.nRow() == y.size());
    auto[order, trans] = co(A);
    cblas_zgemv(order, trans, A.nRow(), A.nCol(), &a, A.data(), A.lDim(),
                x.data(), x.inc(), &b, y.data(), y.inc());
}
void gemv(Complex a, MatrixView<Complex, General, ColMajor, NoConj, Const> A,
          VectorView<Complex, NoConj, Const> x, Complex b,
          VectorView<Complex, NoConj, Mutable> y)
{
    assert(A.nCol() == x.size());
    assert(A.nRow() == y.size());
    auto[order, trans] = co(A);
    cblas_zgemv(order, trans, A.nRow(), A.nCol(), &a, A.data(), A.lDim(),
                x.data(), x.inc(), &b, y.data(), y.inc());
}
void gemv(Complex a, MatrixView<Complex, General, ColMajor, Conj, Const> A,
          VectorView<Complex, NoConj, Const> x, Complex b,
          VectorView<Complex, NoConj, Mutable> y)
{
    assert(A.nCol() == x.size());
    assert(A.nRow() == y.size());
    auto[order, trans] = co(A);
    cblas_zgemv(order, trans, A.nRow(), A.nCol(), &a, A.data(), A.lDim(),
                x.data(), x.inc(), &b, y.data(), y.inc());
}

// // x := A * x
// void trmv(MatrixView<Real, Upper> A, VectorMutView<Real> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     cblas_dtrmv(mo(A), CblasUpper, CblasNoTrans, CblasNonUnit, A.nRow(),
//                 A.data(), A.lDim(), x.data(), x.inc());
// }
// void trmv(MatrixView<Real, Lower> A, VectorMutView<Real> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     cblas_dtrmv(mo(A), CblasLower, CblasNoTrans, CblasNonUnit, A.nRow(),
//                 A.data(), A.lDim(), x.data(), x.inc());
// }

// void trmv(MatrixView<Real, StrictUpper> A, VectorMutView<Real> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     cblas_dtrmv(mo(A), CblasUpper, CblasNoTrans, CblasUnit, A.nRow(),
//     A.data(),
//                 A.lDim(), x.data(), x.inc());
// }
// void trmv(MatrixView<Real, StrictLower> A, VectorMutView<Real> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     cblas_dtrmv(mo(A), CblasLower, CblasNoTrans, CblasUnit, A.nRow(),
//     A.data(),
//                 A.lDim(), x.data(), x.inc());
// }
// void trmv(MatrixView<Complex, Upper> A, VectorMutView<Complex> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     auto[order, trans] = co(A);
//     cblas_ztrmv(order, CblasUpper, trans, CblasNonUnit, A.nRow(), A.data(),
//                 A.lDim(), x.data(), x.inc());
// }
// void trmv(MatrixView<Complex, Lower> A, VectorMutView<Complex> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     auto[order, trans] = co(A);
//     cblas_ztrmv(order, CblasLower, trans, CblasNonUnit, A.nRow(), A.data(),
//                 A.lDim(), x.data(), x.inc());
// }
// void trmv(MatrixView<Complex, StrictUpper> A, VectorMutView<Complex> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     auto[order, trans] = co(A);
//     cblas_ztrmv(order, CblasUpper, trans, CblasUnit, A.nRow(), A.data(),
//                 A.lDim(), x.data(), x.inc());
// }
// void trmv(MatrixView<Complex, StrictLower> A, VectorMutView<Complex> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     auto[order, trans] = co(A);
//     cblas_ztrmv(order, CblasLower, trans, CblasUnit, A.nRow(), A.data(),
//                 A.lDim(), x.data(), x.inc());
// }

// // solve A x = b
// void trsv(MatrixView<Real, Upper> A, VectorMutView<Real> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     cblas_dtrsv(mo(A), CblasUpper, CblasNoTrans, CblasNonUnit, A.nRow(),
//                 A.data(), A.lDim(), x.data(), x.inc());
// }
// void trsv(MatrixView<Real, Lower> A, VectorMutView<Real> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     cblas_dtrsv(mo(A), CblasLower, CblasNoTrans, CblasNonUnit, A.nRow(),
//                 A.data(), A.lDim(), x.data(), x.inc());
// }
// // unit diagonal
// void trsv(MatrixView<Real, StrictUpper> A, VectorMutView<Real> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     cblas_dtrsv(mo(A), CblasUpper, CblasNoTrans, CblasUnit, A.nRow(),
//     A.data(),
//                 A.lDim(), x.data(), x.inc());
// }
// void trsv(MatrixView<Real, StrictLower> A, VectorMutView<Real> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     cblas_dtrsv(mo(A), CblasLower, CblasNoTrans, CblasUnit, A.nRow(),
//     A.data(),
//                 A.lDim(), x.data(), x.inc());
// }
// // solve A x = b
// void trsv(MatrixView<Complex, Upper> A, VectorMutView<Complex> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     auto[order, trans] = co(A);
//     cblas_ztrsv(order, CblasUpper, trans, CblasNonUnit, A.nRow(), A.data(),
//                 A.lDim(), x.data(), x.inc());
// }
// void trsv(MatrixView<Complex, Lower> A, VectorMutView<Complex> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     auto[order, trans] = co(A);
//     cblas_ztrsv(order, CblasLower, trans, CblasNonUnit, A.nRow(), A.data(),
//                 A.lDim(), x.data(), x.inc());
// }
// // unit diagonal
// void trsv(MatrixView<Complex, StrictUpper> A, VectorMutView<Complex> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     auto[order, trans] = co(A);
//     cblas_ztrsv(order, CblasUpper, trans, CblasUnit, A.nRow(), A.data(),
//                 A.lDim(), x.data(), x.inc());
// }
// void trsv(MatrixView<Complex, StrictLower> A, VectorMutView<Complex> x)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     auto[order, trans] = co(A);
//     cblas_ztrsv(order, CblasLower, trans, CblasUnit, A.nRow(), A.data(),
//                 A.lDim(), x.data(), x.inc());
// }

// // y := a * A * x + b * y
// void symv(Real a, MatrixView<Real, Upper> A, VectorView<Real> x, Real b,
//           VectorMutView<Real> y)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     assert(A.nRow() == y.size());
//     auto uplo = A.colMajor() ? CblasLower : CblasUpper;
//     cblas_dsymv(mo(A), uplo, A.nRow(), a, A.data(), A.lDim(), x.data(),
//     x.inc(),
//                 b, y.data(), y.inc());
// }
// void symv(Real a, MatrixView<Real, Lower> A, VectorView<Real> x, Real b,
//           VectorMutView<Real> y)
// {
//     assert(A.nRow() == A.nCol());
//     assert(A.nRow() == x.size());
//     assert(A.nRow() == y.size());
//     auto uplo = A.colMajor() ? CblasUpper : CblasLower;
//     cblas_dsymv(mo(A), uplo, A.nRow(), a, A.data(), A.lDim(), x.data(),
//     x.inc(),
//                 b, y.data(), y.inc());
// }

// // A := a*x*y'+ A
// void ger(Real a, VectorView<Real> x, VectorView<Real> y,
//          MatrixMutView<Real, General> A);

// // A := a*x*x' + A
// void syr(Real a, VectorView<Real> x, MatrixMutView<Real, General> A);

// // A := a*x*y'+ a*y*x' + A
// void syr2(Real a, VectorView<Real> x, VectorView<Real> y,
//           MatrixMutView<Real, General> A);
//
}
