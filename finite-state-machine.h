#pragma once

#include "state-machine.h"

#include <type_traits>
#include <unordered_map>
#include <unordered_set>

namespace state_machine
{

template<class StateT, StateT From, StateT To>
struct Transition
{
    constexpr static StateT from = From;
    constexpr static StateT to = To;
};

template<class ... Transitions>
struct TransitionList
{
    typedef std::tuple<Transitions ...> types;
    constexpr static std::size_t size = std::tuple_size<types>::value;

    template <std::size_t N>
    using typeof = typename std::tuple_element<N, types>::type;
};

template<class StateT, class HashStruct>
struct TransitionLookup : private std::unordered_map<StateT, std::unordered_set<StateT, HashStruct>, HashStruct>
{
public:
    bool has(const StateT& from, const StateT& to) const
    {
        auto fromSetIt = this->find(from);
        if(fromSetIt != this->end())
            return fromSetIt->second.count(to);
        return false;
    }

    //TODO: Determine why from can't be const&
    void add(StateT from, const StateT& to)
    {
        this->operator[](from).insert(to);
    }
};

namespace _private {
//Allows hashing of enum class types
//https://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
struct _EnumClassHash
{
    template <class T>
    std::size_t operator()(const T& t) const
    {
        return static_cast<std::size_t>(t);
    }
};

template<class StateT, class HashStruct, class TList, std::size_t max = TList::size, std::size_t index = 0>
struct TransitionAdder
{
    static void addto(TransitionLookup<StateT, HashStruct>& lookup)
    {
        typedef typename TList::template typeof<index> transition;
        lookup.add(transition::from, transition::to);
        TransitionAdder<StateT, HashStruct, TList, max, index+1>::addto(lookup);
    }
};

template<class StateT, class HashStruct, class TList, std::size_t max>
struct TransitionAdder<StateT, HashStruct, TList, max, max>
{
    static void addto(TransitionLookup<StateT, HashStruct>& lookup)
    {}
};

template<class StateT, class TList, class HashStruct>
TransitionLookup<StateT, HashStruct> _buildTransitionLookup()
{
    TransitionLookup<StateT, HashStruct> lookup;
    TransitionAdder<StateT, HashStruct, TList>::addto(lookup);
    return lookup;
}
}

template<class StateT = int, 
         class ValidTransitionList = TransitionList<>, 
         class HashStruct = typename std::conditional<std::is_enum<StateT>::value, _private::_EnumClassHash, std::hash<StateT>>::type>
class FiniteStateMachine : public StateMachine<StateT>
{
    const static TransitionLookup<StateT, HashStruct> _transitions;

public:
    FiniteStateMachine(const StateT& startState, const TransitionCallback<StateT>& onAcceptedTransition = TransitionCallback<StateT>(), const TransitionCallback<StateT>& onDeniedTransition = TransitionCallback<StateT>()) : 
        StateMachine<StateT>(startState, 
                            [this](const StateT& from, const StateT& to){return _transitions.has(from, to);},
                            onAcceptedTransition,
                            onDeniedTransition){}
};

template<class StateT, class ValidTransitionList, class HashStruct>
const TransitionLookup<StateT, HashStruct> FiniteStateMachine<StateT, ValidTransitionList, HashStruct>::_transitions = _private::_buildTransitionLookup<StateT, ValidTransitionList, HashStruct>();

}