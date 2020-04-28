#include "state_machine.h"
#include "utils.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <stack>
extern "C"
{
#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>
}

template <class MachineType>
StateMachine<MachineType>::StateMachine(shared_ptr<State> starting_state, shared_ptr<State> ending_state)
    : starting_state(starting_state), ending_state(ending_state)
{
}

template <class MachineType> StateMachine<MachineType>::StateMachine()
{
}

template <class MachineType> StateMachine<MachineType>::~StateMachine()
{
}

template <class MachineType> shared_ptr<State> StateMachine<MachineType>::start()
{
    return starting_state;
}

template <class MachineType> shared_ptr<State> StateMachine<MachineType>::end()
{
    return ending_state;
}

template <class MachineType> vector<shared_ptr<State>> StateMachine<MachineType>::flatten()
{
    vector<shared_ptr<State>> all_states = vector<shared_ptr<State>>();
    get_next_states_into_vector(all_states, starting_state);
    return all_states;
}

template <class MachineType>
void StateMachine<MachineType>::get_next_states_into_vector(vector<shared_ptr<State>> &vec, shared_ptr<State> state)
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

template <class MachineType> vector<int> StateMachine<MachineType>::get_all_input_symbols()
{
    auto all_states = flatten();
    vector<int> symbols;
    for (auto &state : all_states)
    {
        for (auto &trans : state->get_t_functions())
        {
            if (!is_item_in_vector(trans.first, symbols) and trans.first != 1)
            {
                symbols.push_back(trans.first);
            }
        }
    }
    return symbols;
}

template <class MachineType> void StateMachine<MachineType>::print_machine()
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

template <class MachineType> void StateMachine<MachineType>::draw_machine(string file_name)
{
    Agraph_t *graph = agopen((char *)"StateMachine", Agdirected, 0);
    GVC_t *gvc = gvContext();

    vector<shared_ptr<State>> flattend_machine = flatten();

    // if (flattend_machine.size() == 1)
    // {
    //     print_machine();
    // }

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
        unordered_map<string, string> transtions;
        for (auto &trans : state->get_t_functions())
        {
            auto name = to_string(trans.second->name());
            auto edge_lbl = transtions[name];

            if (trans.first != 34)
            {
                transtions[name] = edge_lbl + char_to_str((char)trans.first) + ",";
            }
            else
            {
                transtions[name] = edge_lbl + "{quote}" + ",";
            }
            if (state->name() == 11 && trans.second->name() == 13)
            {
                int i = 0;
            }
        }

        for (auto const &[key, val] : transtions)
        {
            c_name = new char[key.length() + 1];
            strcpy(c_name, key.c_str());
            auto second_node = agnode(graph, c_name, TRUE);
            delete[] c_name;

            string edge_str;
            if (val.size() > 15)
            {
                auto first_index = val.find_first_of(",");
                auto last_index = val.find_last_of(",", val.size() - 2);
                edge_str = val.substr(0, first_index) + ".." + val.substr(last_index + 1, val.size() - last_index);
            }
            else
            {
                edge_str = val;
            }
            char *edge_label = new char[edge_str.length() + 1];
            strcpy(edge_label, edge_str.c_str());
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

template <class MachineType> MachineType StateMachine<MachineType>::make_copy(int &name_index)
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

    return MachineType(copied_states[0], copied_states[copied_states.size() - 1]);
}

template <class MachineType> Set<State> StateMachine<MachineType>::accepting_states()
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

DFA::DFA()
{
}

DFA::DFA(shared_ptr<State> start_ptr, shared_ptr<State> end_ptr)
{
    starting_state = start_ptr;
    ending_state = end_ptr;
    reset_movements();
}

void DFA::move(char c)
{
    current_state = peek_move(c);
}

State DFA::peek_move(char c)
{
    auto movement = current_state.move((int)c);
    if (movement.size() != 1)
    {
        // throw std::logic_error("Lexical Error! Failed to move forward!");
        auto empty_state = State();
        empty_state.set_as_accepting(false);
        return empty_state;
    }
    return movement[0];
}

void DFA::reset_movements()
{
    current_state = *starting_state;
}

State DFA::current()
{
    return current_state;
}

NFA::NFA()
{
}

NFA::NFA(shared_ptr<State> start_ptr, shared_ptr<State> end_ptr)
{
    starting_state = start_ptr;
    ending_state = end_ptr;
    reset_movements();
}

void NFA::move(char c)
{
    auto movement = move_set_of_states(current_states, (int)c);
    auto e_c = e_closure(movement);
    current_states = e_c;
}

void NFA::reset_movements()
{
    current_states = e_closure(Set<State>{*starting_state});
}

Set<State> NFA::current()
{
    return current_states;
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