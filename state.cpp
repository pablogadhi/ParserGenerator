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

State::~State()
{
}