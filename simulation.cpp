#include "simulation.h"

bool simulate_nfa(string str, NFA &nfa)
{
    nfa.reset_movements();
    for (auto &c : str)
    {
        nfa.move(c);
    }

    if (intersec_between_sets<State>(nfa.current(), nfa.accepting_states()).size() != 0)
    {
        return true;
    }
    return false;
}

bool simulate_dfa(string str, DFA &dfa)
{
    dfa.reset_movements();
    for (auto &c : str)
    {
        dfa.move(c);
    }

    if (dfa.current().is_accepting())
    {
        return true;
    }
    return false;
}