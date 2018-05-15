#pragma once

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

struct Context
{
    Marking last_marking;
    std::vector<bool> load_connected;
    ExpPowerFlowModel model;
    uint nBusFailure;
    uint nLoadFailure;
    uint nGenFailure;
    uint nLineFailure;
    std::vector<uint>
        marking_map;  // map marking to the corresponding bus/line idx

    Context(const Context&) = delete;
    Context(Context&&) = delete;
    Context() = default;

    uint busStartMk() const { return 0; }
    uint busEndMk() const { return model.nbus * 2; }
    uint loadStartMk() const { return model.nbus * 2; }
    uint loadEndMk() const { return model.nbus * 2 + model.nload * 3; }
    uint genStartMk() const { return model.nbus * 2 + model.nload * 3; }
    uint genEndMk() const
    {
        return model.nbus * 2 + model.nload * 3 + model.ngen * 2;
    }
    uint lineStartMk() const
    {
        return model.nbus * 2 + model.nload * 3 + model.ngen * 2;
    }
    uint lineEndMk() const
    {
        return model.nbus * 2 + model.nload * 3 + model.ngen * 2 +
               model.nline * 2;
    }
    uint totalFailure() const
    {
        return nBusFailure + nLoadFailure + nGenFailure + nLineFailure;
    }
};

void syncContext(Context& ct, const Marking* mk);
bool balance(ExpPowerFlowModel& model);
bool validate(ExpPowerFlowModel& model);
void shedload(ExpPowerFlowModel& model);
bool markingEqual(const Marking& m1, const Marking& m2);
void powerflowUpdate(Context& ct, const Marking* mk);
void createLoad(SrnCreator& ct, Real failure_rate, Real repair_rate, uint idx,
                MarkingDepBool trunc, Context& context);
void createTwoStates(SrnCreator& ct, Real failure_rate, Real repair_rate,
                     MarkingDepBool trunc);

StochasticRewardNet exp2srn_flat(Context& context, Real bus_fail,
                                 Real bus_repair, Real load_fail,
                                 Real load_repair, Real gen_fail,
                                 Real gen_repair, Real line_fail,
                                 Real line_repair, uint nTrunc);

StochasticRewardNet exp2srn_diff(Context& context, Real bus_fail,
                                 Real bus_repair, Real load_fail,
                                 Real load_repair, Real gen_fail,
                                 Real gen_repair, Real line_fail,
                                 Real line_repair, uint busTrunc,
                                 uint loadTrunc, uint genTrunc,
                                 uint lineTrunc);

Marking createInitMarking(const StochasticRewardNet& srn,
                          const Context& context);
Real servedLoad(Context& context, const Marking& mk);

static const std::string data_base = "./data/powerflow/";
