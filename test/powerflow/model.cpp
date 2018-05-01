#include <gtest/gtest.h>
#include "powerflow.hpp"
using namespace sanity::powerflow;

static const std::string data_base = "./test/powerflow/";
TEST(powerflow, read_ieee17)
{
    auto ieee_model =
        readIeeeCdfModel(data_base + "ieee_cdf_models/ieee14cdf.txt");
    auto model = ieeeCdf2Grid(ieee_model);
    ASSERT_EQ(model.grid.busCount(), 14);
    ASSERT_EQ(model.grid.lineCount(), 20);
    ASSERT_EQ(model.grid.generatorCount(), 5);
    ASSERT_EQ(model.grid.loadCount(), 9);
    ASSERT_EQ(model.slack, 0);
}

TEST(powerflow, read_ieee30)
{
    auto ieee_model =
        readIeeeCdfModel(data_base + "ieee_cdf_models/ieee30cdf.txt");
    auto model = ieeeCdf2Grid(ieee_model);

    ASSERT_EQ(model.grid.busCount(), 30);
    ASSERT_EQ(model.grid.lineCount(), 41);
    ASSERT_EQ(model.grid.generatorCount(), 6);
    ASSERT_EQ(model.grid.loadCount(), 30 - 6);
    ASSERT_EQ(model.slack, 0);
}

TEST(powerflow, read_ieee57)
{
    auto ieee_model =
        readIeeeCdfModel(data_base + "ieee_cdf_models/ieee57cdf.txt");
    auto model = ieeeCdf2Grid(ieee_model);

    ASSERT_EQ(model.grid.busCount(), 57);
    ASSERT_EQ(model.grid.lineCount(), 80);
    ASSERT_EQ(model.grid.generatorCount(), 7);
    ASSERT_EQ(model.grid.loadCount(), 57 - 7);
    ASSERT_EQ(model.slack, 0);
}

TEST(powerflow, read_ieee118)
{
    auto ieee_model =
        readIeeeCdfModel(data_base + "ieee_cdf_models/ieee118cdf.txt");
    auto model = ieeeCdf2Grid(ieee_model);

    ASSERT_EQ(model.grid.busCount(), 118);
    ASSERT_EQ(model.grid.lineCount(), 186);
    ASSERT_EQ(model.grid.generatorCount(), 54);
    ASSERT_EQ(model.grid.loadCount(), 118 - 54);
    ASSERT_EQ(model.slack, 68);
}

// unsupported line type 2
// TEST(powerflow, read_ieee300)
// {
//     auto ieee_model =
//         readIeeeCdfModel(data_base + "ieee_cdf_models/ieee300cdf.txt");
//     auto model = ieeeCdf2Grid(ieee_model);
// }

TEST(powerflow, read_matpower1354)
{
    auto matpower_model =
        readMatpowerModel(data_base + "matpower_models/case1354.txt");
    ASSERT_EQ(matpower_model.base_MVA, 100);
    ASSERT_EQ(matpower_model.buses.size(), 1354);
    ASSERT_EQ(matpower_model.generators.size(), 260);
    ASSERT_EQ(matpower_model.branches.size(), 1991);
    auto grid_model = matpower2Grid(matpower_model);
    ASSERT_EQ(grid_model.grid.busCount(), 1354);
    ASSERT_EQ(grid_model.grid.lineCount(), 1991);
}
