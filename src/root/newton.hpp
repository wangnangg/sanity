#pragma once
#include <functional>
#include "linear.hpp"
#include "type.hpp"

namespace sanity::root
{
IterationResult newton(
    linear::VectorMutableView x0,
    const std::function<
        void(linear::VectorConstView x, linear::VectorMutableView y,
             linear::MatrixMutableView jacobian)>& target_func,
    const std::function<void(linear::MatrixMutableView A,
                             linear::VectorMutableView bx)>& linear_solver,
    int max_iter, Real tol);
}
