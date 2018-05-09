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
    uint _place_count;
    MarkingDepBool _g_enable;

public:
    SrnCreator() : _place_count(0), _g_enable(true) {}
    SrnCreator(uint place_count) : _place_count(place_count), _g_enable(true)
    {
    }
    uint place()
    {
        uint idx = _place_count;
        _place_count += 1;
        return idx;
    }
    SrnTransition& expTrans(MarkingDepReal rate = 1.0)
    {
        uint tid = _trans.size();
        _trans.push_back(SrnTransition(tid, SrnTransType::Exponetial, rate));
        return _trans.back();
    }
    SrnTransition& immTrans(MarkingDepReal weight = 1.0)
    {
        uint tid = _trans.size();
        _trans.push_back(SrnTransition(tid, SrnTransType::Immediate, weight));
        return _trans.back();
    }

    void globalEnable(MarkingDepBool g_enable) { _g_enable = g_enable; }

    StochasticRewardNet create() const;
};

}  // namespace sanity::petrinet
