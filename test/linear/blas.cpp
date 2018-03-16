#include <gtest/gtest.h>
#include <cmath>
#include "linear.hpp"
using namespace sanity::linear;
TEST(linear_blas, real_dot)
{
    auto x = Vector<Real>(3, {1, 2, 3});
    auto y = Vector<Real>(3, {1, 1, 1});
    auto res = blas::dot(x, y);
    ASSERT_EQ(res, 6);
}
TEST(linear_blas, complex_dotc)
{
    auto x = Vector<Complex>(3, {Complex(1, 1), Complex(2, 2), Complex(3, 3)});
    auto y = Vector<Complex>(3, {Complex(1, 2), Complex(-1, 1), Complex(2, 0)});
    auto res = blas::dotc(x, y);
    ASSERT_EQ(res, Complex(9, -1));
}

TEST(linear_blas, complex_dotu)
{
    auto x = Vector<Complex>(3, {Complex(1, 1), Complex(2, 2), Complex(3, 3)});
    auto y = Vector<Complex>(3, {Complex(1, 2), Complex(-1, 1), Complex(2, 0)});
    auto res = blas::dotu(x, y);
    ASSERT_EQ(res, Complex(1, 9));
}

TEST(linear_blas, nrm2)
{
    auto x = Vector<Real>(2, {3, 4});
    ASSERT_EQ(blas::nrm2(x), 5);

    auto y = Vector<Complex>(2, {Complex(3, 4), Complex(3, 4)});
    ASSERT_DOUBLE_EQ(blas::nrm2(y), std::sqrt(50));
}

TEST(linear_blas, asum)
{
    auto x = Vector<Real>(2, {3, 4});
    ASSERT_EQ(blas::asum(x), 7);

    auto y = Vector<Complex>(2, {Complex(3, 4), Complex(3, 4)});
    ASSERT_DOUBLE_EQ(blas::asum(y), 14);
}

TEST(linear_blas, iamax)
{
    auto x = Vector<Real>(2, {3, 4});
    ASSERT_EQ(blas::iamax(x), 1);

    auto y = Vector<Complex>(2, {Complex(5, 4), Complex(3, 4)});
    ASSERT_DOUBLE_EQ(blas::iamax(y), 0);
}

TEST(linear_blas, swap)
{
    {
        auto x = Vector<Real>(3, {1, 2, 3});
        auto y = Vector<Real>(3, 0);
        blas::swap(x.mutView(), y.mutView());
        ASSERT_EQ(x(0), 0);
        ASSERT_EQ(x(1), 0);
        ASSERT_EQ(x(2), 0);
        ASSERT_EQ(y(0), 1);
        ASSERT_EQ(y(1), 2);
        ASSERT_EQ(y(2), 3);
    }
    {
        auto x =
            Vector<Complex>(3, {Complex(1, 2), Complex(2, 3), Complex(3, 4)});
        auto y = Vector<Complex>(3);
        blas::swap(x.mutView(), y.mutView());
        ASSERT_LT(maxDiff(x, Vector<Complex>(3)), 1e-6);
        ASSERT_LT(maxDiff(y, Vector<Complex>(3, {Complex(1, 2), Complex(2, 3),
                                                 Complex(3, 4)})),
                  1e-6);
    }
}

TEST(linear_blas, copy)
{
    {
        auto x = Vector<Real>(3, {1, 2, 3});
        auto y = Vector<Real>(3, 0);
        blas::copy(x, y.mutView());
        ASSERT_EQ(x(0), 1);
        ASSERT_EQ(x(1), 2);
        ASSERT_EQ(x(2), 3);

        ASSERT_EQ(y(0), 1);
        ASSERT_EQ(y(1), 2);
        ASSERT_EQ(y(2), 3);
    }
    {
        auto x =
            Vector<Complex>(3, {Complex(1, 2), Complex(2, 3), Complex(3, 4)});
        auto y = Vector<Complex>(3);
        blas::copy(x, y.mutView());
        ASSERT_LT(maxDiff(x, Vector<Complex>(3, {Complex(1, 2), Complex(2, 3),
                                                 Complex(3, 4)})),
                  1e-6);
        ASSERT_LT(maxDiff(y, Vector<Complex>(3, {Complex(1, 2), Complex(2, 3),
                                                 Complex(3, 4)})),
                  1e-6);
    }
}

TEST(linear_blas, axpy)
{
    {
        auto x = Vector<Real>(3, {1, 2, 3});
        auto y = Vector<Real>(3, {1, 1, 1});
        blas::axpy(0.5, x, y.mutView());
        ASSERT_LT(maxDiff(y, Vector<Real>(3, {1.5, 2, 2.5})), 1e-6);
    }
    {
        auto x =
            Vector<Complex>(3, {Complex(1, 2), Complex(2, 3), Complex(3, 4)});
        auto y =
            Vector<Complex>(3, {Complex(1, 2), Complex(2, 3), Complex(3, 4)});
        blas::axpy(1, x, y.mutView());
        ASSERT_LT(maxDiff(y, Vector<Complex>(3, {Complex(2, 4), Complex(4, 6),
                                                 Complex(6, 8)})),
                  1e-6);
    }
}

TEST(linear_blas, scal)
{
    auto x = Vector<Real>(3, {1, 2, 3});
    blas::scal(2, x.mutView());
    ASSERT_LT(maxDiff(x, Vector<Real>(3, {2, 4, 6})), 1e-6);
    {
        auto y =
            Vector<Complex>(3, {Complex(1, 2), Complex(2, 3), Complex(3, 4)});
        blas::scal(2, y.mutView());
        ASSERT_LT(maxDiff(y, Vector<Complex>(3, {Complex(2, 4), Complex(4, 6),
                                                 Complex(6, 8)})),
                  1e-6);
    }
    {
        auto y =
            Vector<Complex>(3, {Complex(1, 2), Complex(2, 3), Complex(3, 4)});
        blas::scal(Complex(0, 1), y.mutView());
        ASSERT_LT(maxDiff(y, Vector<Complex>(3, {Complex(-2, 1), Complex(-3, 2),
                                                 Complex(-4, 3)})),
                  1e-6);
    }
}
