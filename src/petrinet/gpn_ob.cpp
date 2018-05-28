#include "gpn_ob.hpp"
#include "utils.hpp"

namespace sanity::petrinet
{
using namespace simulate;

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
