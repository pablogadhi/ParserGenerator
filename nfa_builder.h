#ifndef NFA_BUILDER_H
#define NFA_BUILDER_H

#include "state_machine.h"

NFA nfa_from_or(NFA &, NFA &, int &last_index);
NFA nfa_from_concat(NFA &, NFA &);
NFA nfa_from_kleene(NFA &, int &last_index);
NFA nfa_from_transition(int, int &last_index);

#endif