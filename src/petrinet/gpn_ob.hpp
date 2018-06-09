#include "gpn.hpp"
#include "gpn_sim.hpp"

namespace sanity::petrinet
{
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
    virtual void eventTriggered(
        const GpnSimulator::Event& evt, const GpnSimulator::State& state,
        const GpnSimulator::EventQueue& queue) override;
    Real timeSpan() const { return _end_time - _begin_time; }
};

class GpnObProbReward : public GpnObserver
{
    Real _acc_reward;
    Real _last_reward;
    Real _last_time;
    MarkingDepReal _reward;

    std::vector<Real> _samples;

protected:
    virtual void reset(const GpnSimulator::Event& evt,
                       const GpnSimulator::State& state,
                       const GpnSimulator::EventQueue& queue);
    virtual void houseKeeping(const GpnSimulator::Event& evt,
                              const GpnSimulator::State& state,
                              const GpnSimulator::EventQueue& queue);
    virtual void updateReward(const GpnSimulator::Event& evt,
                              const GpnSimulator::State& state,
                              const GpnSimulator::EventQueue& queue);
    virtual void end(const GpnSimulator::Event& evt,
                     const GpnSimulator::State& state,
                     const GpnSimulator::EventQueue& queue);

public:
    GpnObProbReward(Real begin_time, Real end_time, MarkingDepReal reward)
        : GpnObserver(begin_time, end_time), _reward(reward)
    {
    }
    const std::vector<Real>& samples() const { return _samples; }
};

class GpnObCumReward : public GpnObserver
{
    Real _acc_reward;
    Real _last_reward;
    Real _last_time;
    MarkingDepReal _reward;

    std::vector<Real> _samples;

protected:
    virtual void reset(const GpnSimulator::Event& evt,
                       const GpnSimulator::State& state,
                       const GpnSimulator::EventQueue& queue);
    virtual void houseKeeping(const GpnSimulator::Event& evt,
                              const GpnSimulator::State& state,
                              const GpnSimulator::EventQueue& queue);
    virtual void updateReward(const GpnSimulator::Event& evt,
                              const GpnSimulator::State& state,
                              const GpnSimulator::EventQueue& queue);
    virtual void end(const GpnSimulator::Event& evt,
                     const GpnSimulator::State& state,
                     const GpnSimulator::EventQueue& queue);

public:
    GpnObCumReward(Real begin_time, Real end_time, MarkingDepReal reward)
        : GpnObserver(begin_time, end_time), _reward(reward)
    {
    }
    const std::vector<Real>& samples() const { return _samples; }
};

class GpnObMtta : public GpnSimulator::Observer
{
    std::vector<Real> _samples;

public:
    virtual void eventTriggered(
        const GpnSimulator::Event& evt, const GpnSimulator::State& state,
        const GpnSimulator::EventQueue& queue) override;

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

class GpnObEventCounter : public GpnSimulator::Observer
{
    std::vector<Real> _samples;
    uint _counter;

public:
    virtual void eventTriggered(
        const GpnSimulator::Event& evt, const GpnSimulator::State& state,
        const GpnSimulator::EventQueue& queue) override;
    const std::vector<Real>& samples() const { return _samples; }
    GpnObEventCounter() = default;
};

uint gpnPlaceToken(PetriNetState state, uint pid);
MarkingDepReal gpnPlaceTokenFunc(uint pid);

}  // namespace sanity::petrinet
