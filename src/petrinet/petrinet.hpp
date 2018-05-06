#pragma once
#include <functional>
#include <vector>
#include "type.hpp"
namespace sanity::petrinet
{
using Token = int;

class Marking
{
    // make sure the address of this buffer doesn't change
    Token* _tokens;
    uint _nplace;

public:
    Marking(uint nplace, int ntoken = 0) : _nplace(nplace)
    {
        _tokens = new Token[nplace];
        for (uint i = 0; i < nplace; i++)
        {
            _tokens[i] = ntoken;
        }
    }
    Marking() : _tokens(nullptr), _nplace(0) {}
    Marking(const Marking&) = delete;
    Marking(Marking&& mk) : _tokens(mk._tokens), _nplace(mk._nplace)
    {
        mk._nplace = 0;
        mk._tokens = nullptr;
    }
    ~Marking() { delete _tokens; }
    uint size() const { return _nplace; }
    const Token& nToken(uint pid) const { return _tokens[pid]; }
    void setToken(uint pid, Token num) { _tokens[pid] = num; }
    void deposit(uint pid, Token num) { _tokens[pid] += num; }
    void remove(uint pid, Token num) { _tokens[pid] -= num; }

    // make it explicit
    Marking clone() const
    {
        auto mk = Marking();
        mk._tokens = new Token[_nplace];
        mk._nplace = _nplace;
        for (uint i = 0; i < _nplace; i++)
        {
            mk.setToken(i, nToken(i));
        }
        return mk;
    }
};

class PetriNet;

struct State
{
    const PetriNet* net;
    const Marking* marking;
};

class MarkingDepBool
{
public:
    using ReturnType = bool;
    using QueryFunc = std::function<ReturnType(State)>;
    MarkingDepBool() : _val(), _func(nullptr) {}
    MarkingDepBool(QueryFunc func) : _func(func) {}
    MarkingDepBool(ReturnType value) : _val(value), _func(nullptr) {}
    ReturnType operator()(State st) const
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
    using QueryFunc = std::function<ReturnType(State)>;
    MarkingDepInt() : _val(), _func(nullptr) {}
    MarkingDepInt(QueryFunc func) : _func(func) {}
    MarkingDepInt(ReturnType value) : _val(value), _func(nullptr) {}
    ReturnType operator()(State st) const
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
    using QueryFunc = std::function<ReturnType(State)>;
    MarkingDepReal() : _val(), _func(nullptr) {}
    MarkingDepReal(QueryFunc func) : _func(func) {}
    MarkingDepReal(ReturnType value) : _val(value), _func(nullptr) {}
    ReturnType operator()(State st) const
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
