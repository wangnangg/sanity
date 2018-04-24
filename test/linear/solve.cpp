#include <gtest/gtest.h>
#include <cmath>
#include "linear.hpp"

using namespace sanity::linear;
TEST(linear_solve, gesv)
{
    const auto A = createMatrix(3, 3,
                                {

                                    12, -51, 4,   //
                                    6, 167, -68,  //
                                    -4, 24, -41,  //
                                });

    const auto b = createVector(3, {1, 2, 3});
    auto x = b;
    auto workA = A;
    solve(mutableView(workA), mutableView(x));
    auto b_ = dot(A, x);
    std::cout << "A:\n" << A << std::endl;
    std::cout << "x:\n" << x << std::endl;
    std::cout << "b:\n" << b << std::endl;
    ASSERT_LT(maxDiff(b, b_), 1e-10);
}

TEST(linear_solve, lapack_gesv)
{
    const auto A = createMatrix(3, 3,
                                {

                                    12, -51, 4,   //
                                    6, 167, -68,  //
                                    -4, 24, -41,  //
                                });

    const auto b = createMatrix(3, 2,
                                {
                                    1, 2,  //
                                    2, 4,  //
                                    3, 6   //
                                });
    auto x = b;
    auto workA = A;
    std::vector<int> perm((uint)A.nrow());
    lapack::gesv(mutableView(workA), mutableView(x), perm);
    auto b_ = dot(A, x);
    std::cout << "A:\n" << A << std::endl;
    std::cout << "x:\n" << x << std::endl;
    std::cout << "b:\n" << b << std::endl;
    ASSERT_LT(maxDiff(b, b_), 1e-10);
}
