#include "common.hpp"

void syncContext(Context& ct, const MarkingIntf* mk)
{
    // bus
    ct.nBusFailure = 0;
    for (uint pid = ct.busStartMk(); pid < ct.busEndMk(); pid += 2)
    {
        if (mk->nToken(pid + 1) == 1)
        {
            ct.model.buses[ct.marking_map[pid]].busOk = false;
            ct.nBusFailure += 1;
        }
        else
        {
            ct.model.buses[ct.marking_map[pid]].busOk = true;
        }
    }

    // load
    ct.nLoadFailure = 0;
    for (uint pid = ct.loadStartMk(); pid < ct.loadEndMk(); pid += 3)
    {
        if (mk->nToken(pid) == 1)
        {
            ct.model.buses[ct.marking_map[pid]].loadOk = true;
            ct.model.buses[ct.marking_map[pid]].loadConnected = true;
        }
        else if (mk->nToken(pid + 1) == 1)
        {
            ct.model.buses[ct.marking_map[pid]].loadOk = false;
            ct.model.buses[ct.marking_map[pid]].loadConnected = false;
            ct.nLoadFailure += 1;
        }
        else
        {
            ct.model.buses[ct.marking_map[pid]].loadOk = true;
            ct.model.buses[ct.marking_map[pid]].loadConnected = false;
        }
    }

    // gen
    ct.nGenFailure = 0;
    for (uint pid = ct.genStartMk(); pid < ct.genEndMk(); pid += 2)
    {
        if (mk->nToken(pid + 1) == 1)
        {
            ct.model.buses[ct.marking_map[pid]].generatorOk = false;
            ct.nGenFailure += 1;
        }
        else
        {
            ct.model.buses[ct.marking_map[pid]].generatorOk = true;
        }
    }

    // line
    ct.nLineFailure = 0;
    for (uint pid = ct.lineStartMk(); pid < ct.lineEndMk(); pid += 2)
    {
        if (mk->nToken(pid + 1) == 1)
        {
            ct.model.lines[ct.marking_map[pid]].ok = false;
            ct.nLineFailure += 1;
        }
        else
        {
            ct.model.lines[ct.marking_map[pid]].ok = true;
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
    auto converted = exp2DCModel(model);
    auto dc_res = solveDC(converted.dcModel, solveLU);
    for (bool s : dc_res.islandSolved)
    {
        if (!s)
        {
            return false;
        }
    }
    // check over loaded line
    for (const auto& line : model.lines)
    {
        int dc_line_idx = converted.exp2dcLine.forward(line.idx);
        if (dc_line_idx >= 0)
        {
            Real line_power = dc_res.lineRealPowers[(uint)dc_line_idx];
#ifndef NDEBUG
            std::cout << "line " << line.idx << " power: " << line_power
                      << "/" << line.maxPower << std::endl;
#endif
            if (std::abs(line_power) > std::abs(line.maxPower))
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

void powerflowUpdate(Context& ct, const MarkingIntf* mk)
{
    if (ct.last_marking.equal(mk))
    {
        return;
    }
    else
    {
        syncContext(ct, mk);
        shedload(ct.model);
        for (uint i = 0; i < ct.model.nload; i++)
        {
            uint pid = i * 3 + ct.loadStartMk();
            ct.load_connected[i] =
                ct.model.buses[ct.marking_map[pid]].loadConnected;
        }
        ct.last_marking = *dynamic_cast<Marking*>(mk->clone().get());
        return;
    }
}

void createLoad(SrnCreator& ct, Real failure_rate, Real repair_rate, uint idx,
                MarkingDepBool trunc, Context& context)
{
    auto up = ct.place();
    auto down = ct.place();
    auto disc = ct.place();
    ct.expTrans(failure_rate).iarc(up).oarc(down).enable(trunc);
    ct.expTrans(repair_rate).iarc(down).oarc(up);
    auto disc_enable = [idx, &context](PetriNetState state) {
        powerflowUpdate(context, state.marking);
        return !context.load_connected[idx];
    };
    ct.immTrans(1).iarc(up).oarc(disc).enable(disc_enable);
    auto recon_enable = [idx, &context](PetriNetState state) {
        powerflowUpdate(context, state.marking);
        return context.load_connected[idx];
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
StochasticRewardNet exp2srn_sim(Context& context, Real bus_fail,
                                Real bus_repair, Real load_fail,
                                Real load_repair, Real gen_fail,
                                Real gen_repair, Real line_fail,
                                Real line_repair)
{
    SrnCreator ct;
    context.marking_map = std::vector<uint>();
    // create bus
    for (uint i = 0; i < context.model.buses.size(); i++)
    {
        createTwoStates(ct, bus_fail, bus_repair, true);
        context.marking_map.push_back(i);
        context.marking_map.push_back(i);
    }

    // create load
    uint nload = 0;
    for (uint i = 0; i < context.model.buses.size(); i++)
    {
        if (context.model.buses[i].load > 0)
        {
            createLoad(ct, load_fail, load_repair, nload, true, context);
            nload += 1;
            context.marking_map.push_back(i);
            context.marking_map.push_back(i);
            context.marking_map.push_back(i);
        }
    }
    context.load_connected = std::vector<bool>(context.model.nload, true);

    // create gen
    for (uint i = 0; i < context.model.buses.size(); i++)
    {
        if (context.model.buses[i].generation > 0)
        {
            createTwoStates(ct, gen_fail, gen_repair, true);
            context.marking_map.push_back(i);
            context.marking_map.push_back(i);
        }
    }

    // create line
    for (uint i = 0; i < context.model.lines.size(); i++)
    {
        createTwoStates(ct, line_fail, line_repair, true);
        context.marking_map.push_back(i);
        context.marking_map.push_back(i);
    }

    return ct.create();
}
StochasticRewardNet exp2srn_flat(Context& context, Real bus_fail,
                                 Real bus_repair, Real load_fail,
                                 Real load_repair, Real gen_fail,
                                 Real gen_repair, Real line_fail,
                                 Real line_repair, uint nTrunc)
{
    SrnCreator ct;
    context.marking_map = std::vector<uint>();
    // create bus
    for (uint i = 0; i < context.model.buses.size(); i++)
    {
        createTwoStates(ct, bus_fail, bus_repair,
                        [&context, nTrunc](auto state) {
                            powerflowUpdate(context, state.marking);
                            if (context.totalFailure() < nTrunc)
                            {
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        });
        context.marking_map.push_back(i);
        context.marking_map.push_back(i);
    }

    // create load
    uint nload = 0;
    for (uint i = 0; i < context.model.buses.size(); i++)
    {
        if (context.model.buses[i].load > 0)
        {
            createLoad(ct, load_fail, load_repair, nload,
                       [&context, nTrunc](auto state) {
                           powerflowUpdate(context, state.marking);
                           if (context.totalFailure() < nTrunc)
                           {
                               return true;
                           }
                           else
                           {
                               return false;
                           }
                       },
                       context);
            nload += 1;
            context.marking_map.push_back(i);
            context.marking_map.push_back(i);
            context.marking_map.push_back(i);
        }
    }
    context.load_connected = std::vector<bool>(context.model.nload, true);

    // create gen
    for (uint i = 0; i < context.model.buses.size(); i++)
    {
        if (context.model.buses[i].generation > 0)
        {
            createTwoStates(ct, gen_fail, gen_repair,
                            [&context, nTrunc](auto state) {
                                powerflowUpdate(context, state.marking);
                                if (context.totalFailure() < nTrunc)
                                {
                                    return true;
                                }
                                else
                                {
                                    return false;
                                }
                            });
            context.marking_map.push_back(i);
            context.marking_map.push_back(i);
        }
    }

    // create line
    for (uint i = 0; i < context.model.lines.size(); i++)
    {
        createTwoStates(ct, line_fail, line_repair,
                        [&context, nTrunc](auto state) {
                            powerflowUpdate(context, state.marking);
                            if (context.totalFailure() < nTrunc)
                            {
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        });
        context.marking_map.push_back(i);
        context.marking_map.push_back(i);
    }

    return ct.create();
}

StochasticRewardNet exp2srn_diff(Context& context, Real bus_fail,
                                 Real bus_repair, Real load_fail,
                                 Real load_repair, Real gen_fail,
                                 Real gen_repair, Real line_fail,
                                 Real line_repair, uint busTrunc,
                                 uint loadTrunc, uint genTrunc,
                                 uint lineTrunc)
{
    SrnCreator ct;

    context.marking_map = std::vector<uint>();
    // create bus
    for (uint i = 0; i < context.model.buses.size(); i++)
    {
        createTwoStates(ct, bus_fail, bus_repair,
                        [&context, busTrunc](auto state) {
                            powerflowUpdate(context, state.marking);
                            if (context.nBusFailure < busTrunc)
                            {
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        });
        context.marking_map.push_back(i);
        context.marking_map.push_back(i);
    }

    uint nload = 0;
    // create load
    for (uint i = 0; i < context.model.buses.size(); i++)
    {
        if (context.model.buses[i].load > 0)
        {
            createLoad(ct, load_fail, load_repair, nload,
                       [&context, loadTrunc](auto state) {
                           powerflowUpdate(context, state.marking);
                           if (context.nLoadFailure < loadTrunc)
                           {
                               return true;
                           }
                           else
                           {
                               return false;
                           }
                       },
                       context);
            nload += 1;
            context.marking_map.push_back(i);
            context.marking_map.push_back(i);
            context.marking_map.push_back(i);
        }
    }
    context.load_connected = std::vector<bool>(context.model.nload, true);

    // create gen
    for (uint i = 0; i < context.model.buses.size(); i++)
    {
        if (context.model.buses[i].generation > 0)
        {
            createTwoStates(ct, gen_fail, gen_repair,
                            [&context, genTrunc](auto state) {
                                powerflowUpdate(context, state.marking);
                                if (context.nGenFailure < genTrunc)
                                {
                                    return true;
                                }
                                else
                                {
                                    return false;
                                }
                            });
            context.marking_map.push_back(i);
            context.marking_map.push_back(i);
        }
    }

    // create line
    for (uint i = 0; i < context.model.lines.size(); i++)
    {
        createTwoStates(ct, line_fail, line_repair,
                        [&context, lineTrunc](auto state) {
                            powerflowUpdate(context, state.marking);
                            if (context.nLineFailure < lineTrunc)
                            {
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        });
        context.marking_map.push_back(i);
        context.marking_map.push_back(i);
    }

    return ct.create();
}

Marking createInitMarking(const StochasticRewardNet& srn,
                          const Context& context)
{
    Marking mk(srn.pnet.placeCount(), 0);
    for (uint pid = context.busStartMk(); pid < context.busEndMk(); pid += 2)
    {
        mk.setToken(pid, 1);
    }
    for (uint pid = context.loadStartMk(); pid < context.loadEndMk();
         pid += 3)
    {
        mk.setToken(pid, 1);
    }
    for (uint pid = context.genStartMk(); pid < context.genEndMk(); pid += 2)
    {
        mk.setToken(pid, 1);
    }
    for (uint pid = context.lineStartMk(); pid < context.lineEndMk();
         pid += 2)
    {
        mk.setToken(pid, 1);
    }
    return mk;
}

Real servedLoad(Context& context, const MarkingIntf* mk)
{
    Real load = 0.0;
    syncContext(context, mk);
    for (const auto& bus : context.model.buses)
    {
        if (bus.busOk && bus.loadOk && bus.loadConnected)
        {
            load += bus.load;
        }
    }
    return load;
}

DiffRes solveDiff(Context& context, DiffTrunc tr)
{
    Real bus_fail = 0.0001;
    Real bus_repair = 0.01;

    Real load_fail = 0.001;
    Real load_repair = 1;

    Real gen_fail = 0.001;
    Real gen_repair = 0.1;

    Real line_fail = 0.005;
    Real line_repair = 0.5;

    std::cout << std::endl;
    timed_scope t1("total");
    std::cout << "failures allowed: bus  " << tr.bus << ", load " << tr.load
              << ", gen " << tr.gen << ", line " << tr.line << std::endl;

    auto srn = exp2srn_diff(context, bus_fail, bus_repair, load_fail,
                            load_repair, gen_fail, gen_repair, line_fail,
                            line_repair, tr.bus, tr.load, tr.gen, tr.line);
    auto init_mk = createInitMarking(srn, context);

    ReducedReachGenResult rg;
    {
        timed_scope t2("generation");
        rg = genReducedReachGraph(srn, init_mk, 1e-6, 100);
    }
    std::cout << "# markings: " << rg.nodeMarkings.size() << std::endl;

    uint max_iter = 1000;
    Real tol = 1e-6;
    Real w = 0.8;
    SrnSteadyStateSol sol;
    {
        timed_scope t2("solution");
        sol = srnSteadyStateSor(rg.graph, rg.edgeRates, w, tol, max_iter);
    }
    auto reward_load =
        srnProbReward(srn, sol, rg.nodeMarkings, [&context](auto state) {
            return servedLoad(context, state.marking);
        });

    std::cout << "load severed in average: " << reward_load << std::endl;

    return {.reward = reward_load, .nMarkings = (uint)rg.nodeMarkings.size()};
}
