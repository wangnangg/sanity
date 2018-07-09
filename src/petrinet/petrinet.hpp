#pragma once
#include <cassert>
#include <functional>
#include <vector>
#include "marking.hpp"
#include "type.hpp"

namespace sanity::petrinet
{
class PetriNet;

struct PetriNetState
{
    const PetriNet* net;
    const MarkingIntf* marking;
};

class MarkingDepBool
{
public:
    using ReturnType = bool;
    using QueryFunc = std::function<ReturnType(PetriNetState state)>;
    MarkingDepBool() : _val(), _func(nullptr) {}
    template <typename QueryFunc,
              std::enable_if_t<!std::is_integral<QueryFunc>::value, int> = 0>
    MarkingDepBool(QueryFunc func) : _val(), _func(func)
    {
    }
    MarkingDepBool(ReturnType value) : _val(value), _func(nullptr) {}

    ReturnType operator()(PetriNetState st) const
    {
        if (_func)
        {
            return _func(st);
        }
        else
        {
            return _val;
        }
    }

private:
    ReturnType _val;
    QueryFunc _func;
};

class MarkingDepUint
{
public:
    using ReturnType = uint;
    using QueryFunc = std::function<ReturnType(PetriNetState)>;
    MarkingDepUint() : _val(), _func(nullptr) {}
    template <typename QueryFunc,
              std::enable_if_t<!std::is_integral<QueryFunc>::value, int> = 0>
    MarkingDepUint(QueryFunc func) : _val(), _func(func)
    {
    }
    MarkingDepUint(ReturnType value) : _val(value), _func(nullptr) {}
    ReturnType operator()(PetriNetState st) const
    {
        if (_func)
        {
            return _func(st);
        }
        else
        {
            return _val;
        }
    }

private:
    ReturnType _val;
    QueryFunc _func;
};

class MarkingDepReal
{
public:
    using ReturnType = Real;
    using QueryFunc = std::function<ReturnType(PetriNetState)>;
    MarkingDepReal() : _val(), _func(nullptr) {}
    template <typename QueryFunc,
              std::enable_if_t<!std::is_integral<QueryFunc>::value, int> = 0>
    MarkingDepReal(QueryFunc func) : _val(), _func(func)
    {
    }
    MarkingDepReal(ReturnType value) : _val(value), _func(nullptr) {}
    ReturnType operator()(PetriNetState st) const
    {
        if (_func)
        {
            return _func(st);
        }
        else
        {
            return _val;
        }
    }

private:
    ReturnType _val;
    QueryFunc _func;
};

struct Arc
{
    uint pid;
    MarkingDepUint multi;
};

struct Transition
{
    uint tid;
    uint prio;
    MarkingDepBool enablingFunc;
    std::vector<Arc> inputArcs;
    std::vector<Arc> outputArcs;
    std::vector<Arc> inhibitorArcs;
};

class PetriNet
{
    uint _place_count;
    MarkingDepBool _global_enabling;
    std::vector<uint> _tid2pos;
    std::vector<Transition> _transitions;

public:
    PetriNet() = default;
    PetriNet(uint place_count, MarkingDepBool g_enable,
             std::vector<Transition> trans);
    virtual ~PetriNet() {}
    const Transition& getTransition(uint tid) const
    {
        return _transitions[_tid2pos[tid]];
    }
    uint transCount() const { return _transitions.size(); }
    uint placeCount() const { return _place_count; }
    std::vector<uint> enabledTransitions(const MarkingIntf* mk) const;
    int firstEanbledTrans(const MarkingIntf* mk) const;
    bool isTransitionEnabled(uint tid, const MarkingIntf* mk) const;
    std::unique_ptr<MarkingIntf> fireTransition(uint tid,
                                                const MarkingIntf* mk) const;

private:
    bool isolateEnableCheck(const Transition& tr,
                            const MarkingIntf* mk) const;
};

}  // namespace sanity::petrinet
