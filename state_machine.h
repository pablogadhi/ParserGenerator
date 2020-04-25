#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "state.h"

template <class MachineType> class StateMachine
{
  protected:
    shared_ptr<State> starting_state;
    shared_ptr<State> ending_state;

  public:
    StateMachine();
    StateMachine(shared_ptr<State>, shared_ptr<State>);
    ~StateMachine();
    shared_ptr<State> start();
    shared_ptr<State> end();
    vector<shared_ptr<State>> flatten();
    void get_next_states_into_vector(vector<shared_ptr<State>> &, shared_ptr<State>);
    vector<int> get_all_input_symbols();
    void print_machine();
    void draw_machine(string file_name);
    MachineType make_copy(int &);
    Set<State> accepting_states();
};

class DFA : public StateMachine<DFA>
{
  private:
    State current_state;

  public:
    DFA();
    DFA(shared_ptr<State>, shared_ptr<State>);
    void move(char);
    State peek_move(char);
    void reset_movements();
    State current();
};

class NFA : public StateMachine<NFA>
{
  private:
    Set<State> current_states;

  public:
    NFA();
    NFA(shared_ptr<State>, shared_ptr<State>);
    void move(char);
    void reset_movements();
    Set<State> current();
};

Set<State> e_closure(Set<State>);
Set<State> move_set_of_states(Set<State>, int);

template class StateMachine<NFA>;
template class StateMachine<DFA>;

#endif