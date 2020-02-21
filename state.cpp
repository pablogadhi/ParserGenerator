#include "state.h"

State::State(string s_name)
{
    state_name = s_name;
    t_functions = vector<pair<int, State *>>();
}

State::State(string name, vector<pair<int, State *>> t_functions) : state_name(name), t_functions(t_functions)
{
}

void State::set_t_functions(vector<pair<int, State *>> t_funcs)
{
    t_functions = t_funcs;
}

vector<pair<int, State *>> State::get_t_functions()
{
    return t_functions;
}

void State::add_t_function(pair<int, State *> new_func)
{
    t_functions.push_back(new_func);
}

string State::name()
{
    return state_name;
}

vector<State *> State::get_next_e_states()
{
    vector<State *> e_states = vector<State *>();
    for (auto &trans : get_t_functions())
    {
        if (trans.first == -1)
        {
            e_states.push_back(trans.second);
        }
    }
    return e_states;
}

vector<State *> State::move(int symbol)
{
    vector<State *> movements;
    for (auto &trans : t_functions)
    {
        if (trans.first == symbol)
        {
            movements.push_back(trans.second);
        }
    }
    return movements;
}