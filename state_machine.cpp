#include "state_machine.h"
#include <algorithm>
#include <iostream>
#include <map>
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
            string trans_name = trans.second->name();
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
    for (auto &state : flattend_machine)
    {
        char *c_name = new char[state->name().length() + 1];
        strcpy(c_name, state->name().c_str());
        auto first_node = agnode(graph, c_name, TRUE);
        if (state->type() == last)
        {
            agsafeset(first_node, (char *)"shape", (char *)"doublecircle", (char *)"doublecircle");
        }
        else
        {
            agsafeset(first_node, (char *)"shape", (char *)"circle", (char *)"circle");
        }
        delete[] c_name;
        for (auto &trans : state->get_t_functions())
        {
            c_name = new char[trans.second->name().length() + 1];
            strcpy(c_name, trans.second->name().c_str());
            auto second_node = agnode(graph, c_name, TRUE);
            delete[] c_name;
            string edge_label;
            edge_label.push_back((char)trans.first);
            auto edge = agedge(graph, first_node, second_node, (char *)edge_label.c_str(), TRUE);
            agsafeset(edge, (char *)"label", (char *)edge_label.c_str(), (char *)edge_label.c_str());
        }
    }
    agsafeset(graph, (char *)"rankdir", (char *)"LR", (char *)"LR");

    gvLayout(gvc, graph, "dot");
    gvRenderFilename(gvc, graph, "dot", file_name.c_str());
    gvFreeLayout(gvc, graph);
    agclose(graph);
}

StateMachine StateMachine::make_copy(int &name_index)
{
    vector<shared_ptr<State>> copied_states;
    vector<shared_ptr<State>> original_states = flatten();
    map<string, shared_ptr<State>> name_map;
    for (auto &state : original_states)
    {
        if (name_map.find(state->name()) == name_map.end())
        {
            name_index++;
            auto new_state_name = to_string(name_index);
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