
#include <gtest/gtest.h>
#include "linear.hpp"
#include "root.hpp"
using namespace sanity::linear;
using namespace sanity::root;
TEST(root, newton1)
{
    // solve: x^2 - 4 = 0
    auto f = [](VectorConstView x, VectorMutableView y,
                MatrixMutableView jac) {
        y(0) = x(0) * x(0) - 4;
        jac(0, 0) = 2 * x(0);
    };
    auto solver = [](MatrixMutableView A, VectorMutableView xb) {
        xb(0) = xb(0) / A(0, 0);
    };

    Vector x(1);
    x(0) = 10;
    auto res = solveNewton(mutableView(x), f, solver, 100, 1e-6);
    std::cout << x << std::endl;
    std::cout << "error: " << res.error << std::endl;
    std::cout << "iter: " << res.nIter << std::endl;
    ASSERT_LT(res.error, 1e-6);
    ASSERT_NEAR(x(0), 2.0, 1e-6);
}
