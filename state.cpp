#include "state.h"

State::State()
{
}

State::State(int name)
{
    s_name = name;
    t_functions = vector<pair<int, shared_ptr<State>>>();
}

State::State(int name, vector<pair<int, shared_ptr<State>>> t_functions) : s_name(name), t_functions(t_functions)
{
}

State::State(int name, bool accepting, string ref) : s_name(name), s_is_accepting(accepting), s_ref_name(ref)
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

int State::name() const
{
    return s_name;
}

void State::set_as_accepting(bool accepting)
{
    s_is_accepting = accepting;
}

bool State::is_accepting()
{
    return s_is_accepting;
}

void State::set_reference_name(string name)
{
    s_ref_name = name;
}

string State::reference_name()
{
    return s_ref_name;
}

Set<State> State::next_e_states()
{
    Set<State> e_states;
    for (auto &trans : get_t_functions())
    {
        if (trans.first == 1)
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