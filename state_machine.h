#include "state.h"

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
    void print_machine();
    void print_t_functions(State, vector<string> &);
};
