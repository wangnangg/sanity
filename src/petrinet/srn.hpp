#pragma once
#include "linear.hpp"
#include "petrinet.hpp"

namespace sanity::petrinet
{
enum class SrnTransType
{
    Exponetial,
    Immediate
};
struct SrnTransProp
{
    SrnTransType type;
    MarkingDepReal val;  // rate or weight
};
class SrnTransition
{
    Transition _trans;
    SrnTransProp _prop;

public:
    SrnTransition(uint idx, SrnTransType type, MarkingDepReal val)
        : _trans(), _prop{type, val}
    {
        _trans.tid = idx;
        if (type == SrnTransType::Exponetial)
        {
            _trans.prio = 0;
        }
        else
        {
            _trans.prio = 1;
        }
        _trans.enablingFunc = true;
    }
    SrnTransition& iarc(uint place, MarkingDepInt multi = 1)
    {
        _trans.inputArcs.push_back({place, multi});
        return *this;
    }
    SrnTransition& harc(uint place, MarkingDepInt multi = 1)
    {
        _trans.inhibitorArcs.push_back({place, multi});
        return *this;
    }
    SrnTransition& oarc(uint place, MarkingDepInt multi = 1)
    {
        _trans.outputArcs.push_back({place, multi});
        return *this;
    }
    SrnTransition& enable(MarkingDepBool enabling)
    {
        _trans.enablingFunc = enabling;
        return *this;
    }
    SrnTransition& prio(uint prio)
    {
        if (_prop.type == SrnTransType::Exponetial)
        {
            _trans.prio = prio * 2;
        }
        else
        {
            _trans.prio = prio * 2 + 1;
        }
        return *this;
    }
    SrnTransition& rate(MarkingDepReal rate)
    {
        _prop.val = rate;
        return *this;
    }
    SrnTransition& weight(MarkingDepReal w)
    {
        _prop.val = w;
        return *this;
    }
    uint idx() const { return _trans.tid; }
    friend class SrnCreator;
};

struct StochasticRewardNet
{
    PetriNet pnet;
    std::vector<SrnTransProp> transProps;
};

class SrnCreator
{
    std::vector<SrnTransition> _trans;
    std::vector<uint> _init_token;
    MarkingDepBool _g_enable;

public:
    SrnCreator() : _g_enable(true) {}
    uint place(uint token = 0);
    SrnTransition& expTrans(MarkingDepReal rate);
    SrnTransition& immTrans(MarkingDepReal weight = 1.0);
    void globalEnable(MarkingDepBool g_enable);
    StochasticRewardNet create() const;
    Marking marking() const;
};

}  // namespace sanity::petrinet
