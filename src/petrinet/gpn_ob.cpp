#include "gpn_ob.hpp"
#include "utils.hpp"

namespace sanity::petrinet
{
using namespace simulate;
void GpnObPlaceNonEmpty::eventTriggered(const GpnSimulator::Event& evt,
                                        const GpnSimulator::State& state,
                                        const GpnSimulator::EventQueue& queue)
{
    switch (evt.type)
    {
        case EventType::User:
            if (_non_empty)
            {
                _acc_time += evt.time - _last_time;
            }
            _non_empty = state.currMarking.nToken(_pid) > 0;
            break;
        case EventType::Begin:
            _acc_time = 0;
            _non_empty = state.currMarking.nToken(_pid) > 0;
            break;
        case EventType::End:
            if (_non_empty)
            {
                _acc_time += evt.time - _last_time;
            }
            break;
    }
    _last_time = evt.time;
}

void GpnObPlaceToken::eventTriggered(const GpnSimulator::Event& evt,
                                     const GpnSimulator::State& state,
                                     const GpnSimulator::EventQueue& queue)
{
    switch (evt.type)
    {
        case EventType::User:
            _acc_reward += (Real)_last_token * (evt.time - _last_time);
            _last_token = state.currMarking.nToken(_pid);
            break;
        case EventType::Begin:
            _last_token = state.currMarking.nToken(_pid);
            _acc_reward = 0;
            break;
        case EventType::End:
            _acc_reward += (Real)_last_token * (evt.time - _last_time);
            break;
    }
    _last_time = evt.time;
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
        std::cout << "marking " << state.currMarking << std::endl;
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
}  // namespace sanity::petrinet
