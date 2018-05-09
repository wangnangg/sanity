#include "oper.hpp"
#include <cassert>
namespace sanity::splinear
{
void dot(const Spmatrix& A, linear::VectorConstView v,
         linear::VectorMutableView x)
{
    assert(A.ncol == v.size());
    assert(A.nrow == x.size());
    if (A.format == Spmatrix::RowCompressed)
    {
        for (uint row = 0; row < A.nrow; row++)
        {
            x(row) = 0.0;
            auto iter = initRowIter(A, row);
            while (!iter.end())
            {
                x(row) += iter.val() * v(iter.col());
                iter.nextNonzero();
            };
        }
    }
    else
    {
        fill(0.0, x);
        for (uint col = 0; col < A.ncol; col++)
        {
            auto iter = initColIter(A, col);
            while (!iter.end())
            {
                x(iter.row()) += iter.val() * v(col);
                iter.nextNonzero();
            };
        }
    }
}

void dotpx(const Spmatrix& A, linear::VectorConstView v,
           linear::VectorMutableView x)
{
    assert(A.ncol == v.size());
    assert(A.nrow == x.size());
    if (A.format == Spmatrix::RowCompressed)
    {
        for (uint row = 0; row < A.nrow; row++)
        {
            auto iter = initRowIter(A, row);
            while (!iter.end())
            {
                x(row) += iter.val() * v(iter.col());
                iter.nextNonzero();
            };
        }
    }
    else
    {
        for (uint col = 0; col < A.ncol; col++)
        {
            auto iter = initColIter(A, col);
            while (!iter.end())
            {
                x(iter.row()) += iter.val() * v(col);
                iter.nextNonzero();
            };
        }
    }
}
}  // namespace sanity::splinear
