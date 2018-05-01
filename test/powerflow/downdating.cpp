
#include <gtest/gtest.h>
#include "powerflow.hpp"
using namespace sanity::powerflow;

static const std::string data_base = "./test/powerflow/";
static void printPfSol(const std::vector<BusState>& sol)
{
    for (uint i = 0; i < sol.size(); i++)
    {
        std::cout << "bus " << i << ":"
                  << "P: " << sol[i].power.real()
                  << ", Q: " << sol[i].power.imag()
                  << ", |V|: " << std::abs(sol[i].voltage) << ", arg(V): "
                  << std::arg(sol[i].voltage) / 3.1415926 * 180 << " degree"
                  << std::endl;
    }
}

static void printPfSolV(const std::vector<BusState>& sol)
{
    for (uint i = 0; i < sol.size(); i++)
    {
        std::cout << "bus " << i << ":"
                  << "|V|: " << std::abs(sol[i].voltage)
                  << ", arg(V): " << std::arg(sol[i].voltage) << ", i.e.,  "
                  << std::arg(sol[i].voltage) / 3.1415926 * 180 << " degree"
                  << std::endl;
    }
}

void testDowndatingDisLineSG(const PowerGrid& old_grid, int slack_id,
                             int dis_line_idx, const PowerGrid& new_grid,
                             bool print_sol = false)
{
    Real err = 1e-6;
    int max_iter = 10;
    std::vector<BusState> old_sol;
    std::vector<BusState> new_sol;
    {
        std::cout << "solving the old system with flat start" << std::endl;
        auto sol = flatStart(old_grid, slack_id);
        auto res = solveNewton(old_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << "solution:" << std::endl;
        if (print_sol)
        {
            printPfSol(sol);
        }
        std::cout << std::endl;

        old_sol = sol;
    }
    {
        std::cout << "solving the new system with flat start" << std::endl;
        auto sol = flatStart(new_grid, slack_id);
        auto res = solveNewton(new_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << "solution:" << std::endl;
        if (print_sol)
        {
            printPfSol(sol);
        }
        std::cout << std::endl;

        new_sol = sol;
    }
    {
        std::cout << "solving the new system with old solution" << std::endl;
        auto sol = old_sol;
        auto res = solveNewton(new_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << std::endl;
    }
    {
        std::cout << "solving the new system with downdated old solution"
                  << std::endl;
        auto sol = old_sol;
        downdatingOnDisconnectedLineSG(old_grid, slack_id, dis_line_idx, sol);
        std::cout << "which is:" << std::endl;
        if (print_sol)
        {
            printPfSol(sol);
        }
        auto res = solveNewton(new_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << std::endl;
    }
}

void testDowndatingDisLineLL(const PowerGrid& old_grid, int slack_id,
                             int dis_line_idx, const PowerGrid& new_grid,
                             bool print_sol = false)
{
    Real err = 1e-6;
    int max_iter = 10;
    std::vector<BusState> old_sol;
    std::vector<BusState> new_sol;
    {
        std::cout << "solving the old system with flat start" << std::endl;
        auto sol = flatStart(old_grid, slack_id);
        auto res = solveNewton(old_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << "solution:" << std::endl;
        if (print_sol)
        {
            printPfSol(sol);
        }
        std::cout << std::endl;

        old_sol = sol;
    }
    {
        std::cout << "solving the new system with flat start" << std::endl;
        auto sol = flatStart(new_grid, slack_id);
        auto res = solveNewton(new_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << "solution:" << std::endl;
        if (print_sol)
        {
            printPfSol(sol);
        }
        std::cout << std::endl;

        new_sol = sol;
    }
    {
        std::cout << "solving the new system with old solution" << std::endl;
        auto sol = old_sol;
        auto res = solveNewton(new_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << std::endl;
    }
    {
        std::cout << "solving the new system with downdated old solution"
                  << std::endl;
        auto sol = old_sol;
        downdatingOnDisconnectedLineLL(old_grid, slack_id, dis_line_idx, sol);
        std::cout << "which is:" << std::endl;
        if (print_sol)
        {
            printPfSol(sol);
        }
        auto res = solveNewton(new_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << std::endl;
    }
}

void testDowndatingDisLineLG(const PowerGrid& old_grid, int slack_id,
                             int dis_line_idx, const PowerGrid& new_grid,
                             bool print_sol = false)
{
    Real err = 1e-6;
    int max_iter = 40;
    std::vector<BusState> old_sol;
    std::vector<BusState> new_sol;
    {
        std::cout << "solving the old system with flat start" << std::endl;
        auto sol = flatStart(old_grid, slack_id);
        auto res = solveNewton(old_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << "solution:" << std::endl;
        if (print_sol)
        {
            printPfSol(sol);
        }
        std::cout << std::endl;

        old_sol = sol;
    }
    {
        std::cout << "solving the new system with flat start" << std::endl;
        auto sol = flatStart(new_grid, slack_id);
        auto res = solveNewton(new_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << "solution:" << std::endl;
        if (print_sol)
        {
            printPfSol(sol);
        }
        std::cout << std::endl;

        new_sol = sol;
    }
    {
        std::cout << "solving the new system with old solution" << std::endl;
        auto sol = old_sol;
        auto res = solveNewton(new_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << std::endl;
    }
    {
        std::cout << "solving the new system with downdated old solution"
                  << std::endl;
        auto sol = old_sol;
        downdatingOnDisconnectedLineLG(old_grid, slack_id, dis_line_idx, sol);
        std::cout << "which is:" << std::endl;
        if (print_sol)
        {
            printPfSol(sol);
        }
        auto res = solveNewton(new_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << std::endl;
    }
}

TEST(powerflow, downdating_check1)
{
    Real err = 1e-6;
    PowerGrid old_grid;
    int dis_line_idx;
    {
        auto bus1 = old_grid.addGeneratorBus(0, 1, 0, 0);
        auto bus2 = old_grid.addGeneratorBus(0.6661, 1.05, 0, 0);
        auto bus3 = old_grid.addLoadBus(Complex(2.8653, 1.2244));
        dis_line_idx =
            old_grid.addTransmissionLine(bus1, bus2, Complex(0, 0.1), 0.01);
        old_grid.addTransmissionLine(bus1, bus3, Complex(0, 0.1), 0.01);
        old_grid.addTransmissionLine(bus2, bus3, Complex(0, 0.1), 0.01);
    }
    auto sol = flatStart(old_grid, 0);
    auto bus_map = remapBus(old_grid, 0);
    auto Y = admittanceMatrix(old_grid, bus_map);
    std::cout << "old Y:\n" << Y << std::endl;
    solveNewton(old_grid, 0, sol, sanity::linear::solveLU, 100, err);
    printPfSolV(sol);
    auto down_sol = sol;
    downdatingOnDisconnectedLineSG(old_grid, 0, dis_line_idx, down_sol);
    printPfSolV(down_sol);
    Real dB_0j = 0.01;
    PowerGrid new_grid;
    {
        auto bus1 = new_grid.addGeneratorBus(0, 1, 0, 0);
        auto bus2 = new_grid.addGeneratorBus(0.6661, 1.05, 0, 0);
        auto bus3 = new_grid.addLoadBus(Complex(2.8653, 1.2244));
        new_grid.addTransmissionLine(
            bus1, bus2, 1.0 / (1.0 / Complex(0, 0.1) - Complex(0, dB_0j)),
            0.01);
        new_grid.addTransmissionLine(bus1, bus3, Complex(0, 0.1), 0.01);
        new_grid.addTransmissionLine(bus2, bus3, Complex(0, 0.1), 0.01);
    }
    auto new_bus_map = remapBus(new_grid, 0);
    auto new_Y = admittanceMatrix(new_grid, new_bus_map);
    std::cout << "new Y:\n" << new_Y << std::endl;
    auto new_sol = flatStart(new_grid, 0);
    solveNewton(new_grid, 0, new_sol, sanity::linear::solveLU, 100, err);
    printPfSolV(new_sol);
    auto dargV1_dB0_j =
        (std::arg(new_sol[1].voltage) - std::arg(sol[1].voltage)) / dB_0j;
    auto dargV2_dB0_j =
        (std::arg(new_sol[2].voltage) - std::arg(sol[2].voltage)) / dB_0j;
    std::cout << "d_arg(V1) / dB_0j = " << std::setprecision(8)
              << dargV1_dB0_j << std::endl;
    std::cout << "d_arg(V2) / dB_0j = " << std::setprecision(8)
              << dargV2_dB0_j << std::endl;

    ASSERT_NEAR(dargV1_dB0_j, 0.00359, 0.0001);
    ASSERT_NEAR(dargV2_dB0_j, 0.00186, 0.0001);
}

TEST(powerflow, downdating_check2)
{
    Real err = 1e-6;
    PowerGrid old_grid;
    int dis_line_idx;
    {
        auto bus1 = old_grid.addGeneratorBus(0, 1, 0, 0);
        auto bus2 = old_grid.addLoadBus(Complex(3, 0.5));
        auto bus3 = old_grid.addLoadBus(Complex(2.8653, 1.2244));
        old_grid.addTransmissionLine(bus1, bus2, Complex(0.2, 0.1), 0.01);
        old_grid.addTransmissionLine(bus1, bus3, Complex(0.3, 0.1), 0.01);
        dis_line_idx =
            old_grid.addTransmissionLine(bus2, bus3, Complex(0, 0.1), 0.01);
    }
    auto sol = flatStart(old_grid, 0);
    auto bus_map = remapBus(old_grid, 0);
    auto Y = admittanceMatrix(old_grid, bus_map);
    std::cout << "old Y:\n" << Y << std::endl;
    solveNewton(old_grid, 0, sol, sanity::linear::solveLU, 100, err);
    printPfSolV(sol);
    auto down_sol = sol;
    downdatingOnDisconnectedLineLL(old_grid, 0, dis_line_idx, down_sol);
    printPfSolV(down_sol);
    Real dB_12 = 0.01;
    PowerGrid new_grid;
    {
        auto bus1 = new_grid.addGeneratorBus(0, 1, 0, 0);
        auto bus2 = new_grid.addLoadBus(Complex(3, 0.5));
        auto bus3 = new_grid.addLoadBus(Complex(2.8653, 1.2244));
        new_grid.addTransmissionLine(bus1, bus2, Complex(0.2, 0.1), 0.01);
        new_grid.addTransmissionLine(bus1, bus3, Complex(0.3, 0.1), 0.01);
        dis_line_idx = new_grid.addTransmissionLine(
            bus2, bus3, 1.0 / (1.0 / Complex(0, 0.1) - Complex(0, dB_12)),
            0.01);
    }
    auto new_bus_map = remapBus(new_grid, 0);
    auto new_Y = admittanceMatrix(new_grid, new_bus_map);
    std::cout << "new Y:\n" << new_Y << std::endl;
    auto new_sol = flatStart(new_grid, 0);
    solveNewton(new_grid, 0, new_sol, sanity::linear::solveLU, 100, err);
    printPfSolV(new_sol);
    auto dargV1_dB0_j =
        (std::arg(new_sol[1].voltage) - std::arg(sol[1].voltage)) / dB_12;
    auto dargV2_dB0_j =
        (std::arg(new_sol[2].voltage) - std::arg(sol[2].voltage)) / dB_12;
    std::cout << "d_arg(V1) / dB_0j = " << std::setprecision(8)
              << dargV1_dB0_j << std::endl;
    std::cout << "d_arg(V2) / dB_0j = " << std::setprecision(8)
              << dargV2_dB0_j << std::endl;
}

TEST(powerflow, __downdating_dis_line_SG_exampleT7_4)
{
    PowerGrid old_grid;
    int dis_line_idx;
    {
        auto bus1 = old_grid.addGeneratorBus(0, 1, 0, 0);
        auto bus2 = old_grid.addGeneratorBus(0.6661, 1.05, 0, 0);
        auto bus3 = old_grid.addLoadBus(Complex(2.8653, 1.2244));
        dis_line_idx =
            old_grid.addTransmissionLine(bus1, bus2, Complex(0, 0.1), 0.01);
        old_grid.addTransmissionLine(bus1, bus3, Complex(0, 0.1), 0.01);
        old_grid.addTransmissionLine(bus2, bus3, Complex(0, 0.1), 0.01);
    }
    PowerGrid new_grid;
    {
        auto bus1 = new_grid.addGeneratorBus(0, 1, 0, 0);
        auto bus2 = new_grid.addGeneratorBus(0.6661, 1.05, 0, 0);
        auto bus3 = new_grid.addLoadBus(Complex(2.8653, 1.2244));
        new_grid.addTransmissionLine(bus1, bus3, Complex(0, 0.1), 0.01);
        new_grid.addTransmissionLine(bus2, bus3, Complex(0, 0.1), 0.01);
    }
    testDowndatingDisLineSG(old_grid, 0, dis_line_idx, new_grid);
}

TEST(powerflow, __downdating_dis_line_LL_ieee14)
{
    auto ieee_model =
        readIeeeCdfModel(data_base + "ieee_cdf_models/ieee14cdf.txt");
    auto old_grid = ieeeCdf2Grid(ieee_model);

    // remove branch 6, which connects 3 to 4
    ieee_model.branches.erase(ieee_model.branches.begin() + 6);

    auto new_grid = ieeeCdf2Grid(ieee_model);

    assert(new_grid.grid.lineCount() + 1 == old_grid.grid.lineCount());

    int dis_line_idx = 6;
    assert(old_grid.grid.getLine(dis_line_idx).startBus == 3);
    assert(old_grid.grid.getLine(dis_line_idx).endBus == 4);
    assert(old_grid.slack == 0);
    assert(new_grid.slack == 0);

    testDowndatingDisLineLL(old_grid.grid, old_grid.slack, dis_line_idx,
                            new_grid.grid, true);
}

TEST(powerflow, __downdating_dis_line_SG_ieee14)
{
    auto ieee_model =
        readIeeeCdfModel(data_base + "ieee_cdf_models/ieee14cdf.txt");
    auto old_grid = ieeeCdf2Grid(ieee_model);

    // remove branch 1, which connects 1 (slack) to 2
    ieee_model.branches.erase(ieee_model.branches.begin());

    auto new_grid = ieeeCdf2Grid(ieee_model);

    assert(new_grid.grid.lineCount() + 1 == old_grid.grid.lineCount());

    int dis_line_idx = 0;
    assert(old_grid.grid.getLine(dis_line_idx).startBus == 0);
    assert(old_grid.grid.getLine(dis_line_idx).endBus == 1);
    assert(old_grid.slack == 0);
    assert(new_grid.slack == 0);

    testDowndatingDisLineSG(old_grid.grid, old_grid.slack, dis_line_idx,
                            new_grid.grid);
}

TEST(powerflow, __downdating_dis_line_SG_ieee30)
{
    auto ieee_model =
        readIeeeCdfModel(data_base + "ieee_cdf_models/ieee30cdf.txt");
    auto old_grid = ieeeCdf2Grid(ieee_model);

    // remove branch 1, which connects 1 (slack) to 2
    int dis_line_idx = 0;
    ieee_model.branches.erase(ieee_model.branches.begin() + dis_line_idx);

    auto new_grid = ieeeCdf2Grid(ieee_model);

    assert(new_grid.grid.lineCount() + 1 == old_grid.grid.lineCount());

    assert(old_grid.grid.getLine(dis_line_idx).startBus == 0);
    assert(old_grid.grid.getLine(dis_line_idx).endBus == 1);
    assert(old_grid.slack == 0);
    assert(new_grid.slack == 0);

    testDowndatingDisLineSG(old_grid.grid, old_grid.slack, dis_line_idx,
                            new_grid.grid);
}

// 48, 69, 76
TEST(powerflow, __downdating_dis_line_SG_ieee118_1)
{
    auto ieee_model =
        readIeeeCdfModel(data_base + "ieee_cdf_models/ieee118cdf.txt");
    auto old_grid = ieeeCdf2Grid(ieee_model);

    // remove branch 105, which connects 68 (slack) to 48
    int slack = 68;
    int gbus = 48;
    int dis_line_idx = 105;
    ieee_model.branches.erase(ieee_model.branches.begin() + dis_line_idx);

    auto new_grid = ieeeCdf2Grid(ieee_model);

    assert(new_grid.grid.lineCount() + 1 == old_grid.grid.lineCount());

    assert(old_grid.grid.getLine(dis_line_idx).startBus == gbus);
    assert(old_grid.grid.getLine(dis_line_idx).endBus == slack);
    assert(old_grid.slack == slack);
    assert(new_grid.slack == slack);

    testDowndatingDisLineSG(old_grid.grid, old_grid.slack, dis_line_idx,
                            new_grid.grid);
}

// 48, 69, 76
TEST(powerflow, __downdating_dis_line_SG_ieee118_2)
{
    auto ieee_model =
        readIeeeCdfModel(data_base + "ieee_cdf_models/ieee118cdf.txt");
    auto old_grid = ieeeCdf2Grid(ieee_model);

    // remove branch 105, which connects 68 (slack) to 48
    int slack = 68;
    int gbus = 69;
    int dis_line_idx = 107;
    ieee_model.branches.erase(ieee_model.branches.begin() + dis_line_idx);

    auto new_grid = ieeeCdf2Grid(ieee_model);

    assert(new_grid.grid.lineCount() + 1 == old_grid.grid.lineCount());

    assert(old_grid.grid.getLine(dis_line_idx).startBus == slack);
    assert(old_grid.grid.getLine(dis_line_idx).endBus == gbus);
    assert(old_grid.slack == slack);
    assert(new_grid.slack == slack);

    testDowndatingDisLineSG(old_grid.grid, old_grid.slack, dis_line_idx,
                            new_grid.grid);
}

// 48, 69, 76
TEST(powerflow, __downdating_dis_line_SG_ieee118_3)
{
    auto ieee_model =
        readIeeeCdfModel(data_base + "ieee_cdf_models/ieee118cdf.txt");
    auto old_grid = ieeeCdf2Grid(ieee_model);

    // remove branch 105, which connects 68 (slack) to 48
    int slack = 68;
    int gbus = 76;
    int dis_line_idx = 118;
    ieee_model.branches.erase(ieee_model.branches.begin() + dis_line_idx);

    auto new_grid = ieeeCdf2Grid(ieee_model);

    assert(new_grid.grid.lineCount() + 1 == old_grid.grid.lineCount());

    assert(old_grid.grid.getLine(dis_line_idx).startBus == slack);
    assert(old_grid.grid.getLine(dis_line_idx).endBus == gbus);
    assert(old_grid.slack == slack);
    assert(new_grid.slack == slack);

    testDowndatingDisLineSG(old_grid.grid, old_grid.slack, dis_line_idx,
                            new_grid.grid);
}

TEST(powerflow, __downdating_dis_line_SG_GBreduced1)
{
    auto ext_model = readMatpowerModel(
        data_base + "./matpower_models/GBreducednetwrok.txt");
    auto old_grid = matpower2Grid(ext_model);

    // remove branch 105, which connects 26 (slack) to 48
    int slack = 26;
    int gbus = 25;
    int dis_line_idx = 90;
    ext_model.branches.erase(ext_model.branches.begin() + dis_line_idx);

    auto new_grid = matpower2Grid(ext_model);

    assert(new_grid.grid.lineCount() + 1 == old_grid.grid.lineCount());

    assert(old_grid.grid.getLine(dis_line_idx).startBus == slack);
    assert(old_grid.grid.getLine(dis_line_idx).endBus == gbus);
    assert(old_grid.slack == slack);
    assert(new_grid.slack == slack);

    testDowndatingDisLineSG(old_grid.grid, old_grid.slack, dis_line_idx,
                            new_grid.grid);
}

TEST(powerflow, __downdating_dis_line_SG_GBreduced2)
{
    auto ext_model = readMatpowerModel(
        data_base + "./matpower_models/GBreducednetwrok.txt");
    auto old_grid = matpower2Grid(ext_model);

    // remove branch 105, which connects 26 (slack) to 48
    int slack = 26;
    int gbus = 27;
    int dis_line_idx = 92;
    ext_model.branches.erase(ext_model.branches.begin() + dis_line_idx);

    auto new_grid = matpower2Grid(ext_model);

    assert(new_grid.grid.lineCount() + 1 == old_grid.grid.lineCount());

    assert(old_grid.grid.getLine(dis_line_idx).startBus == gbus);
    assert(old_grid.grid.getLine(dis_line_idx).endBus == slack);
    assert(old_grid.slack == slack);
    assert(new_grid.slack == slack);

    testDowndatingDisLineSG(old_grid.grid, old_grid.slack, dis_line_idx,
                            new_grid.grid);
}

void testLineFailure(const PowerGrid& old_grid, int slack_id,
                     const PowerGrid& new_grid, bool print_sol = false)
{
    Real err = 1e-6;
    int max_iter = 10;
    std::vector<BusState> old_sol;
    std::vector<BusState> new_sol;
    {
        std::cout << "solving the old system with flat start" << std::endl;
        auto sol = flatStart(old_grid, slack_id);
        auto res = solveNewton(old_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << "solution:" << std::endl;
        if (print_sol)
        {
            printPfSol(sol);
        }
        std::cout << std::endl;

        old_sol = sol;
        ASSERT_LT(res.nIter, max_iter);
    }
    {
        std::cout << "solving the new system with flat start" << std::endl;
        auto sol = flatStart(new_grid, slack_id);
        auto res = solveNewton(new_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << "solution:" << std::endl;
        if (print_sol)
        {
            printPfSol(sol);
        }
        std::cout << std::endl;

        new_sol = sol;
    }
    {
        std::cout << "solving the new system with old solution" << std::endl;
        auto sol = old_sol;
        auto res = solveNewton(new_grid, slack_id, sol,
                               sanity::linear::solveLU, max_iter, err);
        std::cout << "error: " << res.error << ", iter:" << res.nIter
                  << std::endl;
        std::cout << std::endl;
        ASSERT_LT(res.nIter, max_iter);
    }
}

TEST(powerflow, __linefailure_ieee14)
{
    auto old_model =
        readIeeeCdfModel(data_base + "ieee_cdf_models/ieee14cdf.txt");
    auto old_grid = ieeeCdf2Grid(old_model);
    for (uint i = 0; i < old_model.branches.size(); i++)
    {
        std::cout << "removing line:" << i << std::endl;
        if (i == 13)
        {
            continue;
        }
        auto new_model =
            readIeeeCdfModel(data_base + "ieee_cdf_models/ieee14cdf.txt");
        new_model.branches.erase(new_model.branches.begin() + i);
        auto new_grid = ieeeCdf2Grid(new_model);
        testLineFailure(old_grid.grid, old_grid.slack, new_grid.grid);
    }
}

TEST(powerflow, __linefailure_ieee118)
{
    auto old_model =
        readIeeeCdfModel(data_base + "ieee_cdf_models/ieee118cdf.txt");
    auto old_grid = ieeeCdf2Grid(old_model);

    // line 132 diverge
    for (uint i = 0; i < old_model.branches.size(); i++)
    {
        std::cout << "removing line:" << i << std::endl;
        if (i == 6 || i == 8 || i == 112 || i == 133 || i == 132 ||
            i == 175 || i == 176 || i == 182 || i == 183)
        {
            continue;
        }
        auto new_model = old_model;
        new_model.branches.erase(new_model.branches.begin() + i);
        auto new_grid = ieeeCdf2Grid(new_model);
        testLineFailure(old_grid.grid, old_grid.slack, new_grid.grid);
    }
}

TEST(powerflow, __linefailure_GBreduced)
{
    auto old_model =
        readMatpowerModel(data_base + "matpower_models/GBreducednetwrok.txt");
    auto old_grid = matpower2Grid(old_model);

    // line 132 diverge
    for (uint i = 0; i < old_model.branches.size(); i++)
    {
        std::cout << "removing line:" << i << std::endl;
        if (false)
        {
            continue;
        }
        auto new_model = old_model;
        new_model.branches.erase(new_model.branches.begin() + i);
        auto new_grid = matpower2Grid(new_model);
        testLineFailure(old_grid.grid, old_grid.slack, new_grid.grid);
    }
}

// line 12 (GL) diverge
// line 55 (GL) diverge
// line 75 (LL) diverge
TEST(powerflow, __linefailure_case1354)
{
    auto old_model =
        readMatpowerModel(data_base + "matpower_models/case1354.txt");
    auto old_grid = matpower2Grid(old_model);

    // line 132 diverge
    for (uint i = 73; i < old_model.branches.size(); i++)
    {
        std::cout << "------------------------------------------"
                  << std::endl;
        std::cout << "removing line:" << i << std::endl;
        auto new_model = old_model;
        new_model.branches.erase(new_model.branches.begin() + i);
        auto new_grid = matpower2Grid(new_model);
        testLineFailure(old_grid.grid, old_grid.slack, new_grid.grid);
    }
}

TEST(powerflow, __downdating_dis_line_LL_case1354)
{
    auto ext_model =
        readMatpowerModel(data_base + "./matpower_models/case1354.txt");
    auto old_grid = matpower2Grid(ext_model);

    // remove branch 75
    int dis_line_idx = 75;
    ext_model.branches.erase(ext_model.branches.begin() + dis_line_idx);

    auto new_grid = matpower2Grid(ext_model);

    assert(new_grid.grid.lineCount() + 1 == old_grid.grid.lineCount());

    testDowndatingDisLineLL(old_grid.grid, old_grid.slack, dis_line_idx,
                            new_grid.grid);
}

TEST(powerflow, __downdating_dis_line_LG_case1354)
{
    auto ext_model =
        readMatpowerModel(data_base + "./matpower_models/case1354.txt");
    auto old_grid = matpower2Grid(ext_model);

    // remove branch 55
    int dis_line_idx = 55;
    ext_model.branches.erase(ext_model.branches.begin() + dis_line_idx);

    auto new_grid = matpower2Grid(ext_model);

    assert(new_grid.grid.lineCount() + 1 == old_grid.grid.lineCount());

    testDowndatingDisLineLG(old_grid.grid, old_grid.slack, dis_line_idx,
                            new_grid.grid);
}
