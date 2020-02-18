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
};
