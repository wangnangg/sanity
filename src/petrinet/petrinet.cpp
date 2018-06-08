#include "petrinet.hpp"
#include <cassert>
namespace sanity::petrinet
{
static bool cmpTrans(const Transition& x, const Transition& y)
{
    return x.prio > y.prio;
}
PetriNet::PetriNet(uint place_count, MarkingDepBool g_enable,

                   std::vector<Transition> trans)
    : _place_count(place_count),
      _global_enabling(g_enable),
      _tid2pos(trans.size()),
      _transitions(std::move(trans))
{
    std::sort(_transitions.begin(), _transitions.end(),
              cmpTrans);  // from high prio to low prio
    for (size_t i = 0; i < _transitions.size(); i++)
    {
        _tid2pos[_transitions[i].tid] = i;
    }
}

bool PetriNet::isolateEnableCheck(const Transition& tr,
                                  const MarkingIntf* mk) const
{
    auto state = PetriNetState{this, mk};
    if (!_global_enabling(state))
    {
        return false;
    }
    if (!tr.enablingFunc(state))
    {
        return false;
    }
    for (const auto& arc : tr.inputArcs)
    {
        if (mk->nToken(arc.pid) < arc.multi(state))
        {
            return false;
        }
    }
    for (const auto& arc : tr.inhibitorArcs)
    {
        if (mk->nToken(arc.pid) >= arc.multi(state))
        {
            return false;
        }
    }
    return true;
}

int PetriNet::firstEanbledTrans(const MarkingIntf* mk) const
{
    for (const auto& tr : _transitions)
    {
        if (isolateEnableCheck(tr, mk))
        {
            return (int)tr.tid;
        }
    }
    return -1;
}

std::vector<uint> PetriNet::enabledTransitions(const MarkingIntf* mk) const
{
    auto etrans = std::vector<uint>();
    bool found_enabled = false;
    uint64_t enabled_prio;
    for (const auto& tr : _transitions)
    {
        if (found_enabled && enabled_prio > tr.prio)
        {
            return etrans;
        }
        if (isolateEnableCheck(tr, mk))
        {
            etrans.push_back(tr.tid);
            enabled_prio = tr.prio;
            found_enabled = true;
        }
    }
    return etrans;
}

std::unique_ptr<MarkingIntf> PetriNet::fireTransition(
    uint tid, const MarkingIntf* mk) const
{
    auto newmk = mk->clone();
    const auto& tr = getTransition(tid);
    auto state = PetriNetState{this, mk};
    for (const auto& arc : tr.inputArcs)
    {
        auto multi = arc.multi(state);
        assert(newmk->nToken(arc.pid) >= multi);
        newmk->setToken(arc.pid, newmk->nToken(arc.pid) - multi);
    }
    for (const auto& arc : tr.outputArcs)
    {
        auto multi = arc.multi(state);
        newmk->setToken(arc.pid, newmk->nToken(arc.pid) + multi);
    }
    return newmk;
}

bool PetriNet::isTransitionEnabled(uint tid, const MarkingIntf* mk) const
{
    bool found_enabled = false;
    uint64_t enabled_prio;
    for (const auto& tr : _transitions)
    {
        if (found_enabled && enabled_prio > tr.prio)
        {
            return false;
        }
        if (tr.tid == tid)
        {
            return isolateEnableCheck(tr, mk);
        }
        if (isolateEnableCheck(tr, mk))
        {
            enabled_prio = tr.prio;
            found_enabled = true;
        }
    }
    return false;
}

}  // namespace sanity::petrinet
