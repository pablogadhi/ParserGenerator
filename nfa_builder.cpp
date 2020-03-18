#include "nfa_builder.h"

StateMachine nfa_from_or(StateMachine &a, StateMachine &b, int &last_index)
{
    shared_ptr<State> first_state = make_shared<State>(to_string(last_index + 1));
    shared_ptr<State> last_state = make_shared<State>(to_string(last_index + 2));
    first_state->set_t_functions(
        vector<pair<int, shared_ptr<State>>>{make_pair(36, a.start()), make_pair(36, b.start())});
    a.end()->set_t_functions(vector<pair<int, shared_ptr<State>>>{make_pair(36, last_state)});
    b.end()->set_t_functions(vector<pair<int, shared_ptr<State>>>{make_pair(36, last_state)});
    last_index += 2;
    last_state->change_type(last);
    a.end()->change_type(normal);
    b.end()->change_type(normal);
    return StateMachine(first_state, last_state);
}

StateMachine nfa_from_concat(StateMachine &a, StateMachine &b)
{
    for (auto &state : a.flatten())
    {
        auto t_funcs = state->get_t_functions();
        for (int i = 0; i < t_funcs.size(); i++)
        {
            if (*(t_funcs[i].second) == *a.end())
            {
                auto symb = t_funcs[i].first;
                state->remove_t_function_at(i);
                state->add_t_function(make_pair(symb, b.start()));
            }
        }
    }
    return StateMachine(a.start(), b.end());
}

StateMachine nfa_from_kleene(StateMachine &a, int &last_index)
{
    shared_ptr<State> first_state = make_shared<State>(to_string(last_index + 1));
    shared_ptr<State> last_state = make_shared<State>(to_string(last_index + 2));
    first_state->set_t_functions(
        vector<pair<int, shared_ptr<State>>>{make_pair(36, a.start()), make_pair(36, last_state)});
    a.end()->add_t_function(make_pair(36, a.start()));
    a.end()->add_t_function(make_pair(36, last_state));
    last_index += 2;
    last_state->change_type(last);
    a.end()->change_type(normal);
    return StateMachine(first_state, last_state);
}

StateMachine nfa_from_transition(int transition, int &last_index)
{
    shared_ptr<State> last_state = make_shared<State>(to_string(last_index + 2));
    string first_name_string = to_string(last_index + 1);
    shared_ptr<State> first_state =
        make_shared<State>(first_name_string, vector<pair<int, shared_ptr<State>>>{make_pair(transition, last_state)});
    last_index += 2;
    last_state->change_type(last);
    return StateMachine(first_state, last_state);
}