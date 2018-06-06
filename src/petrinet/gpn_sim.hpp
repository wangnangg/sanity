#pragma once
#include <functional>
#include "gpn.hpp"
#include "petrinet.hpp"
#include "simulate.hpp"

namespace sanity::petrinet
{
struct GpnSimState
{
    // const part
    GeneralPetriNet net;
    std::unique_ptr<MarkingIntf> initMarking;
    std::function<Real()> uniformSampler;

    std::unique_ptr<MarkingIntf> currMarking;
    std::vector<Real> remainingTime;  // for transitions with resume policy

    GpnSimState(GeneralPetriNet net, const MarkingIntf& marking,
                std::function<Real()> usampler)
        : net(std::move(net)),
          initMarking(marking.clone()),
          uniformSampler(std::move(usampler)),
          currMarking(),
          remainingTime()
    {
    }
};

using GpnSimulator = simulate::SimulatorT<uint, GpnSimState>;

GpnSimulator gpnSimulator(GeneralPetriNet net, const MarkingIntf& init_mk,
                          std::function<Real()> uniform_sampler);

}  // namespace sanity::petrinet
