#pragma once

#include <functional>

namespace state_machine
{

template<class StateT>
using TransitionCallback = std::function<void(const StateT&, const StateT&)>;

template<class StateT>
using TransitionCheck = std::function<bool(const StateT&, const StateT&)>;

template<class StateT>
class StateMachine
{
    StateT _currentState;
    TransitionCheck<StateT> _checkTransition;
    TransitionCallback<StateT> _onAccept, _onDeny;

public:
    StateMachine(const StateT& startState, const TransitionCheck<StateT>& onTransitionCheck, const TransitionCallback<StateT>& onAcceptedTransition = TransitionCallback<StateT>(), const TransitionCallback<StateT>& onDeniedTransition = TransitionCallback<StateT>()) : 
        _currentState(startState),
        _checkTransition(onTransitionCheck),
        _onAccept(onAcceptedTransition),
        _onDeny(onDeniedTransition){}

    const StateT& state() const { return _currentState; }
    void transition(const StateT& newState)
    {
        if(_checkTransition(_currentState, newState))
        {
            if(_onAccept)
                _onAccept(_currentState, newState);
            _currentState = newState;
        }
        else if(_onDeny)
            _onDeny(_currentState, newState);
    }
};

}