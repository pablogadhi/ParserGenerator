#include "dfa_builder.h"
#include "ptr_set.h"
#include <algorithm>
#include <map>
#include <memory>
#include <stack>

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

Set<Set<State>> get_unmarked_states(Set<Set<State>> set)
{
    Set<Set<State>> unmarked_states;
    for (auto &state : set)
    {
        if (!state.is_marked())
        {
            unmarked_states.add(state);
        }
    }
    return unmarked_states;
}

Set<Set<State>> mark_state(Set<Set<State>> set, Set<State> state_to_mark)
{
    auto set_with_marked_state = set;
    auto item_index = set_with_marked_state.has_item(state_to_mark);
    set_with_marked_state[item_index].mark();
    return set_with_marked_state;
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

bool is_set_acceptance(Set<State> set)
{
    for (auto &state : set)
    {
        if (state.type() == last)
        {
            return true;
        }
    }
    return false;
}

StateMachine machine_from_transitions(vector<pair<pair<Set<State>, int>, Set<State>>> d_tran)
{
    Set<State> created_states;
    vector<shared_ptr<State>> state_ptrs;
    for (auto &trans : d_tran)
    {
        Set<State> states_in_tran[2] = {trans.first.first, trans.second};
        int indices[2];
        for (int i = 0; i < 2; i++)
        {
            auto new_state = State(states_in_tran[i].get_name());
            auto item_index = created_states.has_item(new_state);
            if (item_index == -1)
            {
                if (is_set_acceptance(states_in_tran[i]))
                {
                    new_state.change_type(last);
                }
                created_states.add(new_state);
                indices[i] = created_states.size() - 1;
                state_ptrs.push_back(make_shared<State>(created_states[indices[i]]));
            }
            else
            {
                indices[i] = item_index;
            }
        }

        state_ptrs[indices[0]]->add_t_function(make_pair(trans.first.second, state_ptrs[indices[1]]));
    }

    return StateMachine(state_ptrs[0], state_ptrs[state_ptrs.size() - 1]);
}

StateMachine dfa_from_nfa(StateMachine nfa)
{
    Set<Set<State>> d_states;
    int name_index = 0;
    d_states.add(e_closure(Set(to_string(name_index), vector<State>{(*nfa.start())})));
    auto symbols = nfa.get_all_input_symbols();
    vector<pair<pair<Set<State>, int>, Set<State>>> d_tran;

    auto unmarked_states = get_unmarked_states(d_states);
    while (!unmarked_states.empty())
    {
        auto T = unmarked_states[0];
        d_states = mark_state(d_states, T);

        for (auto &a : symbols)
        {
            auto movements = move_set_of_states(T, a);
            if (!movements.empty())
            {
                auto U = e_closure(movements);
                auto U_index = d_states.has_item(U);
                if (U_index == -1)
                {
                    name_index++;
                    U.set_name(to_string(name_index));
                    d_states.add(U);
                }
                else
                {
                    U.set_name(d_states[U_index].get_name());
                }
                d_tran.push_back(make_pair(make_pair(T, a), U));
            }
        }

        unmarked_states = get_unmarked_states(d_states);
    }

    return machine_from_transitions(d_tran);
}

// void nullable(TreeNode node)
// {
//     bool is_nullable;
//     switch (node.symbol())
//     {
//     case 36:
//         is_nullable = true;
//         break;
//     case 124:
//         nullable(*node.left());
//         nullable(node.right());
//         is_nullable = any_cast<bool>(node.left()->get_info()["nullable"]) ||
//                       any_cast<bool>(node.right()->get_info()["nullable"]);
//         break;
//     case 46:
//         nullable(node.left());
//         nullable(node.right());
//         is_nullable = any_cast<bool>(node.left()->get_info()["nullable"]) &&
//                       any_cast<bool>(node.right()->get_info()["nullable"]);
//         break;
//     case 42:
//         is_nullable = true;
//         break;
//     default:
//         is_nullable = false;
//         break;
//     }
//     node.add_info_entry("nullable", is_nullable);
// }

// Set<TreeNode> firstpos_or(TreeNode node_1, TreeNode node_2)
// {

//     return set_union<Set<TreeNode>, TreeNode>(any_cast<Set<TreeNode>>(node_1->get_info()["firstpos"]),
//                                               any_cast<Set<TreeNode>>(node_2->get_info()["firstpos"]));
// }

// void firstpos(TreeNode node, string info_entry_key)
// {
//     Set<TreeNode> result;
//     switch (node->symbol())
//     {
//     case 36:
//         break;
//     case 124:
//         firstpos(node->left(), info_entry_key);
//         firstpos(node->right(), info_entry_key);
//         result = firstpos_or(node->left(), node->right());
//         break;
//     case 46:
//         if (any_cast<bool>(node->left()->get_info()["nullable"]))
//         {
//             firstpos(node->left(), info_entry_key);
//             firstpos(node->right(), info_entry_key);
//             result = firstpos_or(node->left(), node->right());
//         }
//         else
//         {
//             firstpos(node->left(), info_entry_key);
//             result = any_cast<Set<TreeNode>>(node->left()->get_info()["firstpos"]);
//         }
//         break;
//     case 42:
//         firstpos(node->left(), info_entry_key);
//         result = any_cast<Set<TreeNode>>(node->left()->get_info()["firstpos"]);
//         break;
//     default:
//         result.add(node);
//         break;
//     }
//     node->add_info_entry(info_entry_key, result);
// }

// Set<TreeNode> lastpos_or(TreeNode node_1, TreeNode node_2)
// {

//     return set_union<Set<TreeNode>, TreeNode>(any_cast<Set<TreeNode>>(node_1->get_info()["lastpos"]),
//                                               any_cast<Set<TreeNode>>(node_2->get_info()["lastpos"]));
// }

// void lastpos(TreeNode node)
// {
//     if (node->symbol() == 46)
//     {
//         if (any_cast<bool>(node->right()->get_info()["nullable"]))
//         {
//             lastpos(node->left());
//             lastpos(node->right());
//             node->add_info_entry("lastpos", lastpos_or(node->left(), node->right()));
//         }
//         else
//         {
//             lastpos(node->right());
//         }
//     }
//     else
//     {
//         firstpos(node, "lastpos");
//     }
// }

// void followpos(TreeNode node)
// {
//     if (node->symbol() == 46)
//     {
//         for (auto &node : any_cast<Set<TreeNode>>(node->left()->get_info()["lastpos"]))
//         {
//             node->add_info_entry("followpos", node->right()->get_info()["lastpos"]);
//         }
//     }
//     else if (node->symbol() == 42)
//     {
//         for (auto &node : any_cast<Set<TreeNode>>(node->get_info()["lastpos"]))
//         {
//             node->add_info_entry("followpos", node->get_info()["firstpos"]);
//         }
//     }
// }

// StateMachine dfa_from_syntax_tree(TreeNode root)
// {
//     auto root_ptr = make_shared<TreeNode>(root);
//     nullable(root_ptr);
//     firstpos(root_ptr, "firstpos");
//     lastpos(root_ptr);
//     followpos(root_ptr);

//     Set<Set<TreeNode>> d_states;
//     return StateMachine();
// }