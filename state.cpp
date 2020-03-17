#include "state.h"

State::State()
{
}

State::State(string s_name)
{
    state_name = s_name;
    t_functions = vector<pair<int, shared_ptr<State>>>();
}

State::State(string name, vector<pair<int, shared_ptr<State>>> t_functions) : state_name(name), t_functions(t_functions)
{
}

void State::set_t_functions(vector<pair<int, shared_ptr<State>>> t_funcs)
{
    t_functions = t_funcs;
}

vector<pair<int, shared_ptr<State>>> State::get_t_functions()
{
    return t_functions;
}

void State::add_t_function(pair<int, shared_ptr<State>> new_func)
{
    t_functions.push_back(new_func);
}

void State::remove_t_function_at(int index)
{
    t_functions.erase(t_functions.begin() + index);
}

string State::name() const
{
    return state_name;
}

Set<State> State::get_next_e_states()
{
    Set<State> e_states;
    for (auto &trans : get_t_functions())
    {
        if (trans.first == 36)
        {
            e_states.add(*trans.second);
        }
    }
    return e_states;
}

Set<State> State::move(int symbol)
{
    Set<State> movements;
    for (auto &trans : t_functions)
    {
        if (trans.first == symbol)
        {
            movements.add(*trans.second);
        }
    }
    return movements;
}