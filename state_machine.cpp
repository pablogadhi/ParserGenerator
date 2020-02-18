#include "state_machine.h"

StateMachine::StateMachine(State *starting_state, State *ending_state)
    : starting_state(starting_state), ending_state(ending_state)
{
}

StateMachine::StateMachine()
{
}

StateMachine::~StateMachine()
{
}

State *StateMachine::start()
{
    return starting_state;
}

State *StateMachine::end()
{
    return ending_state;
}