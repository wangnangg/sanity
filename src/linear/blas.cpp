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

int iamax(VectorConstView x) { return cblas_idamax(x.size(), &x(0), x.inc()); }

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

static CBLAS_ORDER order = CblasRowMajor;
static CBLAS_TRANSPOSE trans = CblasNoTrans;
void gemv(Real a, MatrixConstView A, VectorConstView x, Real b,
          VectorMutableView y)

{
    assert(A.ncol() == x.size());
    assert(A.nrow() == y.size());
    cblas_dgemv(order, trans, A.nrow(), A.ncol(), a, &A(0, 0), A.ldim(), &x(0),
                x.inc(), b, &y(0), y.inc());
}
}
