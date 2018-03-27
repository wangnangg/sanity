#include <gtest/gtest.h>
#include <cmath>
#include "linear.hpp"
using namespace sanity::linear;
TEST(linear_blas, dot)
{
    auto x = createVector(3, {1, 2, 3});
    auto y = createVector(3, {1, 1, 1});
    auto res = blas::dot(x, y);
    ASSERT_EQ(res, 6);
}
TEST(linear_blas, nrm2)
{
    auto x = createVector(2, {3, 4});
    ASSERT_EQ(blas::nrm2(x), 5);
}

TEST(linear_blas, asum)
{
    auto x = createVector(2, {3, 4});
    ASSERT_EQ(blas::asum(x), 7);
}

TEST(linear_blas, iamax)
{
    auto x = createVector(2, {3, 4});
    ASSERT_EQ(blas::iamax(x), 1);
}

TEST(linear_blas, swap)
{
    auto x = createVector(3, {1, 2, 3});
    auto y = Vector(3, 0);
    blas::swap(x.mut(), y.mut());
    ASSERT_EQ(x(0), 0);
    ASSERT_EQ(x(1), 0);
    ASSERT_EQ(x(2), 0);
    ASSERT_EQ(y(0), 1);
    ASSERT_EQ(y(1), 2);
    ASSERT_EQ(y(2), 3);
}

TEST(linear_blas, copy)
{
    auto x = createVector(3, {1, 2, 3});
    auto y = Vector(3, 0);
    blas::copy(x, y.mut());
    ASSERT_EQ(x(0), 1);
    ASSERT_EQ(x(1), 2);
    ASSERT_EQ(x(2), 3);

    ASSERT_EQ(y(0), 1);
    ASSERT_EQ(y(1), 2);
    ASSERT_EQ(y(2), 3);
}

TEST(linear_blas, axpy)
{
    auto x = createVector(3, {1, 2, 3});
    auto y = createVector(3, {1, 1, 1});
    blas::axpy(0.5, x, y.mut());
    ASSERT_LT(maxDiff(y, createVector(3, {1.5, 2, 2.5})), 1e-6);
}

TEST(linear_blas, scal)
{
    auto x = createVector(3, {1, 2, 3});
    blas::scal(2, x.mut());
    ASSERT_LT(maxDiff(x, createVector(3, {2, 4, 6})), 1e-6);
}

TEST(linear_blas, gemv)
{
    const auto A = createMatrix(3, 4,
                                {
                                    1, 0, 0, 2,  //
                                    1, 1, 0, 0,  //
                                    0, 0, 1, 1,  //
                                });
    const auto At = createMatrix(4, 3,
                                 {
                                     1, 1, 0,  //
                                     0, 1, 0,  //
                                     0, 0, 1,  //
                                     2, 0, 1   //
                                 });

    auto x = createVector(4, {1, 1, 1, 1});
    auto y = createVector(3, {1, 2, 3});
    blas::gemv(2, A, x, 1, y.mut());
    ASSERT_LT(maxDiff(y, createVector(3, {7, 6, 7})), 1e-6);
}
