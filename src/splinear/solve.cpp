#include "solve.hpp"
#include "utils.hpp"

namespace sanity::splinear
{
using namespace linear;
IterationResult solveSor(const Spmatrix& A, linear::VectorMutableView x,
                         linear::VectorConstView b, Real w, Real tol,
                         uint max_iter)
{
    assert(A.nrow == A.ncol);
    assert(A.nrow == (uint)x.size());
    assert(A.nrow == (uint)b.size());
    assert(x.size() > 0);
    assert(A.format == Spmatrix::RowCompressed);
    /* debug info
    std::cout << "Sor trying to solve A = " << A << std::endl;
    std::cout << "b = " << b << std::endl;
    std::cout << "guess = " << x << std::endl;
    */
    uint iter;
    Real error;
    auto x_prev = Vector(x.size());
    for (iter = 1; iter <= max_iter; iter++)
    {
        copy(constView(x), mutableView(x_prev));
        for (uint i = 0; i < A.nrow; i++)
        {
            auto iter = initRowIter(A, i);
            Real residual = b(i);
            Real a_ii = 0;
            while (!iter.end())
            {
                if (iter.col() == i)  // diag
                {
                    a_ii = iter.val();
                }
                else
                {  // non diag
                    residual -= iter.val() * x(iter.col());
                }
                iter.nextNonzero();
            }
            assert(std::abs(a_ii) > tol);
            x(i) = w * residual / a_ii + (1 - w) * x(i);
        }
        /* debug info
        std::cout << x << std::endl;
        */
        error = maxDiff(x, x_prev);
        if (error < tol)
        {
            break;
        }
    }
    return {.error = error, .nIter = iter};
}

}  // namespace sanity::splinear
