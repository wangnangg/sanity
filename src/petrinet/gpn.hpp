#pragma once
#include <memory>
#include "petrinet.hpp"
#include "srn.hpp"

namespace sanity::petrinet
{
enum class GpnTransType
{
    Immediate,
    Timed
};

enum class GpnSamplePolicy
{
    Resample,
    Resume
};

using GpnSampler = std::function<Real(
    PetriNetState pstate, std::function<Real()>& uniform_sampler)>;

struct GpnTransProp
{
    GpnTransType type;
    struct
    {
        GpnSampler sampler;
        GpnSamplePolicy samplePolicy;
    } timed;
    struct
    {
        MarkingDepReal weight;
    } immediate;
};

class GpnTransition
{
protected:
    Transition _trans;
    GpnTransProp _prop;
    GpnTransition(uint idx, GpnTransType type, GpnSampler sampler,
                  GpnSamplePolicy policy, MarkingDepReal weight)
        : _trans(), _prop{type, {std::move(sampler), policy}, {weight}}

    {
        _trans.tid = idx;
        if (type == GpnTransType::Timed)
        {
            _trans.prio = 0;
        }
        else
        {
            _trans.prio = 1;
        }
        _trans.enablingFunc = true;
    }

public:
    virtual ~GpnTransition() = default;
    friend class GpnCreator;
};
class GpnImmeTransition : public GpnTransition
{
public:
    GpnImmeTransition(uint idx, MarkingDepReal weight)
        : GpnTransition(idx, GpnTransType::Immediate, GpnSampler(),
                        GpnSamplePolicy(), weight)
    {
    }
    GpnImmeTransition& iarc(uint place, MarkingDepUint multi = 1)
    {
        _trans.inputArcs.push_back({place, multi});
        return *this;
    }
    GpnImmeTransition& harc(uint place, MarkingDepUint multi = 1)
    {
        _trans.inhibitorArcs.push_back({place, multi});
        return *this;
    }
    GpnImmeTransition& oarc(uint place, MarkingDepUint multi = 1)
    {
        _trans.outputArcs.push_back({place, multi});
        return *this;
    }
    GpnImmeTransition& enable(MarkingDepBool enabling)
    {
        _trans.enablingFunc = enabling;
        return *this;
    }
    GpnImmeTransition& prio(uint prio)
    {
        if (_prop.type == GpnTransType::Timed)
        {
            _trans.prio = prio * 2;
        }
        else
        {
            _trans.prio = prio * 2 + 1;
        }
        return *this;
    }
    uint idx() const { return _trans.tid; }
};

class GpnTimedTransition : public GpnTransition
{
public:
    GpnTimedTransition(uint idx, GpnSampler sampler, GpnSamplePolicy policy)
        : GpnTransition(idx, GpnTransType::Timed, std::move(sampler), policy,
                        0.0)
    {
    }
    GpnTimedTransition& iarc(uint place, MarkingDepUint multi = 1)
    {
        _trans.inputArcs.push_back({place, multi});
        return *this;
    }
    GpnTimedTransition& harc(uint place, MarkingDepUint multi = 1)
    {
        _trans.inhibitorArcs.push_back({place, multi});
        return *this;
    }
    GpnTimedTransition& oarc(uint place, MarkingDepUint multi = 1)
    {
        _trans.outputArcs.push_back({place, multi});
        return *this;
    }
    GpnTimedTransition& enable(MarkingDepBool enabling)
    {
        _trans.enablingFunc = enabling;
        return *this;
    }
    GpnTimedTransition& prio(uint prio)
    {
        if (_prop.type == GpnTransType::Timed)
        {
            _trans.prio = prio * 2;
        }
        else
        {
            _trans.prio = prio * 2 + 1;
        }
        return *this;
    }
    GpnTimedTransition& policy(GpnSamplePolicy policy)
    {
        _prop.timed.samplePolicy = policy;
        return *this;
    }
    uint idx() const { return _trans.tid; }
    friend class GpnCreator;
};

struct GeneralPetriNet
{
    PetriNet pnet;
    std::vector<GpnTransProp> transProps;
};

class GpnCreator
{
    std::vector<std::unique_ptr<GpnTransition>> _trans;
    MarkingDepBool _g_enable;
    std::vector<uint> _init_token;

public:
    GpnCreator() : _g_enable(true) {}
    uint place(uint token = 0);
    GpnImmeTransition& immTrans(MarkingDepReal weight = 1.0);
    GpnTimedTransition& timedTrans(GpnSampler firingTime);
    GpnTimedTransition& expTrans(MarkingDepReal rate);
    void globalEnable(MarkingDepBool g_enable);
    GeneralPetriNet create() const;
    Marking marking() const;
    BitMarking bitMarking() const;
    ByteMarking byteMarking() const;
};

GeneralPetriNet srn2gpn(const StochasticRewardNet& srn);

}  // namespace sanity::petrinet
