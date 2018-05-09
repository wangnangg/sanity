#include <gtest/gtest.h>
#include "linear.hpp"

using namespace sanity::linear;

TEST(linear_matrix_view, print)
{
    auto A = createMatrix(5, 6,
                          {
                              1, 2, 3, 4, 5, 6,  //
                              2, 3, 4, 5, 6, 1,  //
                              3, 4, 5, 6, 1, 2,  //
                              4, 5, 6, 1, 2, 3,  //
                              5, 6, 1, 2, 3, 4   //
                          });

    std::cout << A << std::endl;
}

TEST(linear_matrix_view, blockView)
{
    auto A = createMatrix(5, 6,
                          {
                              1, 2, 3, 4, 5, 6,  //
                              2, 3, 4, 5, 6, 1,  //
                              3, 4, 5, 6, 1, 2,  //
                              4, 5, 6, 1, 2, 3,  //
                              5, 6, 1, 2, 3, 4   //
                          });
    {
        auto mA = blockView(A, 1, 3, A.nrow() - 1, A.ncol() - 3);
        auto mA_ = createMatrix(4, 3,
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
