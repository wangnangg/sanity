#include "srn.hpp"
#include "linear.hpp"
#include "reach.hpp"

namespace sanity::petrinet
{
using namespace linear;
StochasticRewardNet SrnCreator::create() const
{
    std::vector<Transition> trans;
    std::vector<SrnTransProp> props;
    for (const auto& t : _trans)
    {
        trans.push_back(t._trans);
        props.push_back(t._prop);
    }
    return {.pnet = PetriNet(_init_token.size(), _g_enable, std::move(trans)),
            .transProps = std::move(props)};
}

uint SrnCreator::place(uint token)
{
    uint idx = _init_token.size();
    _init_token.push_back(token);
    return idx;
}
SrnTransition& SrnCreator::expTrans(MarkingDepReal rate)
{
    uint tid = _trans.size();
    _trans.push_back(SrnTransition(tid, SrnTransType::Exponetial, rate));
    return _trans.back();
}
SrnTransition& SrnCreator::immTrans(MarkingDepReal weight)
{
    uint tid = _trans.size();
    _trans.push_back(SrnTransition(tid, SrnTransType::Immediate, weight));
    return _trans.back();
}

void SrnCreator::globalEnable(MarkingDepBool g_enable)
{
    _g_enable = g_enable;
}

Marking SrnCreator::marking() const
{
    Marking init(_init_token.size());
    for (uint i = 0; i < _init_token.size(); i++)
    {
        init.setToken(i, _init_token[i]);
    }
    return init;
}

BitMarking SrnCreator::bitMarking() const
{
    BitMarking init(_init_token.size());
    for (uint i = 0; i < _init_token.size(); i++)
    {
        init.setToken(i, _init_token[i]);
    }
    return init;
}

ByteMarking SrnCreator::byteMarking() const
{
    ByteMarking init(_init_token.size());
    for (uint i = 0; i < _init_token.size(); i++)
    {
        init.setToken(i, _init_token[i]);
    }
    return init;
}

}  // namespace sanity::petrinet
