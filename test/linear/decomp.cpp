#include <gtest/gtest.h>
#include "linear.hpp"
using namespace sanity::linear;
TEST(linear_decomp, QR1)
{
    auto A = createMatrix(3, 3,
                          {
                              1, 2, 3,  //
                              4, 5, 6,  //
                              7, 8, 9   //
                          });
    auto A_ = A;
    auto[Q, R] = decompQR(A.mut());

    std::cout << Q << std::endl;
    std::cout << R << std::endl;
}
