#include "dfa_builder.h"
#include "set.h"
#include "utils.h"
#include <algorithm>
#include <map>
#include <stack>

Set<State *> *e_closure(Set<State *> *set)
{
    stack<State *> s_stack;
    for (auto state : set->get_items())
    {
        s_stack.push(state);
    }
    Set<State *> *e_closure_set = set;

    while (!s_stack.empty())
    {
        State *t = s_stack.top();
        s_stack.pop();
        for (State *&u : t->get_next_e_states())
        {
            if (!e_closure_set->has_item(u))
            {
                e_closure_set->add(u);
                s_stack.push(u);
            }
        }
    }

    return e_closure_set;
}

Set<Set<State *> *> get_unmarked_states(Set<Set<State *> *> set)
{
    Set<Set<State *> *> unmarked_states;
    for (auto &state : set.get_items())
    {
        if (!state->is_marked())
        {
            unmarked_states.add(state);
        }
    }
    return unmarked_states;
}

Set<State *> *move_set_of_states(Set<State *> *set, int symbol)
{
    vector<State *> result_vec;
    for (auto &state : set->get_items())
    {
        auto moved_state = state->move(symbol);
        result_vec.insert(result_vec.end(), moved_state.begin(), moved_state.end());
    }
    return new Set<State *>(result_vec);
}

void dfa_from_nfa(StateMachine nfa)
{
    Set<Set<State *> *> d_states;
    int name_index = 0;
    d_states.add(e_closure(new Set<State *>(to_string(name_index), vector<State *>{(nfa.start())})));
    auto symbols = nfa.get_all_input_symbols();
    vector<pair<pair<Set<State *> *, int>, Set<State *> *>> d_tran;

    auto unmarked_states = get_unmarked_states(d_states);
    while (!unmarked_states.empty())
    {
        auto T = unmarked_states.get_items()[0];
        T->mark();

        for (auto &a : symbols)
        {
            auto movements = move_set_of_states(T, a);
            if (!movements->empty())
            {
                auto U = e_closure(movements);
                if (!d_states.has_item(U))
                {
                    U->unmark();
                    name_index++;
                    U->set_name(to_string(name_index));
                    d_states.add(U);
                }
                d_tran.push_back(make_pair(make_pair(T, a), U));
            }
        }

        unmarked_states = get_unmarked_states(d_states);
    }
    int i = 0;
}