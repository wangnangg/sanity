#include <gtest/gtest.h>
#include "linear.hpp"
using namespace sanity::linear;
TEST(linear_decomp, QR1)
{
    auto A = Matrix<Real>(3, 3,
                          {
                              1, 2, 3,  //
                              4, 5, 6,  //
                              7, 8, 9   //
                          });
    auto[Q, R] = decompQR(A);

    std::cout << Q << std::endl;
    std::cout << R << std::endl;
}
