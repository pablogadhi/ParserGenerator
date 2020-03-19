#include "state_machine.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <stack>
extern "C"
{
#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>
}

StateMachine::StateMachine(shared_ptr<State> starting_state, shared_ptr<State> ending_state)
    : starting_state(starting_state), ending_state(ending_state)
{
}

StateMachine::StateMachine()
{
}

StateMachine::~StateMachine()
{
}

shared_ptr<State> StateMachine::start()
{
    return starting_state;
}

shared_ptr<State> StateMachine::end()
{
    return ending_state;
}

vector<shared_ptr<State>> StateMachine::flatten()
{
    vector<shared_ptr<State>> all_states = vector<shared_ptr<State>>();
    get_next_states_into_vector(all_states, starting_state);
    return all_states;
}

void StateMachine::get_next_states_into_vector(vector<shared_ptr<State>> &vec, shared_ptr<State> state)
{
    if (!is_item_ptr_in_vector<shared_ptr<State>>(state, vec))
    {
        vec.push_back(state);
        for (auto &trans : state->get_t_functions())
        {
            get_next_states_into_vector(vec, trans.second);
        }
    }
}

vector<int> StateMachine::get_all_input_symbols()
{
    auto all_states = flatten();
    vector<int> symbols;
    for (auto &state : all_states)
    {
        for (auto &trans : state->get_t_functions())
        {
            if (!is_item_in_vector(trans.first, symbols) and trans.first != 36)
            {
                symbols.push_back(trans.first);
            }
        }
    }
    return symbols;
}

void StateMachine::print_machine()
{
    vector<shared_ptr<State>> flattend_machine = flatten();
    for (auto &state : flattend_machine)
    {
        for (auto &trans : state->get_t_functions())
        {
            int trans_name = trans.second->name();
            char trans_c = (char)(trans.first);

            cout << state->name() << " {" << trans_c << "}-> " << trans_name << endl;
        }
    }
}

void StateMachine::draw_machine(string file_name)
{
    Agraph_t *graph = agopen((char *)"StateMachine", Agdirected, 0);
    GVC_t *gvc = gvContext();

    vector<shared_ptr<State>> flattend_machine = flatten();

    if (flattend_machine.size() == 1)
    {
        print_machine();
    }

    for (auto &state : flattend_machine)
    {
        char *c_name = new char[to_string(state->name()).length() + 1];
        strcpy(c_name, to_string(state->name()).c_str());
        auto first_node = agnode(graph, c_name, TRUE);

        if (state->is_accepting())
        {
            agsafeset(first_node, (char *)"shape", (char *)"doublecircle", (char *)"circle");
        }
        else
        {
            agsafeset(first_node, (char *)"shape", (char *)"circle", (char *)"circle");
        }

        if (*starting_state == *state)
        {
            agsafeset(first_node, (char *)"color", (char *)"green", (char *)"black");
        }

        delete[] c_name;
        for (auto &trans : state->get_t_functions())
        {
            c_name = new char[to_string(trans.second->name()).length() + 1];
            strcpy(c_name, to_string(trans.second->name()).c_str());
            auto second_node = agnode(graph, c_name, TRUE);
            delete[] c_name;
            // TODO Calculate string length correctly
            char *edge_label = new char[10];
            string edge_string = "";
            edge_string.push_back((char)trans.first);
            strcpy(edge_label, edge_string.c_str());
            auto edge = agedge(graph, first_node, second_node, edge_label, TRUE);
            agsafeset(edge, (char *)"label", edge_label, (char *)"");
            delete[] edge_label;
        }
    }
    agsafeset(graph, (char *)"rankdir", (char *)"LR", (char *)"LR");

    gvLayout(gvc, graph, "dot");
    gvRenderFilename(gvc, graph, "dot", (file_name + ".dot").c_str());
    gvFreeLayout(gvc, graph);
    agclose(graph);

    string render_command = "dot -Tpng -Gdpi=300 " + file_name + ".dot" + " -o " + file_name + ".png";
    system(render_command.c_str());
}

StateMachine StateMachine::make_copy(int &name_index)
{
    vector<shared_ptr<State>> copied_states;
    vector<shared_ptr<State>> original_states = flatten();
    map<int, shared_ptr<State>> name_map;
    for (auto &state : original_states)
    {
        if (name_map.find(state->name()) == name_map.end())
        {
            name_index++;
            auto new_state_name = name_index;
            auto new_state = make_shared<State>(State(new_state_name));
            copied_states.push_back(new_state);
            name_map[state->name()] = new_state;
        }
    }

    for (auto &state : original_states)
    {
        auto new_state = name_map[state->name()];
        for (auto &trans : state->get_t_functions())
        {
            auto state_to_trans = name_map[trans.second->name()];
            new_state->add_t_function(make_pair(trans.first, state_to_trans));
        }
    }

    return StateMachine(copied_states[0], copied_states[copied_states.size() - 1]);
}

Set<State> StateMachine::accepting_states()
{
    Set<State> set;
    for (auto &state : flatten())
    {
        if (state->is_accepting())
        {
            set.add(*state);
        }
    }
    return set;
}

Set<State> e_closure(Set<State> set)
{
    stack<State> s_stack;
    for (auto state : set)
    {
        s_stack.push(state);
    }
    Set<State> e_closure_set = set;

    while (!s_stack.empty())
    {
        State t = s_stack.top();
        s_stack.pop();
        for (State &u : t.next_e_states())
        {
            if (e_closure_set.has_item(u) == -1)
            {
                e_closure_set.add(u);
                s_stack.push(u);
            }
        }
    }

    return e_closure_set;
}

Set<State> move_set_of_states(Set<State> set, int symbol)
{
    vector<State> result_vec;
    for (auto &state : set)
    {
        auto moved_state = state.move(symbol);
        result_vec.insert(result_vec.end(), moved_state.begin(), moved_state.end());
    }
    return Set(result_vec);
}