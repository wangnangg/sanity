#include <gtest/gtest.h>
#include "linear.hpp"

using namespace sanity::linear;

TEST(linear_matrix_view, viewportCast)
{
    auto _A = Matrix<Real, ColMajor>(5, 6,
                          {
                              1, 2, 3, 4, 5, 6,  //
                              2, 3, 4, 5, 6, 1,  //
                              3, 4, 5, 6, 1, 2,  //
                              4, 5, 6, 1, 2, 3,  //
                              5, 6, 1, 2, 3, 4   //
                          });
    auto A = constView(_A);
    std::cout << A << std::endl;
    std::cout << viewportCast<General>(A) << std::endl;
    std::cout << viewportCast<Upper>(A) << std::endl;
    std::cout << viewportCast<StrictUpper>(A) << std::endl;
    std::cout << viewportCast<Lower>(A) << std::endl;
    std::cout << viewportCast<StrictLower>(A) << std::endl;
    std::cout << viewportCast<Diagonal>(A) << std::endl;
}

TEST(linear_matrix_view, transpose)
{
    auto _A = Matrix<Real, ColMajor>(5, 6,
                          {
                              1, 2, 3, 4, 5, 6,  //
                              2, 3, 4, 5, 6, 1,  //
                              3, 4, 5, 6, 1, 2,  //
                              4, 5, 6, 1, 2, 3,  //
                              5, 6, 1, 2, 3, 4   //
                          });
    auto A = constView(_A);
    auto _AT = Matrix<Real, ColMajor>(6, 5,
                           {
                               1, 2, 3, 4, 5,  //
                               2, 3, 4, 5, 6,  //
                               3, 4, 5, 6, 1,  //
                               4, 5, 6, 1, 2,  //
                               5, 6, 1, 2, 3,  //
                               6, 1, 2, 3, 4   //
                           });
    auto AT = constView(_AT);
    {
        std::cout << A << std::endl;
        std::cout << transpose(AT) << std::endl;
        ASSERT_LT(maxDiff(A, transpose(AT)), 1e-20);
    }
    {
        auto mA = blockView(A, 1, 3, -1, -1);
        auto mAT = blockView(transpose(AT), 1, 3, -1, -1);
        std::cout << mA << std::endl;
        std::cout << mAT << std::endl;
        ASSERT_LT(maxDiff(mA, mAT), 1e-20);
    }
    {
        auto mA = viewportCast<Upper>(blockView(A, 1, 3, -1, -1));
        auto mAT = transpose(viewportCast<Lower>(blockView(AT, 3, 1, -1, -1)));
        std::cout << mA << std::endl;
        std::cout << mAT << std::endl;
        ASSERT_LT(maxDiff(mA, mAT), 1e-20);
    }
    {
        auto mA = viewportCast<StrictUpper>(blockView(A, 1, 3, -1, -1));
        auto mAT =
            transpose(viewportCast<StrictLower>(blockView(AT, 3, 1, -1, -1)));
        std::cout << mA << std::endl;
        std::cout << mAT << std::endl;
        ASSERT_LT(maxDiff(mA, mAT), 1e-20);
    }
    {
        auto mA = viewportCast<Diagonal>(blockView(A, 1, 3, -1, -1));
        auto mAT =
            transpose(viewportCast<Diagonal>(blockView(AT, 3, 1, -1, -1)));
        std::cout << mA << std::endl;
        std::cout << mAT << std::endl;
        ASSERT_LT(maxDiff(mA, mAT), 1e-20);
    }
}
