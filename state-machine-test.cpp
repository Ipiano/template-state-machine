#include "state-machine.h"

#include <iostream>
#include <string>
#include <random>

using namespace state_machine;
using namespace std;

mt19937_64 reng;
uniform_int_distribution<> stateChoices(0, 5);

enum class State
{
    Start,
    Ready,
    Working,
    Failed,
    Succeeded,
    Resetting
};

string StateNames[] = {"Start", "Ready", "Working", "Failed", "Succeeded", "Resetting"};
ostream& operator << (ostream& out, const State& s)
{
    return out << StateNames[static_cast<size_t>(s)];
}

typedef State state_type;

typedef StateMachine<state_type,
            TransitionList<
                Transition<state_type, State::Start, State::Ready>,
                Transition<state_type, State::Ready, State::Working>,
                Transition<state_type, State::Ready, State::Resetting>,
                Transition<state_type, State::Working, State::Failed>,
                Transition<state_type, State::Working, State::Succeeded>,
                Transition<state_type, State::Failed, State::Resetting>,
                Transition<state_type, State::Succeeded, State::Resetting>,
                Transition<state_type, State::Resetting, State::Ready>
            >
        > StateMachineType;

int main()
{
    StateMachineType sm(State::Start, 
    [](const state_type& fromState, const state_type& toState)
    {cout << "Transition " << fromState << " -> " << toState << endl;},

    [](const state_type& fromState, const state_type& toState)
    {cout << "Failed Transition " << fromState << " -> " << toState << endl;});

    for(int i=0; i<100; i++)
    {
        sm.transition(static_cast<state_type>(stateChoices(reng)));
    }

    return 0;
}