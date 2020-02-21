#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "state.h"
#include "utils.h"

class StateMachine
{
  private:
    State *starting_state;
    State *ending_state;

  public:
    StateMachine();
    StateMachine(State *, State *);
    ~StateMachine();
    State *start();
    State *end();
    vector<State> flatten();
    void get_next_states_into_vector(vector<State> &, State);
    vector<int> get_all_input_symbols();
    void print_machine();
    void print_t_functions(State, vector<string> &);
};

#endif