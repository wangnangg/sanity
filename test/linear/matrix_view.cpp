#include <gtest/gtest.h>
#include "linear.hpp"

using namespace sanity::linear;

TEST(linear_matrix_view, viewportCast)
{
    auto A = Matrix<Real>(5, 6,
                          {
                              1, 2, 3, 4, 5, 6,  //
                              2, 3, 4, 5, 6, 1,  //
                              3, 4, 5, 6, 1, 2,  //
                              4, 5, 6, 1, 2, 3,  //
                              5, 6, 1, 2, 3, 4   //
                          });

    std::cout << A << std::endl;
    std::cout << viewportCast<General>(constView(A)) << std::endl;
    std::cout << viewportCast<Upper>(constView(A)) << std::endl;
    std::cout << viewportCast<StrictUpper>(constView(A)) << std::endl;
    std::cout << viewportCast<Lower>(constView(A)) << std::endl;
    std::cout << viewportCast<StrictLower>(constView(A)) << std::endl;
    std::cout << viewportCast<Diagonal>(constView(A)) << std::endl;
}

TEST(linear_matrix_view, blockView)
{
    auto A = Matrix<Real>(5, 6,
                          {
                              1, 2, 3, 4, 5, 6,  //
                              2, 3, 4, 5, 6, 1,  //
                              3, 4, 5, 6, 1, 2,  //
                              4, 5, 6, 1, 2, 3,  //
                              5, 6, 1, 2, 3, 4   //
                          });
    {
        auto mA = blockView(constView(A), 1, 3, -1, -1);
        auto mA_ = Matrix<Real>(4, 3,
                                {
                                    5, 6, 1,  //
                                    6, 1, 2,  //
                                    1, 2, 3,  //
                                    2, 3, 4   //
                                });
        std::cout << mA << std::endl;
        std::cout << mA_ << std::endl;
        ASSERT_LT(maxDiff(mA, mA_), 1e-20);
    }
}
