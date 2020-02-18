#include "state_machine.h"

StateMachine nfa_from_or(StateMachine &, StateMachine &, int &last_index);
StateMachine nfa_from_concat(StateMachine &, StateMachine &);
StateMachine nfa_from_kleene(StateMachine &, int &last_index);
StateMachine nfa_from_transition(int, int &last_index);