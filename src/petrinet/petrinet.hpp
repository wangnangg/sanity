#pragma once
#include <cassert>
#include <functional>
#include <vector>
#include "type.hpp"
namespace sanity::petrinet
{
using Token = int;

class Marking
{
    // make sure the address of this buffer doesn't change
    std::vector<Token> _tokens;

    Marking(const Marking&) = default;
    Marking& operator=(const Marking&) = default;

public:
    Marking() = default;
    Marking(uint nplace, int ntoken = 0) : _tokens(nplace, ntoken) {}
    Marking& operator=(Marking&& mk) = default;
    Marking(Marking&& mk) = default;
    ~Marking() = default;
    uint size() const { return _tokens.size(); }
    const Token& nToken(uint pid) const { return _tokens[pid]; }
    void setToken(uint pid, Token num) { _tokens[pid] = num; }
    void deposit(uint pid, Token num) { _tokens[pid] += num; }
    void remove(uint pid, Token num)
    {
        assert(_tokens[pid] >= num);
        _tokens[pid] -= num;
    }

    // make it explicit
    Marking clone() const { return *this; }
};

class PetriNet;

struct PetriNetState
{
    const void* net;
    const Marking* marking;
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

class MarkingDepInt
{
public:
    using ReturnType = int;
    using QueryFunc = std::function<ReturnType(PetriNetState)>;
    MarkingDepInt() : _val(), _func(nullptr) {}
    template <typename QueryFunc,
              std::enable_if_t<!std::is_integral<QueryFunc>::value, int> = 0>
    MarkingDepInt(QueryFunc func) : _val(), _func(func)
    {
    }
    MarkingDepInt(ReturnType value) : _val(value), _func(nullptr) {}
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
    MarkingDepInt multi;
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
    PetriNet(uint place_count, MarkingDepBool g_enable,
             std::vector<Transition> trans);
    const Transition& getTransition(uint tid) const
    {
        return _transitions[_tid2pos[tid]];
    }
    uint transCount() const { return _transitions.size(); }
    uint placeCount() const { return _place_count; }
    std::vector<uint> enabledTransitions(const Marking& mk) const;
    int firstEanbledTrans(const Marking& mk) const;
    bool isTransitionEnabled(uint tid, const Marking& mk) const;
    Marking fireTransition(uint tid, const Marking& mk) const;

private:
    bool isolateEnableCheck(const Transition& tr, const Marking& mk) const;
};

}  // namespace sanity::petrinet
