#include "newton.hpp"
#include <functional>
#include "linear.hpp"
#include "type.hpp"

using namespace sanity::linear;
namespace sanity::root
{
IterationResult solveNewton(
    VectorMutableView x0,
    const std::function<void(VectorConstView x, VectorMutableView y,
                             MatrixMutableView jacobian)>& target_func,
    const std::function<void(linear::MatrixMutableView A,
                             linear::VectorMutableView bx)>& linear_solver,
    uint max_iter, Real tol)
{
    uint n = x0.size();
    auto y = Vector(n);
    auto jac = Matrix(n, n);
    uint iter;
    Real error;

    for (iter = 0; iter < max_iter; iter++)
    {
        target_func(x0, mutableView(y), mutableView(jac));
        error = maxAbs(y);
        if (error < tol)
        {
            break;
        }
        // d_x = - jac^(-1) y, i.e. jac (-d_x) = y
        linear_solver(mutableView(jac), mutableView(y));

        // now y = -dx
        // update: x0 = x0 + d_x, i.e. x0 = x0 - _d_x
        blas::axpy(-1, y, x0);
    }
    return IterationResult{.error = error, .nIter = iter};
}

}  // namespace sanity::root
