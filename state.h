#include <string>
#include <vector>

using namespace std;

class State
{
  private:
    vector<pair<int, State *>> t_functions;
    string name;

  public:
    State(string, vector<pair<int, State *>>);
    State(string);
    void set_t_functions(vector<pair<int, State *>>);
    vector<pair<int, State *>> get_t_functions();
    void add_t_function(pair<int, State *>);
    ~State();
};