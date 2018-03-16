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
static enum CBLAS_ORDER mo(const M& A)
{
    return A.isRowMajor() ? CblasRowMajor : CblasColMajor;
}
}
