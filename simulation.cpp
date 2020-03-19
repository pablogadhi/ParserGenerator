#include "simulation.h"

bool simulate_nfa(string str, StateMachine &nfa)
{
    auto S = e_closure(Set<State>(vector<State>{*nfa.start()}));
    for (auto &c : str)
    {
        S = e_closure(move_set_of_states(S, (int)c));
    }
    if (intersec_between_sets<State>(S, nfa.accepting_states()).size() != 0)
    {
        return true;
    }
    return false;
}

bool simulate_dfa(string str, StateMachine &dfa)
{
    auto current_state = *dfa.start();
    for (auto &c : str)
    {
        auto movement = current_state.move((int)c);
        if (movement.size() != 1)
        {
            return false;
        }
        else
        {
            current_state = movement[0];
        }
    }
    if (current_state.type() == accepting)
    {
        return true;
    }
    return false;
}