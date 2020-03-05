#include "dfa_builder.h"
#include "ptr_set.h"
#include <algorithm>
#include <map>
#include <memory>
#include <stack>

shared_ptr<PtrSet<shared_ptr<State>>> e_closure(shared_ptr<PtrSet<shared_ptr<State>>> set)
{
    stack<shared_ptr<State>> s_stack;
    for (auto state : set->get_items())
    {
        s_stack.push(state);
    }
    shared_ptr<PtrSet<shared_ptr<State>>> e_closure_set = set;

    while (!s_stack.empty())
    {
        shared_ptr<State> t = s_stack.top();
        s_stack.pop();
        for (shared_ptr<State> &u : t->get_next_e_states())
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

PtrSet<shared_ptr<PtrSet<shared_ptr<State>>>> get_unmarked_states(PtrSet<shared_ptr<PtrSet<shared_ptr<State>>>> set)
{
    PtrSet<shared_ptr<PtrSet<shared_ptr<State>>>> unmarked_states;
    for (auto &state : set.get_items())
    {
        if (!state->is_marked())
        {
            unmarked_states.add(state);
        }
    }
    return unmarked_states;
}

shared_ptr<PtrSet<shared_ptr<State>>> move_set_of_states(shared_ptr<PtrSet<shared_ptr<State>>> set, int symbol)
{
    vector<shared_ptr<State>> result_vec;
    for (auto &state : set->get_items())
    {
        auto moved_state = state->move(symbol);
        result_vec.insert(result_vec.end(), moved_state.begin(), moved_state.end());
    }
    return make_shared<PtrSet<shared_ptr<State>>>(result_vec);
}

void dfa_from_nfa(StateMachine nfa)
{
    PtrSet<shared_ptr<PtrSet<shared_ptr<State>>>> d_states;
    int name_index = 0;
    d_states.add(e_closure(
        make_shared<PtrSet<shared_ptr<State>>>(to_string(name_index), vector<shared_ptr<State>>{(nfa.start())})));
    auto symbols = nfa.get_all_input_symbols();
    vector<pair<pair<shared_ptr<PtrSet<shared_ptr<State>>>, int>, shared_ptr<PtrSet<shared_ptr<State>>>>> d_tran;

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
                auto U_in_d_states = d_states.has_item(U);
                if (U_in_d_states == nullptr)
                {
                    U->unmark();
                    name_index++;
                    U->set_name(to_string(name_index));
                    d_states.add(U);
                }
                else
                {
                    U->set_name(U_in_d_states->get_name());
                }
                d_tran.push_back(make_pair(make_pair(T, a), U));
            }
        }

        unmarked_states = get_unmarked_states(d_states);
    }
    int i = 0;
}