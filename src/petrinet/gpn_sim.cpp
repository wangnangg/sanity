#include "gpn_sim.hpp"
#include <functional>
#include "petrinet.hpp"
#include "simulate.hpp"
#include "type.hpp"

namespace sanity::petrinet
{
using namespace simulate;

static bool isVanMarking(const GeneralPetriNet& net,
                         const std::vector<uint>& enabled)
{
    if (enabled.size() == 0)
    {
        return false;
    }
    uint tid = enabled.front();
    if (net.transProps[tid].type == GpnTransType::Immediate)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static uint whichSlot(const std::vector<Real>& weights, Real rand_val)
{
    uint i = 0;
    Real acc = 0.0;
    for (i = 0; i < weights.size(); i++)
    {
        acc += weights[i];
        if (rand_val <= acc)
        {
            return i;
        }
    }
    assert(false);
    return i;
}

static uint chooseFiringImmeTrans(const GeneralPetriNet& net,
                                  const MarkingIntf* marking,
                                  const std::vector<uint>& enabled_tids,
                                  Real uniform_val)
{
    assert(enabled_tids.size() > 0);
    std::vector<Real> weights(enabled_tids.size());
    Real weight_sum = 0;
    for (uint i = 0; i < enabled_tids.size(); i++)
    {
        uint tid = enabled_tids[i];
        weights[i] = net.transProps[tid].immediate.weight(
            PetriNetState{&net, marking});
        weight_sum += weights[i];
    }
    return enabled_tids[whichSlot(weights, uniform_val * weight_sum)];
}

// static void print(const std::vector<uint>& a)
// {
//     std::cout << "[";
//     for(uint v : a)
//     {
//         std::cout << v << ", ";
//     }
//     std::cout << "]" << std::endl;
// }

// a - b
static std::vector<uint> setSub(std::vector<uint>& a, std::vector<uint>& b)
{
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    std::vector<uint> res;
    res.reserve(a.size());
    auto b_iter = b.begin();
    for (uint v : a)
    {
        bool found = false;
        while (b_iter != b.end())
        {
            if (*b_iter == v)
            {
                found = true;
                break;
            }
            if (*b_iter > v)
            {
                break;
            }
            b_iter++;
        }
        if (!found)
        {
            res.push_back(v);
        }
    }
    return res;
}

using namespace simulate;
GpnSimulator gpnSimulator(GeneralPetriNet net, const MarkingIntf& init_mk,
                          std::function<Real()> uniform_sampler)
{
    GpnSimulator sim(
        GpnSimState(std::move(net), init_mk, std::move(uniform_sampler)));
    sim.handler(EventType::Begin, [](GpnSimulator::Event evt,
                                     GpnSimulator::State& state,
                                     GpnSimulator::EventQueue& queue) {
        state.currMarking = state.initMarking->clone();
        state.remainingTime = std::vector<Real>(state.net.transCount(), -1.0);
        state.enabledTrans =
            state.net.enabledTransitions(state.currMarking.get());

        const auto& enabled_tids = state.enabledTrans;
        bool van = isVanMarking(state.net, enabled_tids);
        if (van)
        {
            uint next_tid =
                chooseFiringImmeTrans(state.net, state.currMarking.get(),
                                      enabled_tids, state.uniformSampler());
            queue.schedule(evt.time, next_tid);
        }
        else
        {
            for (uint tid : enabled_tids)
            {
                Real evt_time =
                    evt.time + state.net.transProps[tid].timed.sampler(
                                   {&state.net, state.currMarking.get()},
                                   state.uniformSampler);
                queue.schedule(evt_time, tid);
            }
        }
    });

    sim.handler(EventType::User, [](GpnSimulator::Event evt,
                                    GpnSimulator::State& state,
                                    GpnSimulator::EventQueue& queue) {
        auto prev_enabled = state.enabledTrans;
        uint firing_tid = evt.data;
        state.currMarking =
            state.net.fireTransition(firing_tid, state.currMarking.get());
        state.remainingTime[firing_tid] = -1.0;
        state.enabledTrans =
            state.net.enabledTransitions(state.currMarking.get());
        auto& curr_enabled = state.enabledTrans;

        bool prev_van = isVanMarking(state.net, prev_enabled);
        bool curr_van = isVanMarking(state.net, curr_enabled);

        if (prev_van)
        {
            if (curr_van)
            {
                // van -> van
                uint next_tid = chooseFiringImmeTrans(
                    state.net, state.currMarking.get(), curr_enabled,
                    state.uniformSampler());
                queue.schedule(evt.time, next_tid);
            }
            else
            {
                // van -> tan
                for (uint tid : curr_enabled)
                {
                    const auto& tp = state.net.transProps[tid];
                    assert(tp.type == GpnTransType::Timed);
                    Real evt_time = -1;
                    switch (tp.timed.samplePolicy)
                    {
                        case GpnSamplePolicy::Resample:
                            break;
                        case GpnSamplePolicy::Resume:
                            if (state.remainingTime[tid] >= 0.0)
                            {
                                evt_time =
                                    evt.time + state.remainingTime[tid];
                            }
                            break;
                    }
                    if (evt_time < 0)
                    {
                        evt_time = evt.time +
                                   tp.timed.sampler(
                                       {&state.net, state.currMarking.get()},
                                       state.uniformSampler);
                    }
                    queue.schedule(evt_time, tid);
                }
            }
        }
        else
        {
            // remove firing_tid from prev_enabled
            auto iter = std::find(prev_enabled.begin(), prev_enabled.end(),
                                  firing_tid);
            assert(iter != prev_enabled.end());
            auto last = prev_enabled.end() - 1;
            std::iter_swap(iter, last);
            prev_enabled.pop_back();

            if (curr_van)
            {
                // tan -> van
                for (uint tid : prev_enabled)
                {
                    const auto& tp = state.net.transProps[tid];
                    assert(tp.type == GpnTransType::Timed);
                    // remove scheduled events
                    GpnSimulator::Event rm_evt;
                    bool removed = queue.removeFirst(
                        [=](GpnSimulator::Event evt) {
                            return evt.data == tid;
                        },
                        &rm_evt);
                    assert(removed);

                    // save remaining time
                    switch (tp.timed.samplePolicy)
                    {
                        case GpnSamplePolicy::Resample:
                            break;
                        case GpnSamplePolicy::Resume:
                            state.remainingTime[tid] = rm_evt.time - evt.time;
                            break;
                    }
                }
                uint next_tid = chooseFiringImmeTrans(
                    state.net, state.currMarking.get(), curr_enabled,
                    state.uniformSampler());
                queue.schedule(evt.time, next_tid);
            }
            else
            {
                // tan -> tan
                auto disabled2enabled = setSub(curr_enabled, prev_enabled);
                auto enabled2disabled = setSub(prev_enabled, curr_enabled);
                for (uint tid : enabled2disabled)
                {
                    const auto& tp = state.net.transProps[tid];
                    assert(tp.type == GpnTransType::Timed);
                    // remove scheduled events
                    GpnSimulator::Event rm_evt;
                    bool removed = queue.removeFirst(
                        [=](GpnSimulator::Event evt) {
                            return evt.data == tid;
                        },
                        &rm_evt);
                    assert(removed);

                    // save remaining time
                    switch (tp.timed.samplePolicy)
                    {
                        case GpnSamplePolicy::Resample:
                            break;
                        case GpnSamplePolicy::Resume:
                            state.remainingTime[tid] = rm_evt.time - evt.time;
                            break;
                    }
                }

                for (uint tid : disabled2enabled)
                {
                    const auto& tp = state.net.transProps[tid];
                    assert(tp.type == GpnTransType::Timed);
                    Real evt_time = -1;
                    switch (tp.timed.samplePolicy)
                    {
                        case GpnSamplePolicy::Resample:
                            break;
                        case GpnSamplePolicy::Resume:
                            if (state.remainingTime[tid] >= 0.0)
                            {
                                evt_time =
                                    evt.time + state.remainingTime[tid];
                            }
                            break;
                    }
                    if (evt_time < 0)
                    {
                        evt_time = evt.time +
                                   tp.timed.sampler(
                                       {&state.net, state.currMarking.get()},
                                       state.uniformSampler);
                    }
                    queue.schedule(evt_time, tid);
                }
            }
        }
    });

    return sim;
}

}  // namespace sanity::petrinet
