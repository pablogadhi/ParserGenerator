#include "state_machine.h"
#include <algorithm>
#include <iostream>

StateMachine::StateMachine(shared_ptr<State> starting_state, shared_ptr<State> ending_state)
    : starting_state(starting_state), ending_state(ending_state)
{
}

StateMachine::StateMachine()
{
}

StateMachine::~StateMachine()
{
}

shared_ptr<State> StateMachine::start()
{
    return starting_state;
}

shared_ptr<State> StateMachine::end()
{
    return ending_state;
}

vector<State> StateMachine::flatten()
{
    vector<State> all_states = vector<State>();
    get_next_states_into_vector(all_states, *starting_state);
    return all_states;
}

void StateMachine::get_next_states_into_vector(vector<State> &vec, State state)
{
    if (!is_item_in_vector<State>(state, vec))
    {
        vec.push_back(state);
        for (auto &trans : state.get_t_functions())
        {
            get_next_states_into_vector(vec, *(trans.second));
        }
    }
}

vector<int> StateMachine::get_all_input_symbols()
{
    auto all_states = flatten();
    vector<int> symbols;
    for (auto &state : all_states)
    {
        for (auto &trans : state.get_t_functions())
        {
            if (!is_item_in_vector(trans.first, symbols) and trans.first != -1)
            {
                symbols.push_back(trans.first);
            }
        }
    }
    return symbols;
}

void StateMachine::print_machine()
{
    vector<State> flattend_machine = flatten();
    for (auto &state : flattend_machine)
    {
        for (auto &trans : state.get_t_functions())
        {
            string trans_name = trans.second->name();
            string trans_string;
            if (trans.first != -1)
            {
                trans_string = (char)(trans.first);
            }
            else
            {
                trans_string = "";
            }

            cout << state.name() << " {" << trans_string << "}-> " << trans_name << endl;
        }
    }
}