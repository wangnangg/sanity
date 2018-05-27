#pragma once
#include "gpn.hpp"

namespace sanity::petrinet
{
class GpnExpSampler
{
    MarkingDepReal _rate;

public:
    GpnExpSampler(MarkingDepReal rate) : _rate(std::move(rate)) {}
    Real operator()(PetriNetState pstate,
                    std::function<Real()>& uniform_sampler) const;
};

}  // namespace sanity::petrinet
