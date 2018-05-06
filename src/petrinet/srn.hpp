#pragma once
#include "linear.hpp"
#include "petrinet.hpp"

namespace sanity::petrinet
{
enum class SRNTransType
{
    Exponetial,
    Immediate
};
struct SRNTransProp
{
    SRNTransType type;
    MarkingDepReal val;  // rate or weight
};
class SRNTransition
{
    Transition _trans;
    SRNTransProp _prop;

public:
    SRNTransition(uint idx, SRNTransType type, MarkingDepReal val)
        : _trans(), _prop{type, val}
    {
        _trans.tid = idx;
        if (type == SRNTransType::Exponetial)
        {
            _trans.prio = 0;
        }
        else
        {
            _trans.prio = 1;
        }
        _trans.enablingFunc = true;
    }
    SRNTransition& iarc(uint place, MarkingDepInt multi = 1)
    {
        _trans.inputArcs.push_back({place, multi});
        return *this;
    }
    SRNTransition& harc(uint place, MarkingDepInt multi = 1)
    {
        _trans.inhibitorArcs.push_back({place, multi});
        return *this;
    }
    SRNTransition& oarc(uint place, MarkingDepInt multi = 1)
    {
        _trans.outputArcs.push_back({place, multi});
        return *this;
    }
    SRNTransition& enable(MarkingDepBool enabling)
    {
        _trans.enablingFunc = enabling;
        return *this;
    }
    SRNTransition& prio(uint prio)
    {
        if (_prop.type == SRNTransType::Exponetial)
        {
            _trans.prio = prio * 2;
        }
        else
        {
            _trans.prio = prio * 2 + 1;
        }
        return *this;
    }
    SRNTransition& rate(MarkingDepReal rate)
    {
        _prop.val = rate;
        return *this;
    }
    SRNTransition& weight(MarkingDepReal w)
    {
        _prop.val = w;
        return *this;
    }
    uint idx() const { return _trans.tid; }
    friend class SRNCreator;
};

struct StochasticRewardNet
{
    PetriNet pnet;
    std::vector<SRNTransProp> transProps;
};

class SRNCreator
{
    std::vector<SRNTransition> _trans;
    uint _place_count;
    MarkingDepBool _g_enable;

public:
    SRNCreator() : _place_count(0), _g_enable(true) {}
    SRNCreator(uint place_count) : _place_count(place_count), _g_enable(true)
    {
    }
    uint place()
    {
        uint idx = _place_count;
        _place_count += 1;
        return idx;
    }
    SRNTransition& expTrans(MarkingDepReal rate = 1.0)
    {
        uint tid = _trans.size();
        _trans.push_back(SRNTransition(tid, SRNTransType::Exponetial, rate));
        return _trans.back();
    }
    SRNTransition& immTrans(MarkingDepReal weight = 1.0)
    {
        uint tid = _trans.size();
        _trans.push_back(SRNTransition(tid, SRNTransType::Immediate, weight));
        return _trans.back();
    }

    void globalEnable(MarkingDepBool g_enable) { _g_enable = g_enable; }

    StochasticRewardNet create() const;
};

}  // namespace sanity::petrinet
