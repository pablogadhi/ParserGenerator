#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "state.h"
#include "utils.h"

class StateMachine
{
  private:
    shared_ptr<State> starting_state;
    shared_ptr<State> ending_state;

  public:
    StateMachine();
    StateMachine(shared_ptr<State>, shared_ptr<State>);
    // StateMachine(StateMachine &);
    ~StateMachine();
    shared_ptr<State> start();
    shared_ptr<State> end();
    vector<shared_ptr<State>> flatten();
    void get_next_states_into_vector(vector<shared_ptr<State>> &, shared_ptr<State>);
    vector<int> get_all_input_symbols();
    void print_machine();
    void draw_machine(string file_name);
};

#endif