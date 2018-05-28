#include "gpn.hpp"
#include "gpn_sim.hpp"

namespace sanity::petrinet
{
using namespace simulate;
class GpnObserver : public GpnSimulator::Observer
{
protected:
    virtual void reset(const GpnSimulator::Event& evt,
                       const GpnSimulator::State& state,
                       const GpnSimulator::EventQueue& queue) = 0;
    virtual void houseKeeping(const GpnSimulator::Event& evt,
                              const GpnSimulator::State& state,
                              const GpnSimulator::EventQueue& queue) = 0;
    virtual void updateReward(const GpnSimulator::Event& evt,
                              const GpnSimulator::State& state,
                              const GpnSimulator::EventQueue& queue) = 0;
    virtual void end(const GpnSimulator::Event& evt,
                     const GpnSimulator::State& state,
                     const GpnSimulator::EventQueue& queue) = 0;
    Real _begin_time;
    Real _end_time;
    bool _end_passed;

    GpnObserver(Real begin, Real end) : _begin_time(begin), _end_time(end) {}

public:
    void eventTriggered(const GpnSimulator::Event& evt,
                        const GpnSimulator::State& state,
                        const GpnSimulator::EventQueue& queue) override
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
    Real timeSpan() const { return _end_time - _begin_time; }
};

class GpnObPlaceToken : public GpnObserver
{
    uint _pid;

    Real _acc_reward;
    Token _last_token;
    Real _last_time;

    std::vector<Real> _samples;

protected:
    virtual void reset(const GpnSimulator::Event& evt,
                       const GpnSimulator::State& state,
                       const GpnSimulator::EventQueue& queue)
    {
        _acc_reward = 0;
    }
    virtual void houseKeeping(const GpnSimulator::Event& evt,
                              const GpnSimulator::State& state,
                              const GpnSimulator::EventQueue& queue)
    {
        _last_token = state.currMarking.nToken(_pid);
        _last_time = evt.time;
    }
    virtual void updateReward(const GpnSimulator::Event& evt,
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
        _acc_reward += (time - _last_time) * _last_token;
    }
    virtual void end(const GpnSimulator::Event& evt,
                     const GpnSimulator::State& state,
                     const GpnSimulator::EventQueue& queue)
    {
        _samples.push_back(_acc_reward / timeSpan());
    }

public:
    GpnObPlaceToken(uint pid, Real begin_time, Real end_time)
        : GpnObserver(begin_time, end_time), _pid(pid)
    {
    }
    const std::vector<Real>& samples() const { return _samples; }
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
