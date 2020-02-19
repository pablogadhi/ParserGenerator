#include "state_machine.h"
#include <algorithm>
#include <iostream>

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

void StateMachine::print_machine()
{
    vector<string> printed_states = vector<string>();
    print_t_functions(*starting_state, printed_states);
}

void StateMachine::print_t_functions(State state, vector<string> &printed_states)
{

    if (find(printed_states.begin(), printed_states.end(), state.name()) == printed_states.end())
    {
        printed_states.push_back(state.name());
        auto t_funcs = state.get_t_functions();
        for (auto i = t_funcs.begin(); i != t_funcs.end(); i++)
        {
            string trans_name = i->second->name();
            char trans_char;
            if (i->first != -1)
            {
                trans_char = (char)(i->first);
            }

            cout << state.name() << " {" << trans_char << "}-> " << trans_name << endl;
            print_t_functions(*(i->second), printed_states);
        }
    }
}