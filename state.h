#ifndef STATE_H
#define STATE_H

#include "set.h"
#include <memory>
#include <string>
#include <vector>

using namespace std;

class State
{
  private:
    vector<pair<int, shared_ptr<State>>> t_functions;
    int s_name = -1;
    bool s_is_accepting = false;
    string s_ref_name; // Name of the parent StateMachine

  public:
    State(int, vector<pair<int, shared_ptr<State>>>);
    State(int);
    State();
    void set_t_functions(vector<pair<int, shared_ptr<State>>>);
    vector<pair<int, shared_ptr<State>>> get_t_functions();
    void add_t_function(pair<int, shared_ptr<State>>);
    void remove_t_function_at(int);
    int name() const;
    void set_as_accepting(bool);
    bool is_accepting();
    void set_reference_name(string);
    string reference_name();
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