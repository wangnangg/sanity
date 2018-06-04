#include "blas.hpp"
#include <cblas.h>
namespace sanity::linear::blas
{
Real dot(VectorConstView x, VectorConstView y)
{
    assert(x.size() == y.size());
    return cblas_ddot((int)x.size(), &x(0), (int)x.inc(), &y(0),
                      (int)y.inc());
}

Real nrm2(VectorConstView x)
{
    return cblas_dnrm2((int)x.size(), &x(0), (int)x.inc());
}

Real asum(VectorConstView x)
{
    return cblas_dasum((int)x.size(), &x(0), (int)x.inc());
}

int iamax(VectorConstView x)
{
    return cblas_idamax((int)x.size(), &x(0), (int)x.inc());
}

void swap(VectorMutableView x, VectorMutableView y)
{
    assert(x.size() == y.size());
    cblas_dswap((int)x.size(), &x(0), (int)x.inc(), &y(0), (int)y.inc());
}

void copy(VectorConstView x, VectorMutableView y)
{
    assert(x.size() == y.size());
    cblas_dcopy((int)x.size(), &x(0), (int)x.inc(), &y(0), (int)y.inc());
}

void axpy(Real a, VectorConstView x, VectorMutableView y)
{
    assert(x.size() == y.size());
    cblas_daxpy((int)x.size(), a, &x(0), (int)x.inc(), &y(0), (int)y.inc());
}

void scal(Real a, VectorMutableView x)
{
    cblas_dscal((int)x.size(), a, &x(0), (int)x.inc());
}

void scal(Complex a, CVectorMutableView x)
{
    cblas_zscal((int)x.size(), (double *)&a, (double *)&x(0), (int)x.inc());
}

static CBLAS_ORDER order = CblasRowMajor;
CBLAS_TRANSPOSE blasOper(Oper op)
{
    switch (op)
    {
        case Oper::NoTranspose:
            return CblasNoTrans;
        case Oper::Transpose:
            return CblasTrans;
        case Oper::ConjTranspose:
            return CblasConjTrans;
    }
    assert(false);
    return CblasNoTrans;
}
void gemv(Real a, MatrixConstView A, Oper op, VectorConstView x, Real b,
          VectorMutableView y)

{
    if (op == Oper::NoTranspose)
    {
        assert(A.ncol() == x.size());
        assert(A.nrow() == y.size());
    }
    else
    {
        assert(A.nrow() == x.size());
        assert(A.ncol() == y.size());
    }
    cblas_dgemv(order, blasOper(op), (int)A.nrow(), (int)A.ncol(), a,
                &A(0, 0), (int)A.ldim(), &x(0), (int)x.inc(), b, &y(0),
                (int)y.inc());
}

void gemv(Complex a, CMatrixConstView A, Oper op, CVectorConstView x,
          Complex b, CVectorMutableView y)
{
    if (op == Oper::NoTranspose)
    {
        assert(A.ncol() == x.size());
        assert(A.nrow() == y.size());
    }
    else
    {
        assert(A.nrow() == x.size());
        assert(A.ncol() == y.size());
    }
    cblas_zgemv(order, blasOper(op), (int)A.nrow(), (int)A.ncol(),
                (double *)&a, (double *)&A(0, 0), (int)A.ldim(),
                (double *)&x(0), (int)x.inc(), (double *)&b, (double *)&y(0),
                (int)y.inc());
}

void gemm(Real a, MatrixConstView A, Oper opA, MatrixConstView B, Oper opB,
          Real b, MatrixMutableView C)
{
    uint m, n, k;
    if (opA == Oper::NoTranspose)
    {
        m = A.nrow();
        k = A.ncol();
    }
    else
    {
        m = A.ncol();
        k = A.nrow();
    }
    if (opB == Oper::NoTranspose)
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
    cblas_dgemm(order, blasOper(opA), blasOper(opB), (int)m, (int)n, (int)k,
                a, &A(0, 0), (int)A.ldim(), &B(0, 0), (int)B.ldim(), b,
                &C(0, 0), (int)C.ldim());
}

}  // namespace sanity::linear::blas
