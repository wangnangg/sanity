#include <gtest/gtest.h>
#include <stdexcept>
#include "linear.hpp"
#include "model.hpp"
#include "petrinet.hpp"
#include "powerflow.hpp"
#include "splinear.hpp"

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
    for (uint pid = busStartMk(); pid < busEndMk(); pid += 2)
    {
        if (mk->nToken(pid + 1) == 1)
        {
            model.buses[marking_map[pid]].busOk = false;
        }
        else
        {
            model.buses[marking_map[pid]].busOk = true;
        }
    }

    // load
    for (uint pid = loadStartMk(); pid < loadEndMk(); pid += 3)
    {
        if (mk->nToken(pid + 1) == 1)
        {
            model.buses[marking_map[pid]].loadOk = false;
        }
        else
        {
            model.buses[marking_map[pid]].loadOk = true;
        }
    }

    // gen
    for (uint pid = genStartMk(); pid < genEndMk(); pid += 2)
    {
        if (mk->nToken(pid + 1) == 1)
        {
            model.buses[marking_map[pid]].generatorOk = false;
        }
        else
        {
            model.buses[marking_map[pid]].generatorOk = true;
        }
    }

    // line
    for (uint pid = lineStartMk(); pid < lineEndMk(); pid += 2)
    {
        if (mk->nToken(pid + 1) == 1)
        {
            model.lines[marking_map[pid]].ok = false;
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
    std::cout << "balance factor: " << ratio << std::endl;
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
            std::cout << "line " << line.idx << " power: " << line_power
                      << "/" << line.maxPower << std::endl;
            if (line_power > line.maxPower)
            {
                std::cout << "overloaded." << std::endl;
                return false;
            }
        }
        else
        {
            std::cout << "line " << line.idx << " disconnected." << std::endl;
        }
    }
    return true;
}

void shedload(ExpPowerFlowModel& model)
{
    // optimistic check
    for (auto& bus : model.buses)
    {
        bus.loadConnected = true;
    }

    std::cout << "enable all loads" << std::endl;
    if (validate(model))
    {
        std::cout << "...ok" << std::endl;
        return;
    }

    // disable all
    for (auto& bus : model.buses)
    {
        if (bus.busOk)
        {
            if (bus.load > 0 && bus.loadOk)
            {
                bus.loadConnected = false;
            }
        }
    }

    std::cout << "disable all loads" << std::endl;
    // this should be okay
    assert(validate(model));
    std::cout << "...ok" << std::endl;

    // enable one by one;
    for (auto& bus : model.buses)
    {
        if (bus.busOk)
        {
            if (bus.load > 0 && bus.loadOk)
            {
                std::cout << "enable load on bus " << bus.idx << std::endl;
                bus.loadConnected = true;
                if (!validate(model))
                {
                    std::cout << "...fail" << std::endl;
                    bus.loadConnected = false;
                }
                std::cout << "...ok" << std::endl;
            }
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

const std::vector<bool>& powerflow(const Marking* mk)
{
    if (markingEqual(last_marking, *mk))
    {
        return load_connected;
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
        return load_connected;
    }
}

void createLoad(SrnCreator& ct, Real failure_rate, Real repair_rate, uint idx)
{
    auto up = ct.place();
    auto down = ct.place();
    auto disc = ct.place();
    ct.expTrans(failure_rate).iarc(up).oarc(down);
    ct.expTrans(repair_rate).iarc(down).oarc(up);
    auto disc_enable = [=](PetriNetState state) {
        const auto& lc = powerflow(state.marking);
        return !lc[idx];
    };
    ct.immTrans(1).iarc(up).oarc(disc).enable(disc_enable);
    auto recon_enable = [=](PetriNetState state) {
        const auto& lc = powerflow(state.marking);
        return lc[idx];
    };
    ct.immTrans(1).iarc(disc).oarc(up).enable(recon_enable);
}

void createTwoStates(SrnCreator& ct, Real failure_rate, Real repair_rate)
{
    auto up = ct.place();
    auto down = ct.place();
    ct.expTrans(failure_rate).iarc(up).oarc(down);
    ct.expTrans(repair_rate).iarc(down).oarc(up);
}

static void printPfSol(const DCPowerFlowModel& model,
                       const DCPowerFlowResult sol)
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

StochasticRewardNet exp2srn(const ExpPowerFlowModel& model, Real bus_fail,
                            Real bus_repair, Real load_fail, Real load_repair,
                            Real gen_fail, Real gen_repair, Real line_fail,
                            Real line_repair)
{
    SrnCreator ct;

    marking_map = std::vector<uint>();
    // create bus
    nbus = 0;
    for (uint i = 0; i < model.buses.size(); i++)
    {
        createTwoStates(ct, bus_fail, bus_repair);
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
            createLoad(ct, load_fail, load_repair, nload);
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
            createTwoStates(ct, gen_fail, gen_repair);
            ngen += 1;
            marking_map.push_back(i);
            marking_map.push_back(i);
        }
    }

    // create line
    nline = 0;
    for (uint i = 0; i < model.lines.size(); i++)
    {
        createTwoStates(ct, line_fail, line_repair);
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

Real servedLoad(const ExpPowerFlowModel& model, const Marking& mk)
{
    Real load = 0.0;
    for (uint i = loadStartMk(); i < loadEndMk(); i += 3)
    {
        if (mk.nToken(i) == 1)  // load and bus must be up
        {
            uint bus_idx = marking_map[i];
            load += model.buses[bus_idx].load;
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

    auto srn =
        exp2srn(exp_model, bus_fail, bus_repair, load_fail, load_repair,
                gen_fail, gen_repair, line_fail, line_repair);

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
