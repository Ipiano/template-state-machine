#pragma once

#include <functional>
#include <tuple>

namespace state_machine
{

template<class StateT, class EventT>
struct Language
{
    typedef StateT state_t;
    typedef EventT event_t;

    template<state_t From, state_t To, event_t OnEvent>
    struct Transition
    {
        constexpr static state_t from = From;
        constexpr static state_t to = To;
        constexpr static event_t when = OnEvent;
    };
};

template<class StateT, class EventT>
template<StateT From, StateT To, EventT Evt>
constexpr StateT Language<StateT, EventT>::Transition<From, To, Evt>::from;

template<class StateT, class EventT>
template<StateT From, StateT To, EventT Evt>
constexpr StateT Language<StateT, EventT>::Transition<From, To, Evt>::to;

template<class StateT, class EventT>
template<StateT From, StateT To, EventT Evt>
constexpr EventT Language<StateT, EventT>::Transition<From, To, Evt>::when;

template<class LangT, typename LangT::state_t StartState, class... Transitions>
class StateMachine
{
    typedef LangT lang_t;
    typedef typename lang_t::state_t state_t;
    typedef typename lang_t::event_t event_t;

    template <event_t Event, class...TransitionsToCheck> struct StateEventDispatcher;

    // Check all defined transitions to see if one exists for the state/event combo that happened
    template <event_t Event, class Transition, class...Rest> struct StateEventDispatcher<Event, Transition, Rest...>
    {
        std::pair<bool, state_t> operator()(StateMachine& machine, const state_t currState)
        {
            if(Event == Transition::when)
                if (currState == Transition::from)
                    return {true, Transition::to};
                    
            return StateEventDispatcher<Event, Rest...>()(machine, currState);
        }
    };

    // Recursive base case; no handler
    template <event_t Event> struct StateEventDispatcher<Event>
    {
        std::pair<bool, state_t> operator()(StateMachine& machine, const state_t currState)
        {
            return {false, currState};
        }
    };

  public:
    StateMachine(const std::function<void(const state_t, const state_t, const event_t)> onStateChange, const std::function<void(const state_t, const event_t)> onNoStateChange)
    : m_onChange(onStateChange), m_onNoChange(onNoStateChange)
    {}

    template <event_t Trigger> void event()
    {
        const state_t oldState = m_state;

        bool transitionExists;
        std::tie(transitionExists, m_state) = StateEventDispatcher<Trigger, Transitions...>()(*this, oldState);

        if (transitionExists)
            m_onChange(oldState, m_state, Trigger);
        else
            m_onNoChange(oldState, Trigger);
    }

    state_t state();

  private:
    state_t m_state = StartState;
    const std::function<void(const state_t, const state_t, const event_t)> m_onChange;
    const std::function<void(const state_t, const event_t)> m_onNoChange;
};

}
