#include "gpn_ob.hpp"
#include <iostream>
#include "utils.hpp"

namespace sanity::petrinet
{
using namespace simulate;

void GpnObserver::eventTriggered(const GpnSimulator::Event& evt,
                                 const GpnSimulator::State& state,
                                 const GpnSimulator::EventQueue& queue)
{
    switch (evt.type)
    {
        case EventType::User:
            if (evt.time >= _begin_time)
            {
                if (evt.time <= _end_time)
                {
                    updateReward(evt, state, queue);
                }
                else
                {
                    if (!_end_passed)
                    {
                        updateReward(evt, state, queue);
                        end(evt, state, queue);
                        _end_passed = true;
                    }
                }
            }
            break;
        case EventType::Begin:
            reset(evt, state, queue);
            _end_passed = false;
            break;
        case EventType::End:
            if (!_end_passed)
            {
                updateReward(evt, state, queue);
                end(evt, state, queue);
                _end_passed = true;
            }
            break;
    }
    houseKeeping(evt, state, queue);
}

void GpnObProbReward::reset(const GpnSimulator::Event& evt,
                            const GpnSimulator::State& state,
                            const GpnSimulator::EventQueue& queue)
{
    _acc_reward = 0;
}
void GpnObProbReward::houseKeeping(const GpnSimulator::Event& evt,
                                   const GpnSimulator::State& state,
                                   const GpnSimulator::EventQueue& queue)
{
    _last_reward = _reward({&state.net, state.currMarking.get()});
    _last_time = evt.time;
}
void GpnObProbReward::updateReward(const GpnSimulator::Event& evt,
                                   const GpnSimulator::State& state,
                                   const GpnSimulator::EventQueue& queue)
{
    Real time;
    if (evt.time >= _end_time)
    {
        time = _end_time;
    }
    else
    {
        time = evt.time;
    }
    _acc_reward += (time - _last_time) * _last_reward;
}
void GpnObProbReward::end(const GpnSimulator::Event& evt,
                          const GpnSimulator::State& state,
                          const GpnSimulator::EventQueue& queue)
{
    _samples.push_back(_acc_reward / timeSpan());
}

void GpnObCumReward::reset(const GpnSimulator::Event& evt,
                           const GpnSimulator::State& state,
                           const GpnSimulator::EventQueue& queue)
{
    _acc_reward = 0;
}
void GpnObCumReward::houseKeeping(const GpnSimulator::Event& evt,
                                  const GpnSimulator::State& state,
                                  const GpnSimulator::EventQueue& queue)
{
    _last_reward = _reward({&state.net, state.currMarking.get()});
    _last_time = evt.time;
}
void GpnObCumReward::updateReward(const GpnSimulator::Event& evt,
                                  const GpnSimulator::State& state,
                                  const GpnSimulator::EventQueue& queue)
{
    Real time;
    if (evt.time >= _end_time)
    {
        time = _end_time;
    }
    else
    {
        time = evt.time;
    }
    _acc_reward += (time - _last_time) * _last_reward;
}
void GpnObCumReward::end(const GpnSimulator::Event& evt,
                         const GpnSimulator::State& state,
                         const GpnSimulator::EventQueue& queue)
{
    _samples.push_back(_acc_reward);
}

void GpnObLog::eventTriggered(const GpnSimulator::Event& evt,
                              const GpnSimulator::State& state,
                              const GpnSimulator::EventQueue& queue)
{
    if (_log_event)
    {
        std::cout << "event (" << (uint)evt.type << ") time:" << evt.time
                  << " data:" << evt.data << std::endl;
    }
    if (_log_marking)
    {
        std::cout << "marking " << *state.currMarking << std::endl;
    }
    if (_log_queue)
    {
        std::cout << "queue {";
        for (uint i = 0; i < queue.size(); i++)
        {
            std::cout << "(" << (uint)queue.peek(i).type << ", "
                      << queue.peek(i).time << ", " << queue.peek(i).data
                      << ") ";
        }
        std::cout << "}" << std::endl;
    }
}

void GpnObEventCounter::eventTriggered(const GpnSimulator::Event& evt,
                                       const GpnSimulator::State& state,
                                       const GpnSimulator::EventQueue& queue)
{
    switch (evt.type)
    {
        case EventType::Begin:
            _counter = 1;
            break;
        case EventType::User:
            _counter += 1;
            break;
        case EventType::End:
            _counter += 1;
            _samples.push_back(_counter);
            break;
    }
}

uint gpnPlaceToken(PetriNetState state, uint pid)
{
    return state.marking->nToken(pid);
}
MarkingDepReal gpnPlaceTokenFunc(uint pid)
{
    return [pid](PetriNetState state) { return gpnPlaceToken(state, pid); };
}

}  // namespace sanity::petrinet
