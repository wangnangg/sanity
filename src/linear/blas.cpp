#include "blas.hpp"
#include "blas/cblas.h"
namespace sanity::linear::blas
{
Real dot(VectorView<Real> x, VectorView<Real> y)
{
    assert(x.size() == y.size());
    return cblas_ddot(x.size(), x.data(), x.inc(), y.data(), y.inc());
}

Complex dotc(VectorView<Complex> x, VectorView<Complex> y)
{
    assert(x.size() == y.size());
    Complex res;
    cblas_zdotc_sub(x.size(), x.data(), x.inc(), y.data(), y.inc(), &res);
    return res;
}

Complex dotu(VectorView<Complex> x, VectorView<Complex> y)
{
    assert(x.size() == y.size());
    Complex res;
    cblas_zdotu_sub(x.size(), x.data(), x.inc(), y.data(), y.inc(), &res);
    return res;
}

Real nrm2(VectorView<Real> x)
{
    return cblas_dnrm2(x.size(), x.data(), x.inc());
}
Real nrm2(VectorView<Complex> x)
{
    return cblas_dznrm2(x.size(), x.data(), x.inc());
}
Real asum(VectorView<Real> x)
{
    return cblas_dasum(x.size(), x.data(), x.inc());
}
Real asum(VectorView<Complex> x)
{
    return cblas_dzasum(x.size(), x.data(), x.inc());
}

int iamax(VectorView<Real> x)
{
    return cblas_idamax(x.size(), x.data(), x.inc());
}
int iamax(VectorView<Complex> x)
{
    return cblas_izamax(x.size(), x.data(), x.inc());
}

void swap(VectorMutView<Real> x, VectorMutView<Real> y)
{
    assert(x.size() == y.size());
    cblas_dswap(x.size(), x.data(), x.inc(), y.data(), y.inc());
}
void swap(VectorMutView<Complex> x, VectorMutView<Complex> y)
{
    assert(x.size() == y.size());
    cblas_zswap(x.size(), x.data(), x.inc(), y.data(), y.inc());
}
void copy(VectorView<Real> x, VectorMutView<Real> y)
{
    assert(x.size() == y.size());
    cblas_dcopy(x.size(), x.data(), x.inc(), y.data(), y.inc());
}
void copy(VectorView<Complex> x, VectorMutView<Complex> y)
{
    assert(x.size() == y.size());
    cblas_zcopy(x.size(), x.data(), x.inc(), y.data(), y.inc());
}
void axpy(Real a, VectorView<Real> x, VectorMutView<Real> y)
{
    assert(x.size() == y.size());
    cblas_daxpy(x.size(), a, x.data(), x.inc(), y.data(), y.inc());
}
void axpy(Complex a, VectorView<Complex> x, VectorMutView<Complex> y)
{
    assert(x.size() == y.size());
    cblas_zaxpy(x.size(), &a, x.data(), x.inc(), y.data(), y.inc());
}

void scal(Real a, VectorMutView<Real> x)
{
    cblas_dscal(x.size(), a, x.data(), x.inc());
}
void scal(Real a, VectorMutView<Complex> x)
{
    cblas_zdscal(x.size(), a, x.data(), x.inc());
}
void scal(Complex a, VectorMutView<Complex> x)
{
    cblas_zscal(x.size(), &a, x.data(), x.inc());
}

template <typename M>
static enum CBLAS_ORDER mo(const M &A)
{
    return A.colMajor() ? CblasColMajor : CblasRowMajor;
}
template <typename M>
static enum CBLAS_TRANSPOSE mt(const M &A)
{
    return CblasNoTrans;
}

void gemv(Real a, MatrixView<Real, General> A, VectorView<Real> x, Real b,
          VectorMutView<Real> y)
{
    assert(A.nCol() == x.size());
    assert(A.nRow() == y.size());
    cblas_dgemv(mo(A), mt(A), A.nRow(), A.nCol(), a, A.data(), A.lDim(),
                x.data(), x.inc(), b, y.data(), y.inc());
}

struct ComplexOrderTrans
{
    CBLAS_ORDER order;
    CBLAS_TRANSPOSE trans;
};

template <MatrixViewport vt>
static ComplexOrderTrans co(const MatrixView<Complex, vt> &A)
{
    ComplexOrderTrans res;
    if (A.colMajor())
    {
        if (A.conjugated())
        {
            res.order = CblasRowMajor;
            res.trans = CblasConjTrans;
        }
        else
        {
            res.order = CblasRowMajor;
            res.trans = CblasTrans;
        }
    }
    else
    {
        if (A.conjugated())
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

void gemv(Complex a, MatrixView<Complex, General> A, VectorView<Complex> x,
          Complex b, VectorMutView<Complex> y)
{
    assert(A.nCol() == x.size());
    assert(A.nRow() == y.size());
    auto[order, trans] = co(A);
    cblas_zgemv(order, trans, A.nRow(), A.nCol(), &a, A.data(), A.lDim(),
                x.data(), x.inc(), &b, y.data(), y.inc());
}

// x := A * x
void trmv(MatrixView<Real, Upper> A, VectorMutView<Real> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    cblas_dtrmv(mo(A), CblasUpper, mt(A), CblasNonUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}
void trmv(MatrixView<Real, Lower> A, VectorMutView<Real> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    cblas_dtrmv(mo(A), CblasLower, mt(A), CblasNonUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}

void trmv(MatrixView<Real, StrictUpper> A, VectorMutView<Real> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    cblas_dtrmv(mo(A), CblasUpper, mt(A), CblasUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}
void trmv(MatrixView<Real, StrictLower> A, VectorMutView<Real> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    cblas_dtrmv(mo(A), CblasLower, mt(A), CblasUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}
void trmv(MatrixView<Complex, Upper> A, VectorMutView<Complex> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    auto[order, trans] = co(A);
    cblas_ztrmv(order, CblasUpper, trans, CblasNonUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}
void trmv(MatrixView<Complex, Lower> A, VectorMutView<Complex> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    auto[order, trans] = co(A);
    cblas_ztrmv(order, CblasLower, trans, CblasNonUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}
void trmv(MatrixView<Complex, StrictUpper> A, VectorMutView<Complex> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    auto[order, trans] = co(A);
    cblas_ztrmv(order, CblasUpper, trans, CblasUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}
void trmv(MatrixView<Complex, StrictLower> A, VectorMutView<Complex> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    auto[order, trans] = co(A);
    cblas_ztrmv(order, CblasLower, trans, CblasUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}

// solve A x = b
void trsv(MatrixView<Real, Upper> A, VectorMutView<Real> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    cblas_dtrsv(mo(A), CblasUpper, mt(A), CblasNonUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}
void trsv(MatrixView<Real, Lower> A, VectorMutView<Real> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    cblas_dtrsv(mo(A), CblasLower, mt(A), CblasNonUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}
// unit diagonal
void trsv(MatrixView<Real, StrictUpper> A, VectorMutView<Real> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    cblas_dtrsv(mo(A), CblasUpper, mt(A), CblasUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}
void trsv(MatrixView<Real, StrictLower> A, VectorMutView<Real> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    cblas_dtrsv(mo(A), CblasLower, mt(A), CblasUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}
// solve A x = b
void trsv(MatrixView<Complex, Upper> A, VectorMutView<Complex> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    auto[order, trans] = co(A);
    cblas_ztrsv(order, CblasUpper, trans, CblasNonUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}
void trsv(MatrixView<Complex, Lower> A, VectorMutView<Complex> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    auto[order, trans] = co(A);
    cblas_ztrsv(order, CblasLower, trans, CblasNonUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}
// unit diagonal
void trsv(MatrixView<Complex, StrictUpper> A, VectorMutView<Complex> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    auto[order, trans] = co(A);
    cblas_ztrsv(order, CblasUpper, trans, CblasUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}
void trsv(MatrixView<Complex, StrictLower> A, VectorMutView<Complex> x)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    auto[order, trans] = co(A);
    cblas_ztrsv(order, CblasLower, trans, CblasUnit, A.nRow(), A.data(),
                A.lDim(), x.data(), x.inc());
}

// y := a * A * x + b * y
void symv(Real a, MatrixView<Real, Upper> A, VectorView<Real> x, Real b,
          VectorMutView<Real> y)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    assert(A.nRow() == y.size());
    auto uplo = A.colMajor() ? CblasLower : CblasUpper;
    cblas_dsymv(mo(A), uplo, A.nRow(), a, A.data(), A.lDim(), x.data(), x.inc(),
                b, y.data(), y.inc());
}
void symv(Real a, MatrixView<Real, Lower> A, VectorView<Real> x, Real b,
          VectorMutView<Real> y)
{
    assert(A.nRow() == A.nCol());
    assert(A.nRow() == x.size());
    assert(A.nRow() == y.size());
    auto uplo = A.colMajor() ? CblasUpper : CblasLower;
    cblas_dsymv(mo(A), uplo, A.nRow(), a, A.data(), A.lDim(), x.data(), x.inc(),
                b, y.data(), y.inc());
}

// A := a*x*y'+ A
void ger(Real a, VectorView<Real> x, VectorView<Real> y,
         MatrixMutView<Real, General> A);

// A := a*x*x' + A
void syr(Real a, VectorView<Real> x, MatrixMutView<Real, General> A);

// A := a*x*y'+ a*y*x' + A
void syr2(Real a, VectorView<Real> x, VectorView<Real> y,
          MatrixMutView<Real, General> A);
}
