#include "gpn.hpp"
#include <cmath>
#include "gpn_sampler.hpp"

namespace sanity::petrinet
{
uint GpnCreator::place(uint token)
{
    uint idx = _init_token.size();
    _init_token.push_back(token);
    return idx;
}

GpnImmeTransition& GpnCreator::immTrans(MarkingDepReal weight)
{
    uint tid = _trans.size();
    _trans.push_back(std::make_unique<GpnImmeTransition>(tid, weight));
    return *dynamic_cast<GpnImmeTransition*>(_trans.back().get());
}
GpnTimedTransition& GpnCreator::timedTrans(GpnSampler firingTime)
{
    uint tid = _trans.size();
    _trans.push_back(std::make_unique<GpnTimedTransition>(
        tid, firingTime, GpnSamplePolicy::Resample));
    return *dynamic_cast<GpnTimedTransition*>(_trans.back().get());
}
GpnTimedTransition& GpnCreator::expTrans(MarkingDepReal rate)
{
    GpnExpSampler sampler(rate);
    return timedTrans(sampler);
}

void GpnCreator::globalEnable(MarkingDepBool g_enable)
{
    _g_enable = g_enable;
}

Marking GpnCreator::marking() const
{
    Marking init(_init_token.size());
    for (uint i = 0; i < _init_token.size(); i++)
    {
        init.setToken(i, (int)_init_token[i]);
    }
    return init;
}

GeneralPetriNet GpnCreator::create() const
{
    std::vector<Transition> trans;
    std::vector<GpnTransProp> props;
    for (const auto& t : _trans)
    {
        trans.push_back(t->_trans);
        props.push_back(t->_prop);
    }
    return {.pnet = PetriNet(_init_token.size(), _g_enable, std::move(trans)),
            .transProps = std::move(props)};
}

GeneralPetriNet srn2gpn(const StochasticRewardNet& srn)
{
    uint tcount = srn.transProps.size();
    std::vector<GpnTransProp> transProps;
    for (uint i = 0; i < tcount; i++)
    {
        const auto& srn_trans = srn.transProps[i];
        auto gpn_trans = GpnTransProp();
        switch (srn_trans.type)
        {
            case SrnTransType::Immediate:
                gpn_trans.type = GpnTransType::Immediate;
                gpn_trans.immediate.weight = srn_trans.val;
                break;
            case SrnTransType::Exponetial:
                gpn_trans.type = GpnTransType::Timed;
                gpn_trans.timed.sampler = GpnExpSampler(srn_trans.val);
                gpn_trans.timed.samplePolicy = GpnSamplePolicy::Resample;
                break;
        }
        transProps.push_back(gpn_trans);
    }
    return GeneralPetriNet{.pnet = srn.pnet,
                           .transProps = std::move(transProps)};
}

}  // namespace sanity::petrinet
