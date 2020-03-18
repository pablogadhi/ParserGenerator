#ifndef STATE_H
#define STATE_H

#include "set.h"
#include <memory>
#include <string>
#include <vector>

using namespace std;

enum state_type
{
    first,
    last,
    normal
};

class State
{
  private:
    vector<pair<int, shared_ptr<State>>> t_functions;
    int s_name = -1;
    state_type s_type = normal;

  public:
    State(int, vector<pair<int, shared_ptr<State>>>);
    State(int);
    State();
    void set_t_functions(vector<pair<int, shared_ptr<State>>>);
    vector<pair<int, shared_ptr<State>>> get_t_functions();
    void add_t_function(pair<int, shared_ptr<State>>);
    void remove_t_function_at(int);
    int name() const;
    void change_type(state_type);
    state_type type();
    Set<State> next_e_states();
    Set<State> move(int);
    bool operator==(const State &s)
    {
        if (s_name == s.name())
        {
            return true;
        }
        return false;
    }
    bool operator<(State &s)
    {
        if (name() < s.name())
        {
            return true;
        }
        return false;
    }
};

#endif