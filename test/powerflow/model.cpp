#include <gtest/gtest.h>
#include "powerflow.hpp"
using namespace sanity::powerflow;

TEST(powerflow, read_ieee_model)
{
    auto ieee_model =
        readIeeeCdfModel("./test/powerflow/ieee_cdf_models/ieee14cdf.txt");
    auto model = ieeeCdf2Grid(ieee_model);
    ASSERT_EQ(model.grid.busCount(), 14);
    ASSERT_EQ(model.grid.lineCount(), 20);
    ASSERT_EQ(model.grid.generatorCount(), 5);
    ASSERT_EQ(model.grid.loadCount(), 9);
    ASSERT_EQ(model.slack, 0);
}
