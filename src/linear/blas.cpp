#include "blas.hpp"
#include "blas/cblas.h"
namespace sanity::linear::blas
{
Real dot(VectorConstView x, VectorConstView y)
{
    assert(x.size() == y.size());
    return cblas_ddot(x.size(), &x(0), x.inc(), &y(0), y.inc());
}

Real nrm2(VectorConstView x) { return cblas_dnrm2(x.size(), &x(0), x.inc()); }

Real asum(VectorConstView x) { return cblas_dasum(x.size(), &x(0), x.inc()); }

int iamax(VectorConstView x)
{
    return cblas_idamax(x.size(), &x(0), x.inc());
}

void swap(VectorMutableView x, VectorMutableView y)
{
    assert(x.size() == y.size());
    cblas_dswap(x.size(), &x(0), x.inc(), &y(0), y.inc());
}

void copy(VectorConstView x, VectorMutableView y)
{
    assert(x.size() == y.size());
    cblas_dcopy(x.size(), &x(0), x.inc(), &y(0), y.inc());
}

void axpy(Real a, VectorConstView x, VectorMutableView y)
{
    assert(x.size() == y.size());
    cblas_daxpy(x.size(), a, &x(0), x.inc(), &y(0), y.inc());
}

void scal(Real a, VectorMutableView x)
{
    cblas_dscal(x.size(), a, &x(0), x.inc());
}

void scal(Complex a, CVectorMutableView x)
{
    cblas_zscal(x.size(), &a, &x(0), x.inc());
}

static CBLAS_ORDER order = CblasRowMajor;
constexpr CBLAS_TRANSPOSE blasOper(Oper op)
{
    switch (op)
    {
        case NoTranspose:
            return CblasNoTrans;
        case Transpose:
            return CblasTrans;
        case ConjTranspose:
            return CblasConjTrans;
    }
}
void gemv(Real a, MatrixConstView A, Oper op, VectorConstView x, Real b,
          VectorMutableView y)

{
    if (op == NoTranspose)
    {
        assert(A.ncol() == x.size());
        assert(A.nrow() == y.size());
    }
    else
    {
        assert(A.nrow() == x.size());
        assert(A.ncol() == y.size());
    }
    cblas_dgemv(order, blasOper(op), A.nrow(), A.ncol(), a, &A(0, 0),
                A.ldim(), &x(0), x.inc(), b, &y(0), y.inc());
}

void gemv(Complex a, CMatrixConstView A, Oper op, CVectorConstView x,
          Complex b, CVectorMutableView y)
{
    if (op == NoTranspose)
    {
        assert(A.ncol() == x.size());
        assert(A.nrow() == y.size());
    }
    else
    {
        assert(A.nrow() == x.size());
        assert(A.ncol() == y.size());
    }
    cblas_zgemv(order, blasOper(op), A.nrow(), A.ncol(), &a, &A(0, 0),
                A.ldim(), &x(0), x.inc(), &b, &y(0), y.inc());
}

void gemm(Real a, MatrixConstView A, Oper opA, MatrixConstView B, Oper opB,
          Real b, MatrixMutableView C)
{
    int m, n, k;
    if (opA == NoTranspose)
    {
        m = A.nrow();
        k = A.ncol();
    }
    else
    {
        m = A.ncol();
        k = A.nrow();
    }
    if (opB == NoTranspose)
    {
        n = B.ncol();
        assert(k == B.nrow());
    }
    else
    {
        n = B.nrow();
        assert(k == B.ncol());
    }
    assert(m == C.nrow());
    assert(n == C.ncol());
    cblas_dgemm(order, blasOper(opA), blasOper(opB), m, n, k, a, &A(0, 0),
                A.ldim(), &B(0, 0), B.ldim(), b, &C(0, 0), C.ldim());
}

}  // namespace sanity::linear::blas
