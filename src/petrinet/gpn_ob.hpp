#include "gpn.hpp"
#include "gpn_sim.hpp"

namespace sanity::petrinet
{
class GpnObPlaceNonEmpty : public GpnSimulator::Observer
{
    uint _pid;
    Real _acc_time;
    bool _non_empty;
    Real _last_time;

public:
    GpnObPlaceNonEmpty(uint pid) : _pid(pid) {}

    virtual void eventTriggered(
        const GpnSimulator::Event& evt, const GpnSimulator::State& state,
        const GpnSimulator::EventQueue& queue) override;
    Real prob() const { return _acc_time / _last_time; }
    Real accSojTime() const { return _acc_time; }
};

class GpnObPlaceToken : public GpnSimulator::Observer
{
    uint _pid;
    Real _acc_reward;
    Token _last_token;
    Real _last_time;

public:
    GpnObPlaceToken(uint pid) : _pid(pid) {}

    virtual void eventTriggered(
        const GpnSimulator::Event& evt, const GpnSimulator::State& state,
        const GpnSimulator::EventQueue& queue) override;
    Real avgToken() const { return _acc_reward / _last_time; }
};

class GpnObLog : public GpnSimulator::Observer
{
    bool _log_event;
    bool _log_marking;
    bool _log_queue;

public:
    GpnObLog(bool on) : _log_event(on), _log_marking(on), _log_queue(on) {}
    void logEvent(bool log_event) { _log_event = log_event; }
    void logMarking(bool log_marking) { _log_marking = log_marking; }
    void logQueue(bool log_queue) { _log_queue = log_queue; }
    virtual void eventTriggered(
        const GpnSimulator::Event& evt, const GpnSimulator::State& state,
        const GpnSimulator::EventQueue& queue) override;
};
}  // namespace sanity::petrinet
