#ifndef STATE_H
#define STATE_H

#include "set.h"
#include <memory>
#include <string>
#include <vector>

using namespace std;

enum StateType
{
    first,
    last,
    normal
};

class State
{
  private:
    vector<pair<int, shared_ptr<State>>> t_functions;
    string state_name = "";
    StateType type = normal;

  public:
    State(string, vector<pair<int, shared_ptr<State>>>);
    State(string);
    State();
    void set_t_functions(vector<pair<int, shared_ptr<State>>>);
    vector<pair<int, shared_ptr<State>>> get_t_functions();
    void add_t_function(pair<int, shared_ptr<State>>);
    void remove_t_function_at(int);
    string name() const;
    Set<State> get_next_e_states();
    Set<State> move(int);
    bool operator==(const State &s)
    {
        if (state_name == s.name())
        {
            return true;
        }
        return false;
    }
    bool operator<(State &s)
    {
        if (stoi(name()) < stoi(s.name()))
        {
            return true;
        }
        return false;
    }
};

#endif