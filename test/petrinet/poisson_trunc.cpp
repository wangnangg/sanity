#include <gtest/gtest.h>
#include "petrinet.hpp"

using namespace sanity::petrinet;

Real sum(std::vector<Real> vals)
{
    auto s = 0.0;
    for (auto v : vals)
    {
        s += v;
    }
    return s;
}

TEST(petrinet, poisson_prob_trunc)
{
    {
        auto [ltp, rtp, probs] = poissonTruncPoint(20.0, 1e-6);
        ASSERT_EQ(ltp, 3);
        ASSERT_EQ(rtp, 45);
        ASSERT_NEAR(sum(probs), 1.0, 1e-6);
    }

    {
        auto [ltp, rtp, probs] = poissonTruncPoint(400.0, 1e-6);
        ASSERT_EQ(ltp, 306);
        ASSERT_EQ(rtp, 502);
        ASSERT_NEAR(sum(probs), 1.0, 1e-6);
    }
}
