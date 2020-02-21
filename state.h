#ifndef STATE_H
#define STATE_H

#include <string>
#include <vector>

using namespace std;

class State
{
  private:
    vector<pair<int, State *>> t_functions;
    string state_name;

  public:
    State(string, vector<pair<int, State *>>);
    State(string);
    void set_t_functions(vector<pair<int, State *>>);
    vector<pair<int, State *>> get_t_functions();
    void add_t_function(pair<int, State *>);
    string name();
    vector<State *> get_next_e_states();
    vector<State *> move(int);
    bool operator==(State &s)
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