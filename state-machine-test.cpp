#include "state-machine.h"

#include <iostream>
#include <string>
#include <random>

using namespace state_machine;
using namespace std;

mt19937_64 reng;
uniform_int_distribution<> eventChoices(0, 2);


enum class States
{
    STATE_0, STATE_1, STATE_2
};

enum class Events
{
    EVENT_0, EVENT_1, EVENT_2
};


typedef Language<States, Events> Language_t;

typedef Language_t::Transition<States::STATE_0, States::STATE_1, Events::EVENT_0> Transition_1;
typedef Language_t::Transition<States::STATE_0, States::STATE_2, Events::EVENT_1> Transition_2;
typedef Language_t::Transition<States::STATE_1, States::STATE_2, Events::EVENT_1> Transition_3;
typedef Language_t::Transition<States::STATE_2, States::STATE_0, Events::EVENT_2> Transition_4;

typedef StateMachine<Language_t, States::STATE_0, Transition_1, Transition_2, Transition_3, Transition_4> StateMachine_t;

std::ostream& operator << (std::ostream& out, const States s)
{
    return out << static_cast<int>(s);
}

std::ostream& operator << (std::ostream& out, const Events e)
{
    return out << static_cast<int>(e);
}

void change(const States s1, const States s2, const Events e)
{
    std::cout << s1 << " -> " << s2 << " :: " << e << std::endl;
}

void noChange(const States s, const Events e)
{
    std::cout << s << " :: " << e << std::endl;
}

int main()
{
    StateMachine_t machine(&change, &noChange);

    for(int i=0; i<100; i++)
    {
        switch(eventChoices(reng))
        {
            case 0:
                machine.event<Events::EVENT_0>();
            break;
            case 1:
                machine.event<Events::EVENT_1>();
            break;
            case 2:
                machine.event<Events::EVENT_2>();
            break;
        }
    }

    return 0;
}
