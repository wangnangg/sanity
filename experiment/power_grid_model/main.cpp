#include <gtest/gtest.h>
#include <stdexcept>
#include "linear.hpp"
#include "model.hpp"
#include "petrinet.hpp"
#include "powerflow.hpp"
#include "splinear.hpp"
#include "timer.hpp"

using namespace sanity::petrinet;
using namespace sanity::linear;
using namespace sanity::splinear;
using namespace sanity::powerflow;

static Marking last_marking;
static std::vector<bool> load_connected;
static ExpPowerFlowModel exp_model;
static uint nbus;
static uint nload;
static uint ngen;
static uint nline;
static uint nBusFailure;
static uint nLoadFailure;
static uint nGenFailure;
static uint nLineFailure;
static std::vector<uint>
    marking_map;  // map marking to the corresponding bus/line idx

uint busStartMk() { return 0; }
uint busEndMk() { return nbus * 2; }
uint loadStartMk() { return nbus * 2; }
uint loadEndMk() { return nbus * 2 + nload * 3; }
uint genStartMk() { return nbus * 2 + nload * 3; }
uint genEndMk() { return nbus * 2 + nload * 3 + ngen * 2; }
uint lineStartMk() { return nbus * 2 + nload * 3 + ngen * 2; }
uint lineEndMk() { return nbus * 2 + nload * 3 + ngen * 2 + nline * 2; }

void syncModel(ExpPowerFlowModel& model, const Marking* mk)
{
    assert(mk->size() == 2 * nbus + 3 * nload + 2 * ngen + 2 * nline);
    assert(nbus == model.buses.size());
    assert(nline == model.lines.size());

    // bus
    nBusFailure = 0;
    for (uint pid = busStartMk(); pid < busEndMk(); pid += 2)
    {
        if (mk->nToken(pid + 1) == 1)
        {
            model.buses[marking_map[pid]].busOk = false;
            nBusFailure += 1;
        }
        else
        {
            model.buses[marking_map[pid]].busOk = true;
        }
    }

    // load
    nLoadFailure = 0;
    for (uint pid = loadStartMk(); pid < loadEndMk(); pid += 3)
    {
        if (mk->nToken(pid) == 1)
        {
            model.buses[marking_map[pid]].loadOk = true;
            model.buses[marking_map[pid]].loadConnected = true;
        }
        else if (mk->nToken(pid + 1) == 1)
        {
            model.buses[marking_map[pid]].loadOk = false;
            model.buses[marking_map[pid]].loadConnected = false;
            nLoadFailure += 1;
        }
        else
        {
            model.buses[marking_map[pid]].loadOk = true;
            model.buses[marking_map[pid]].loadConnected = false;
        }
    }

    // gen
    nGenFailure = 0;
    for (uint pid = genStartMk(); pid < genEndMk(); pid += 2)
    {
        if (mk->nToken(pid + 1) == 1)
        {
            model.buses[marking_map[pid]].generatorOk = false;
            nGenFailure += 1;
        }
        else
        {
            model.buses[marking_map[pid]].generatorOk = true;
        }
    }

    // line
    nLineFailure = 0;
    for (uint pid = lineStartMk(); pid < lineEndMk(); pid += 2)
    {
        if (mk->nToken(pid + 1) == 1)
        {
            model.lines[marking_map[pid]].ok = false;
            nLineFailure += 1;
        }
        else
        {
            model.lines[marking_map[pid]].ok = true;
        }
    }
}

bool balance(ExpPowerFlowModel& model)
{
    Real load = 0.0;
    Real gen = 0.0;
    for (uint i = 0; i < model.buses.size(); i++)
    {
        if (model.buses[i].busOk)
        {
            if (model.buses[i].load > 0 && model.buses[i].loadConnected &&
                model.buses[i].loadOk)
            {
                load += model.buses[i].load;
            }
        }
    }
    for (uint i = 0; i < model.buses.size(); i++)
    {
        if (model.buses[i].busOk)
        {
            if (model.buses[i].generation > 0 && model.buses[i].generatorOk)
            {
                gen += model.buses[i].generation;
            }
        }
    }
    if (gen == 0)
    {
        if (load == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    Real ratio = load / gen;
#ifndef NDEBUG
    std::cout << "balance factor: " << ratio << std::endl;
#endif
    if (ratio > 1)
    {
        return false;
    }
    for (uint i = 0; i < model.buses.size(); i++)
    {
        if (model.buses[i].busOk)
        {
            if (model.buses[i].generation > 0 && model.buses[i].generatorOk)
            {
                model.buses[i].genRatio = ratio;
            }
        }
    }
    return true;
}

bool validate(ExpPowerFlowModel& model)
{
    if (!balance(model))
    {
        return false;
    }
    auto converted = exp2DCModel(exp_model);
    auto dc_res = solveDC(converted.dcModel, solveLU);
    for (bool s : dc_res.islandSolved)
    {
        if (!s)
        {
            return false;
        }
    }
    // check over loaded line
    for (const auto& line : exp_model.lines)
    {
        int dc_line_idx = converted.exp2dcLine.forward(line.idx);
        if (dc_line_idx >= 0)
        {
            Real line_power = dc_res.lineRealPowers[(uint)dc_line_idx];
#ifndef NDEBUG
            std::cout << "line " << line.idx << " power: " << line_power
                      << "/" << line.maxPower << std::endl;
#endif
            if (line_power > line.maxPower)
            {
#ifndef NDEBUG
                std::cout << "overloaded." << std::endl;
#endif
                return false;
            }
        }
        else
        {
#ifndef NDEBUG
            std::cout << "line " << line.idx << " disconnected." << std::endl;
#endif
        }
    }
    return true;
}

void shedload(ExpPowerFlowModel& model)
{
    // optimistic check
    for (auto& bus : model.buses)
    {
        if (bus.busOk && bus.load > 0 && bus.loadOk)
        {
            bus.loadConnected = true;
        }
        else
        {
            bus.loadConnected = false;
        }
    }

#ifndef NDEBUG
    std::cout << "enable all loads" << std::endl;
#endif
    if (validate(model))
    {
#ifndef NDEBUG
        std::cout << "...ok" << std::endl;
#endif
        return;
    }

    // disable all
    for (auto& bus : model.buses)
    {
        if (bus.busOk && bus.load > 0 && bus.loadOk)
        {
            bus.loadConnected = false;
        }
    }

#ifndef NDEBUG
    std::cout << "disable all loads" << std::endl;
#endif
    // this should be okay
    assert(validate(model));

#ifndef NDEBUG
    std::cout << "...ok" << std::endl;
#endif

    // enable one by one;
    for (auto& bus : model.buses)
    {
        if (bus.busOk && bus.load > 0 && bus.loadOk)
        {
#ifndef NDEBUG
            std::cout << "enable load on bus " << bus.idx << std::endl;
#endif
            bus.loadConnected = true;
            if (!validate(model))
            {
#ifndef NDEBUG
                std::cout << "...fail" << std::endl;
#endif
                bus.loadConnected = false;
            }

#ifndef NDEBUG
            std::cout << "...ok" << std::endl;
#endif
        }
    }
}

bool markingEqual(const Marking& m1, const Marking& m2)
{
    if (m1.size() != m2.size())
    {
        return false;
    }
    for (uint i = 0; i < m1.size(); i++)
    {
        if (m1.nToken(i) != m2.nToken(i))
        {
            return false;
        }
    }
    return true;
}

void powerflowUpdate(const Marking* mk)
{
    if (markingEqual(last_marking, *mk))
    {
        return;
    }
    else
    {
        syncModel(exp_model, mk);
        shedload(exp_model);
        for (uint i = 0; i < nload; i++)
        {
            uint pid = i * 3 + loadStartMk();
            load_connected[i] =
                exp_model.buses[marking_map[pid]].loadConnected;
        }
        last_marking = mk->clone();
        return;
    }
}

void createLoad(SrnCreator& ct, Real failure_rate, Real repair_rate, uint idx,
                MarkingDepBool trunc)
{
    auto up = ct.place();
    auto down = ct.place();
    auto disc = ct.place();
    ct.expTrans(failure_rate).iarc(up).oarc(down).enable(trunc);
    ct.expTrans(repair_rate).iarc(down).oarc(up);
    auto disc_enable = [=](PetriNetState state) {
        powerflowUpdate(state.marking);
        return !load_connected[idx];
    };
    ct.immTrans(1).iarc(up).oarc(disc).enable(disc_enable);
    auto recon_enable = [=](PetriNetState state) {
        powerflowUpdate(state.marking);
        return load_connected[idx];
    };
    ct.immTrans(1).iarc(disc).oarc(up).enable(recon_enable);
}

void createTwoStates(SrnCreator& ct, Real failure_rate, Real repair_rate,
                     MarkingDepBool trunc)
{
    auto up = ct.place();
    auto down = ct.place();
    ct.expTrans(failure_rate).iarc(up).oarc(down).enable(trunc);
    ct.expTrans(repair_rate).iarc(down).oarc(up);
}

void printPfSol(const DCPowerFlowModel& model, const DCPowerFlowResult sol)
{
    for (uint is = 0; is < sol.islands.size(); is++)
    {
        std::cout << "island " << is << ":" << std::endl;
        for (uint i = 0; i < sol.islands[is].size(); i++)
        {
            const auto& bus_idx = sol.islands[is][i];
            std::cout << "bus " << bus_idx << ", angle: "
                      << sol.busVoltageAngles[bus_idx] * 180 / 3.1415926
                      << " degrees" << std::endl;
        }
    }
    for (uint i = 0; i < sol.lineRealPowers.size(); i++)
    {
        std::cout << "power on line from " << model.getLine(i).startBus
                  << " to " << model.getLine(i).endBus << ": "
                  << sol.lineRealPowers[i] << std::endl;
    }
}

uint totalFailure()
{
    return nBusFailure + nLoadFailure + nGenFailure + nLineFailure;
}

StochasticRewardNet exp2srn_flat(const ExpPowerFlowModel& model,
                                 Real bus_fail, Real bus_repair,
                                 Real load_fail, Real load_repair,
                                 Real gen_fail, Real gen_repair,
                                 Real line_fail, Real line_repair,
                                 uint nTrunc)
{
    SrnCreator ct;

    marking_map = std::vector<uint>();
    // create bus
    nbus = 0;
    for (uint i = 0; i < model.buses.size(); i++)
    {
        createTwoStates(ct, bus_fail, bus_repair, [=](auto state) {
            powerflowUpdate(state.marking);
            if (totalFailure() < nTrunc)
            {
                return true;
            }
            else
            {
                return false;
            }
        });
        nbus += 1;
        marking_map.push_back(i);
        marking_map.push_back(i);
    }

    // create load
    nload = 0;
    for (uint i = 0; i < model.buses.size(); i++)
    {
        if (model.buses[i].load > 0)
        {
            createLoad(ct, load_fail, load_repair, nload, [=](auto state) {
                powerflowUpdate(state.marking);
                if (totalFailure() < nTrunc)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            });
            nload += 1;
            marking_map.push_back(i);
            marking_map.push_back(i);
            marking_map.push_back(i);
        }
    }
    load_connected = std::vector<bool>(nload, true);

    // create gen
    ngen = 0;
    for (uint i = 0; i < model.buses.size(); i++)
    {
        if (model.buses[i].generation > 0)
        {
            createTwoStates(ct, gen_fail, gen_repair, [=](auto state) {
                powerflowUpdate(state.marking);
                if (totalFailure() < nTrunc)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            });
            ngen += 1;
            marking_map.push_back(i);
            marking_map.push_back(i);
        }
    }

    // create line
    nline = 0;
    for (uint i = 0; i < model.lines.size(); i++)
    {
        createTwoStates(ct, line_fail, line_repair, [=](auto state) {
            powerflowUpdate(state.marking);
            if (totalFailure() < nTrunc)
            {
                return true;
            }
            else
            {
                return false;
            }
        });
        nline += 1;
        marking_map.push_back(i);
        marking_map.push_back(i);
    }

    return ct.create();
}

StochasticRewardNet exp2srn_diff(const ExpPowerFlowModel& model,
                                 Real bus_fail, Real bus_repair,
                                 Real load_fail, Real load_repair,
                                 Real gen_fail, Real gen_repair,
                                 Real line_fail, Real line_repair,
                                 uint busTrunc, uint loadTrunc, uint genTrunc,
                                 uint lineTrunc)
{
    SrnCreator ct;

    marking_map = std::vector<uint>();
    // create bus
    nbus = 0;
    for (uint i = 0; i < model.buses.size(); i++)
    {
        createTwoStates(ct, bus_fail, bus_repair, [=](auto state) {
            powerflowUpdate(state.marking);
            if (nBusFailure < busTrunc)
            {
                return true;
            }
            else
            {
                return false;
            }
        });
        nbus += 1;
        marking_map.push_back(i);
        marking_map.push_back(i);
    }

    // create load
    nload = 0;
    for (uint i = 0; i < model.buses.size(); i++)
    {
        if (model.buses[i].load > 0)
        {
            createLoad(ct, load_fail, load_repair, nload, [=](auto state) {
                powerflowUpdate(state.marking);
                if (nLoadFailure < loadTrunc)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            });
            nload += 1;
            marking_map.push_back(i);
            marking_map.push_back(i);
            marking_map.push_back(i);
        }
    }
    load_connected = std::vector<bool>(nload, true);

    // create gen
    ngen = 0;
    for (uint i = 0; i < model.buses.size(); i++)
    {
        if (model.buses[i].generation > 0)
        {
            createTwoStates(ct, gen_fail, gen_repair, [=](auto state) {
                powerflowUpdate(state.marking);
                if (nGenFailure < genTrunc)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            });
            ngen += 1;
            marking_map.push_back(i);
            marking_map.push_back(i);
        }
    }

    // create line
    nline = 0;
    for (uint i = 0; i < model.lines.size(); i++)
    {
        createTwoStates(ct, line_fail, line_repair, [=](auto state) {
            powerflowUpdate(state.marking);
            if (nLineFailure < lineTrunc)
            {
                return true;
            }
            else
            {
                return false;
            }
        });
        nline += 1;
        marking_map.push_back(i);
        marking_map.push_back(i);
    }

    return ct.create();
}

Marking createInitMarking(const StochasticRewardNet& srn)
{
    Marking mk(nbus * 2 + nload * 3 + ngen * 2 + nline * 2, 0);
    assert(srn.pnet.placeCount() == mk.size());
    for (uint pid = busStartMk(); pid < busEndMk(); pid += 2)
    {
        mk.setToken(pid, 1);
    }
    for (uint pid = loadStartMk(); pid < loadEndMk(); pid += 3)
    {
        mk.setToken(pid, 1);
    }
    for (uint pid = genStartMk(); pid < genEndMk(); pid += 2)
    {
        mk.setToken(pid, 1);
    }
    for (uint pid = lineStartMk(); pid < lineEndMk(); pid += 2)
    {
        mk.setToken(pid, 1);
    }
    return mk;
}

static void printSrnSol1(const std::vector<Marking>& markings,
                         const Permutation& mat2node, uint ntan,
                         VectorConstView solution)
{
    std::cout << "tangibles (" << ntan << "):" << std::endl;
    for (uint i = 0; i < ntan; i++)
    {
        const auto& mk = markings[(uint)mat2node.forward(i)];
        std::cout << "( ";
        for (uint j = 0; j < mk.size(); j++)
        {
            std::cout << mk.nToken(j) << ' ';
        }
        std::cout << ")";
        std::cout << " tau:" << solution(i) << std::endl;
    }
    std::cout << "absorbing groups (" << markings.size() - ntan
              << "):" << std::endl;
    for (uint i = ntan; i < markings.size(); i++)
    {
        const auto& mk = markings[(uint)mat2node.forward(i)];
        std::cout << "( ";
        for (uint j = 0; j < mk.size(); j++)
        {
            std::cout << mk.nToken(j) << ' ';
        }
        std::cout << ") ";
        std::cout << " prob:" << solution(i) << std::endl;
    }
}

static void printSrnSol2(const std::vector<Marking>& markings,
                         const Permutation& mat2node, uint ntan,
                         VectorConstView solution)
{
    std::cout << "# markings: " << markings.size()
              << ", # tangibles: " << ntan << std::endl;
    for (uint i = 0; i < markings.size(); i++)
    {
        const auto& mk = markings[i];
        std::cout << "( ";
        for (uint j = 0; j < mk.size(); j++)
        {
            std::cout << mk.nToken(j) << ' ';
        }
        std::cout << ")";
        uint mat_id = (uint)mat2node.backward(i);
        if (mat_id < ntan)
        {
            std::cout << " tau:" << solution(mat_id) << std::endl;
        }
        else
        {
            std::cout << " prob:" << solution(mat_id) << std::endl;
        }
    }
}

Real servedLoad(ExpPowerFlowModel& model, const Marking& mk)
{
    Real load = 0.0;
    syncModel(model, &mk);
    for (const auto& bus : model.buses)
    {
        if (bus.busOk && bus.loadOk && bus.loadConnected)
        {
            load += bus.load;
        }
    }
    return load;
}

TEST(power_grid_model, three_nodes)
{
    Real bus_fail = 0.0001;
    Real bus_repair = 0.01;

    Real load_fail = 0.001;
    Real load_repair = 1;

    Real gen_fail = 0.001;
    Real gen_repair = 0.1;

    Real line_fail = 0.005;
    Real line_repair = 0.5;

    auto b1 = exp_model.addBus(ExpBusType::PV, 0, 1.0);
    auto b2 = exp_model.addBus(ExpBusType::PV, 10 / 100.0, 63 / 100.0);
    auto b3 = exp_model.addBus(ExpBusType::PQ, 90 / 100.0, 0);
    exp_model.addLine(b1, b2, 0.0576, 250.0 / 100.0);
    exp_model.addLine(b2, b3, 0.092, 250.0 / 100.0);
    exp_model.addLine(b1, b3, 0.17, 150.0 / 100.0);

    for (uint f = 0; f <= 10; f++)
    {
        auto srn = exp2srn_flat(exp_model, bus_fail, bus_repair, load_fail,
                                load_repair, gen_fail, gen_repair, line_fail,
                                line_repair, f);
        auto init_mk = createInitMarking(srn);

        auto rg = genReducedReachGraph(srn, init_mk, 1e-6, 100);

        uint max_iter = 1000;
        Real tol = 1e-6;
        Real w = 0.8;
        auto sol = srnSteadyStateDecomp(
            rg.graph, rg.edgeRates, rg.initProbs,
            [=](const Spmatrix& A, VectorMutableView x, VectorConstView b) {
                auto res = solveSor(A, x, b, w, tol, max_iter);
                if (res.error > tol || std::isnan(res.error))
                {
                    std::cout << "Sor. nIter: " << res.nIter;
                    std::cout << ", error: " << res.error << std::endl;
                    throw std::invalid_argument("Sor failed to converge.");
                }
            });
        // printSrnSol2(rg.nodeMarkings, sol.matrix2node, sol.nTangibles,
        //             sol.solution);
        Real prob = 0.0;
        for (uint i = sol.nTransient; i < rg.nodeMarkings.size(); i++)
        {
            prob += sol.solution(i);
        }
        ASSERT_NEAR(prob, 1.0, tol);
        std::cout << "# markings: " << rg.nodeMarkings.size() << std::endl;

        auto reward_load = srnProbReward(
            srn, sol, rg.nodeMarkings,
            [](auto state) { return servedLoad(exp_model, *state.marking); });

        std::cout << "load severed in average: " << reward_load << std::endl;
    }
}

static const std::string data_base = "./data/powerflow/";
TEST(power_grid_model, ieee14_flat)
{
    Real bus_fail = 0.0001;
    Real bus_repair = 0.01;

    Real load_fail = 0.001;
    Real load_repair = 1;

    Real gen_fail = 0.001;
    Real gen_repair = 0.1;

    Real line_fail = 0.005;
    Real line_repair = 0.5;

    auto cdf = readIeeeCdfModel(data_base + "ieee_cdf_models/ieee14cdf.txt");

    exp_model = ieeeCdfModel2ExpModel(cdf);

    {
        auto srn = exp2srn_flat(exp_model, bus_fail, bus_repair, load_fail,
                                load_repair, gen_fail, gen_repair, line_fail,
                                line_repair, 0);
        std::cout << "# bus: " << nbus;
        std::cout << ", # load: " << nload;
        std::cout << ", # gen: " << ngen;
        std::cout << ", # line: " << nline << std::endl;
    }
    for (uint f = 0; f <= 10; f++)
    {
        std::cout << std::endl;
        timed_scope t1("total");
        std::cout << "failures allowed: " << f << std::endl;
        auto srn = exp2srn_flat(exp_model, bus_fail, bus_repair, load_fail,
                                load_repair, gen_fail, gen_repair, line_fail,
                                line_repair, f);
        auto init_mk = createInitMarking(srn);

        ReducedReachGenResult rg;
        {
            timed_scope t2("generation");
            rg = genReducedReachGraph(srn, init_mk, 1e-6, 100);
        }
        std::cout << "# markings: " << rg.nodeMarkings.size() << std::endl;

        uint max_iter = 1000;
        Real tol = 1e-6;
        Real w = 0.8;
        SrnSteadyStateSolution sol;
        {
            timed_scope t2("solution");
            sol = srnSteadyStateDecomp(
                rg.graph, rg.edgeRates, rg.initProbs,
                [=](const Spmatrix& A, VectorMutableView x,
                    VectorConstView b) {
                    auto res = solveSor(A, x, b, w, tol, max_iter);
                    if (res.error > tol || std::isnan(res.error))
                    {
                        std::cout << "Sor. nIter: " << res.nIter;
                        std::cout << ", error: " << res.error << std::endl;
                        throw std::invalid_argument(
                            "Sor failed to converge.");
                    }
                });
        }
        auto reward_load = srnProbReward(
            srn, sol, rg.nodeMarkings,
            [](auto state) { return servedLoad(exp_model, *state.marking); });

        std::cout << "load severed in average: " << reward_load << std::endl;
    }
}

struct DiffTrunc
{
    uint bus;
    uint load;
    uint gen;
    uint line;
};

TEST(power_grid_model, ieee14_diff)
{
    Real bus_fail = 0.0001;
    Real bus_repair = 0.01;

    Real load_fail = 0.001;
    Real load_repair = 1;

    Real gen_fail = 0.001;
    Real gen_repair = 0.1;

    Real line_fail = 0.005;
    Real line_repair = 0.5;

    auto cdf = readIeeeCdfModel(data_base + "ieee_cdf_models/ieee14cdf.txt");

    exp_model = ieeeCdfModel2ExpModel(cdf);

    {
        auto srn = exp2srn_diff(exp_model, bus_fail, bus_repair, load_fail,
                                load_repair, gen_fail, gen_repair, line_fail,
                                line_repair, 0, 0, 0, 0);
        std::cout << "# bus: " << nbus;
        std::cout << ", # load: " << nload;
        std::cout << ", # gen: " << ngen;
        std::cout << ", # line: " << nline << std::endl;
    }
    std::vector<DiffTrunc> trunc = {{0, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 1, 0},
                                    {1, 0, 1, 1}, {2, 0, 1, 1}, {2, 0, 2, 1},

                                    {3, 0, 2, 1}, {2, 0, 2, 2}};
    for (uint i = 7; i < trunc.size(); i++)
    {
        const auto& tr = trunc[i];
        std::cout << std::endl;
        timed_scope t1("total");
        std::cout << "failures allowed: bus  " << tr.bus << ", load "
                  << tr.load << ", gen" << tr.gen << ", line " << tr.line
                  << std::endl;
        auto srn =
            exp2srn_diff(exp_model, bus_fail, bus_repair, load_fail,
                         load_repair, gen_fail, gen_repair, line_fail,
                         line_repair, tr.bus, tr.load, tr.gen, tr.line);
        auto init_mk = createInitMarking(srn);

        ReducedReachGenResult rg;
        {
            timed_scope t2("generation");
            rg = genReducedReachGraph(srn, init_mk, 1e-6, 100);
        }
        std::cout << "# markings: " << rg.nodeMarkings.size() << std::endl;

        uint max_iter = 1000;
        Real tol = 1e-6;
        Real w = 0.8;
        SrnSteadyStateSolution sol;
        {
            timed_scope t2("solution");
            sol = srnSteadyStateDecomp(
                rg.graph, rg.edgeRates, rg.initProbs,
                [=](const Spmatrix& A, VectorMutableView x,
                    VectorConstView b) {
                    auto res = solveSor(A, x, b, w, tol, max_iter);
                    if (res.error > tol || std::isnan(res.error))
                    {
                        std::cout << "Sor. nIter: " << res.nIter;
                        std::cout << ", error: " << res.error << std::endl;
                        throw std::invalid_argument(
                            "Sor failed to converge.");
                    }
                });
        }
        auto reward_load = srnProbReward(
            srn, sol, rg.nodeMarkings,
            [](auto state) { return servedLoad(exp_model, *state.marking); });

        std::cout << "load severed in average: " << reward_load << std::endl;
    }
}
