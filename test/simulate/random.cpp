#include <gtest/gtest.h>
#include <stdexcept>
#include "simulate.hpp"
using namespace sanity::simulate;

TEST(simulate, normal_quantile)
{
    ASSERT_NEAR(stdNormDistQuantile(0.9), 1.282, 0.001);
    ASSERT_NEAR(stdNormDistQuantile(0.95), 1.645, 0.001);
    ASSERT_NEAR(stdNormDistQuantile(0.995), 2.576, 0.001);
    ASSERT_NEAR(stdNormDistQuantile(0.999), 3.090, 0.001);
}

TEST(simulate, confidence_interval)
{
    std::vector<Real> samples = {
        10, 15, 16, 17, 18, 20, 13, 1,  2,  19, 13, 15, 13, 10, 12, 20,
        1,  1,  2,  3,  4,  5,  34, 4,  2,  3,  4,  5,  3,  2,  3,  4,
        3,  4,  5,  2,  3,  4,  5,  6,  23, 3,  4,  3,  3,  2,  23, 34,
        23, 12, 10, 10, 10, 10, 10, 10, 10, 10, 9,  9,  9,  9};

    ASSERT_EQ(samples.size(), 62);

    auto itv = confidenceInterval(samples, 0.95);

    std::cout << "[" << itv.begin << ", " << itv.end << "]" << std::endl;
    ASSERT_NEAR(itv.center(), 9.23, 0.01);
    ASSERT_NEAR(itv.begin, 9.23 - 1.91, 0.01);
    ASSERT_NEAR(itv.end, 9.23 + 1.91, 0.01);
}
