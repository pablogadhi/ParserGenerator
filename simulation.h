#ifndef SIMULATION_H
#define SIMULATION_H

#include "state_machine.h"
#include <string>

bool simulate_nfa(string, NFA &);
bool simulate_dfa(string, DFA &);

#endif