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
        if (*state == *(this->ending_state))
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
            agedge(graph, first_node, second_node, (char *)edge_label.c_str(), TRUE);
        }
    }
    agsafeset(graph, (char *)"rankdir", (char *)"LR", (char *)"LR");

    gvLayout(gvc, graph, "dot");
    gvRenderFilename(gvc, graph, "dot", file_name.c_str());
    gvFreeLayout(gvc, graph);
    agclose(graph);
}