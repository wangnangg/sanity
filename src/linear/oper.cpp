#include "oper.hpp"
#include <algorithm>
#include "blas.hpp"

namespace sanity::linear
{
void fill(Real a, VectorMutableView v)
{
    for (uint i = 0; i < (uint)v.size(); i++)
    {
        v((uint)i) = a;
    }
}
Real norm2(VectorConstView v) { return blas::nrm2(v); }
Real norm1(VectorConstView v) { return blas::asum(v); }
void scale(Real a, VectorMutableView v) { blas::scal(a, v); }
void scale(Complex a, CVectorMutableView v) { blas::scal(a, v); }

void scale(Real a, MatrixMutableView m)
{
    for (uint i = 0; i < m.nrow(); i++)
    {
        for (uint j = 0; j < m.ncol(); j++)
        {
            m(i, j) *= a;
        }
    }
}
void scale(Complex a, CMatrixMutableView m)
{
    for (uint i = 0; i < m.nrow(); i++)
    {
        for (uint j = 0; j < m.ncol(); j++)
        {
            m(i, j) *= a;
        }
    }
}

void swap(VectorMutableView x, VectorMutableView y) { blas::swap(x, y); }

Real dot(VectorConstView x, VectorConstView y) { return blas::dot(x, y); }
void dot(MatrixConstView A, VectorConstView v, VectorMutableView x)
{
    blas::gemv(1.0, A, blas::NoTranspose, v, 0.0, x);
}
void dot(CMatrixConstView A, CVectorConstView v, CVectorMutableView x)
{
    blas::gemv(1.0, A, blas::NoTranspose, v, 0.0, x);
}

Vector dot(MatrixConstView A, VectorConstView v)
{
    Vector x(A.nrow());
    dot(A, v, mutableView(x));
    return x;
}
CVector dot(CMatrixConstView A, CVectorConstView v)
{
    CVector x(A.nrow());
    dot(A, v, mutableView(x));
    return x;
}

void dot(MatrixConstView A, MatrixConstView B, MatrixMutableView C)
{
    blas::gemm(1.0, A, blas::NoTranspose, B, blas::NoTranspose, 0.0, C);
}
Matrix dot(MatrixConstView A, MatrixConstView B)
{
    uint m = A.nrow();
    uint n = B.ncol();
    auto C = Matrix(m, n);
    dot(A, B, mutableView(C));
    return C;
}
void pointwiseProd(VectorConstView v, VectorConstView w, VectorMutableView x)
{
    assert(v.size() == w.size());
    assert(v.size() == x.size());
    uint n = v.size();
    for (uint i = 0; i < n; i++)
    {
        x(i) = v(i) * w(i);
    }
}
void pointwiseProd(CVectorConstView v, CVectorConstView w,
                   CVectorMutableView x)
{
    assert(v.size() == w.size());
    assert(v.size() == x.size());
    uint n = v.size();
    for (uint i = 0; i < n; i++)
    {
        x(i) = v(i) * w(i);
    }
}

void conjuage(CVectorConstView v, CVectorMutableView x)
{
    assert(v.size() == x.size());
    uint n = v.size();
    for (uint i = 0; i < n; i++)
    {
        x(i) = std::conj(v(i));
    }
}

CVector conjuage(CVectorConstView v)
{
    auto x = CVector(v.size());
    conjuage(v, mutableView(x));
    return x;
}
}  // namespace sanity::linear
