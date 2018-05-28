#pragma once
#include <algorithm>
#include <functional>
#include <vector>
#include "type.hpp"

namespace sanity::simulate
{
enum class EventType
{
    Begin = 0,
    End = 1,
    User = 2
};

template <typename EventData>
struct EventT
{
    EventType type;
    Real time;
    EventData data;
    EventT() = default;
    EventT(EventType type, Real time) : type(type), time(time), data() {}
    EventT(EventType type, Real time, EventData data)
        : type(type), time(time), data(std::move(data))
    {
    }
};

template <typename EventData>
struct EventComp
{
    bool operator()(const EventT<EventData>& e1,
                    const EventT<EventData>& e2) const
    {
        return e1.time > e2.time;
    }
};

template <typename EventData>
class EventQueueT
{
public:
    using Event = EventT<EventData>;

private:
    std::vector<Event> _pending_events;
    void schedule(Event event)
    {
        auto pos =
            std::lower_bound(_pending_events.begin(), _pending_events.end(),
                             event, EventComp<EventData>());
        _pending_events.insert(pos, std::move(event));
    }

public:
    void schedule(Real time, EventData data)
    {
        schedule(Event{EventType::User, time, std::move(data)});
    }
    std::vector<Event> remove(
        const std::function<bool(const Event& evt)>& should_remove)
    {
        auto first = std::remove_if(_pending_events.begin(),
                                    _pending_events.end(), should_remove);
        auto res = std::vector<Event>(first, _pending_events.end());
        _pending_events.erase(first, _pending_events.end());
        return res;
    }
    bool removeFirst(
        const std::function<bool(const Event& evt)>& should_remove,
        Event* buf)
    {
        auto first = std::find_if(_pending_events.begin(),
                                  _pending_events.end(), should_remove);
        if (first != _pending_events.end())
        {
            *buf = *first;
            _pending_events.erase(first, first + 1);
            return true;
        }
        else
        {
            return false;
        }
    }
    uint size() const { return _pending_events.size(); }
    Event pop()
    {
        auto res = std::move(_pending_events.back());
        _pending_events.pop_back();
        return res;
    }
    const Event& peek(uint i = 0) const
    {
        return _pending_events[_pending_events.size() - 1 - i];
    }
    void clear() { _pending_events.clear(); }
};

template <typename EventData, typename StateType>
class SimulatorT
{
public:
    using EventQueue = EventQueueT<EventData>;
    using Event = typename EventQueue::Event;
    using State = StateType;
    using Handler = std::function<void(const Event& evt, State& state,
                                       EventQueue& queue)>;
    class Observer
    {
    public:
        virtual void eventTriggered(const Event& evt, const State& state,
                                    const EventQueue& queue) = 0;
    };

private:
    EventQueue _queue;
    std::vector<Handler> _handlers;
    std::vector<Observer*> _obs;
    State _state;
    Real _time;

public:
    SimulatorT(State state)
        : _queue(), _handlers(3), _state(std::move(state)), _time(0)
    {
    }
    void handler(EventType type, Handler handler)
    {
        uint hidx = (uint)type;
        _handlers[hidx] = std::move(handler);
    }
    void addObserver(Observer& ob) { _obs.push_back(&ob); }
    void begin()
    {
        _queue.clear();
        _time = 0.0;
        auto evt = Event(EventType::Begin, _time);
        processEvent(evt);
    }
    void runFor(Real duration)
    {
        _time += duration;
        while (_queue.size() > 0)
        {
            Event evt = _queue.peek();
            if (evt.time <= _time)
            {
                evt = _queue.pop();
                processEvent(evt);
            }
            else
            {
                break;
            }
        }
    }
    void end()
    {
        auto evt = Event(EventType::End, _time);
        processEvent(evt);
    }
    Real time() const { return _time; }

private:
    void processEvent(const Event& evt)
    {
        uint hidx = (uint)evt.type;
        if (_handlers[hidx])
        {
            _handlers[hidx](evt, _state, _queue);
        }
        for (auto ob : _obs)
        {
            ob->eventTriggered(evt, _state, _queue);
        }
    }
};

}  // namespace sanity::simulate
