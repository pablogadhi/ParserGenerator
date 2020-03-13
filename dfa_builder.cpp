#include "dfa_builder.h"
#include "ptr_set.h"
#include <algorithm>
#include <map>
#include <memory>
#include <stack>

shared_ptr<PtrSet<shared_ptr<State>>> e_closure(shared_ptr<PtrSet<shared_ptr<State>>> set)
{
    stack<shared_ptr<State>> s_stack;
    for (auto state : set->get_items())
    {
        s_stack.push(state);
    }
    shared_ptr<PtrSet<shared_ptr<State>>> e_closure_set = set;

    while (!s_stack.empty())
    {
        shared_ptr<State> t = s_stack.top();
        s_stack.pop();
        for (shared_ptr<State> &u : t->get_next_e_states())
        {
            if (!e_closure_set->has_item(u))
            {
                e_closure_set->add(u);
                s_stack.push(u);
            }
        }
    }

    return e_closure_set;
}

PtrSet<shared_ptr<PtrSet<shared_ptr<State>>>> get_unmarked_states(PtrSet<shared_ptr<PtrSet<shared_ptr<State>>>> set)
{
    PtrSet<shared_ptr<PtrSet<shared_ptr<State>>>> unmarked_states;
    for (auto &state : set.get_items())
    {
        if (!state->is_marked())
        {
            unmarked_states.add(state);
        }
    }
    return unmarked_states;
}

shared_ptr<PtrSet<shared_ptr<State>>> move_set_of_states(shared_ptr<PtrSet<shared_ptr<State>>> set, int symbol)
{
    vector<shared_ptr<State>> result_vec;
    for (auto &state : set->get_items())
    {
        auto moved_state = state->move(symbol);
        result_vec.insert(result_vec.end(), moved_state.begin(), moved_state.end());
    }
    return make_shared<PtrSet<shared_ptr<State>>>(result_vec);
}

StateMachine machine_from_transitions(
    vector<pair<pair<shared_ptr<PtrSet<shared_ptr<State>>>, int>, shared_ptr<PtrSet<shared_ptr<State>>>>> d_tran)
{
    PtrSet<shared_ptr<State>> created_states;
    for (auto &trans : d_tran)
    {
        shared_ptr<State> states_in_trans[2];
        string state_names[2] = {trans.first.first->get_name(), trans.second->get_name()};
        for (int i = 0; i < 2; i++)
        {
            states_in_trans[i] = make_shared<State>(State(state_names[i]));
            auto state_in_created_states = created_states.has_item(states_in_trans[i]);
            if (state_in_created_states == nullptr)
            {
                created_states.add(states_in_trans[i]);
            }
            else
            {
                states_in_trans[i] = state_in_created_states;
            }
        }

        states_in_trans[0]->add_t_function(make_pair(trans.first.second, states_in_trans[1]));
    }

    return StateMachine(created_states.get_items()[0], created_states.get_items()[created_states.size() - 1]);
}

StateMachine dfa_from_nfa(StateMachine nfa)
{
    PtrSet<shared_ptr<PtrSet<shared_ptr<State>>>> d_states;
    int name_index = 0;
    d_states.add(e_closure(
        make_shared<PtrSet<shared_ptr<State>>>(to_string(name_index), vector<shared_ptr<State>>{(nfa.start())})));
    auto symbols = nfa.get_all_input_symbols();
    vector<pair<pair<shared_ptr<PtrSet<shared_ptr<State>>>, int>, shared_ptr<PtrSet<shared_ptr<State>>>>> d_tran;

    auto unmarked_states = get_unmarked_states(d_states);
    while (!unmarked_states.empty())
    {
        auto T = unmarked_states.get_items()[0];
        T->mark();

        for (auto &a : symbols)
        {
            auto movements = move_set_of_states(T, a);
            if (!movements->empty())
            {
                auto U = e_closure(movements);
                auto U_in_d_states = d_states.has_item(U);
                if (U_in_d_states == nullptr)
                {
                    U->unmark();
                    name_index++;
                    U->set_name(to_string(name_index));
                    d_states.add(U);
                }
                else
                {
                    U->set_name(U_in_d_states->get_name());
                }
                d_tran.push_back(make_pair(make_pair(T, a), U));
            }
        }

        unmarked_states = get_unmarked_states(d_states);
    }

    return machine_from_transitions(d_tran);
}

void nullable(shared_ptr<TreeNode> node)
{
    bool is_nullable;
    switch (node->symbol())
    {
    case 36:
        is_nullable = true;
        break;
    case 124:
        nullable(node->left());
        nullable(node->right());
        is_nullable = any_cast<bool>(node->left()->get_info()["nullable"]) ||
                      any_cast<bool>(node->right()->get_info()["nullable"]);
        break;
    case 46:
        nullable(node->left());
        nullable(node->right());
        is_nullable = any_cast<bool>(node->left()->get_info()["nullable"]) &&
                      any_cast<bool>(node->right()->get_info()["nullable"]);
        break;
    case 42:
        is_nullable = true;
        break;
    default:
        is_nullable = false;
        break;
    }
    node->add_info_entry("nullable", is_nullable);
}

PtrSet<shared_ptr<TreeNode>> firstpos_or(shared_ptr<TreeNode> node_1, shared_ptr<TreeNode> node_2)
{

    return set_union<PtrSet<shared_ptr<TreeNode>>, shared_ptr<TreeNode>>(
        any_cast<PtrSet<shared_ptr<TreeNode>>>(node_1->get_info()["firstpos"]),
        any_cast<PtrSet<shared_ptr<TreeNode>>>(node_2->get_info()["firstpos"]));
}

void firstpos(shared_ptr<TreeNode> node, string info_entry_key)
{
    PtrSet<shared_ptr<TreeNode>> result;
    switch (node->symbol())
    {
    case 36:
        break;
    case 124:
        firstpos(node->left(), info_entry_key);
        firstpos(node->right(), info_entry_key);
        result = firstpos_or(node->left(), node->right());
        break;
    case 46:
        if (any_cast<bool>(node->left()->get_info()["nullable"]))
        {
            firstpos(node->left(), info_entry_key);
            firstpos(node->right(), info_entry_key);
            result = firstpos_or(node->left(), node->right());
        }
        else
        {
            firstpos(node->left(), info_entry_key);
            result = any_cast<PtrSet<shared_ptr<TreeNode>>>(node->left()->get_info()["firstpos"]);
        }
        break;
    case 42:
        firstpos(node->left(), info_entry_key);
        result = any_cast<PtrSet<shared_ptr<TreeNode>>>(node->left()->get_info()["firstpos"]);
        break;
    default:
        result.add(node);
        break;
    }
    node->add_info_entry(info_entry_key, result);
}

PtrSet<shared_ptr<TreeNode>> lastpos_or(shared_ptr<TreeNode> node_1, shared_ptr<TreeNode> node_2)
{

    return set_union<PtrSet<shared_ptr<TreeNode>>, shared_ptr<TreeNode>>(
        any_cast<PtrSet<shared_ptr<TreeNode>>>(node_1->get_info()["lastpos"]),
        any_cast<PtrSet<shared_ptr<TreeNode>>>(node_2->get_info()["lastpos"]));
}

void lastpos(shared_ptr<TreeNode> node)
{
    if (node->symbol() == 46)
    {
        if (any_cast<bool>(node->right()->get_info()["nullable"]))
        {
            lastpos(node->left());
            lastpos(node->right());
            node->add_info_entry("lastpos", lastpos_or(node->left(), node->right()));
        }
        else
        {
            lastpos(node->right());
        }
    }
    else
    {
        firstpos(node, "lastpos");
    }
}

void followpos(shared_ptr<TreeNode> node)
{
    if (node->symbol() == 46)
    {
        for (auto &node : any_cast<PtrSet<shared_ptr<TreeNode>>>(node->left()->get_info()["lastpos"]).get_items())
        {
            node->add_info_entry("followpos", node->right()->get_info()["lastpos"]);
        }
    }
    else if (node->symbol() == 42)
    {
        for (auto &node : any_cast<PtrSet<shared_ptr<TreeNode>>>(node->get_info()["lastpos"]).get_items())
        {
            node->add_info_entry("followpos", node->get_info()["firstpos"]);
        }
    }
}

StateMachine dfa_from_syntax_tree(TreeNode root)
{
    auto root_ptr = make_shared<TreeNode>(root);
    nullable(root_ptr);
    firstpos(root_ptr, "firstpos");
    lastpos(root_ptr);
    followpos(root_ptr);

    PtrSet<shared_ptr<PtrSet<shared_ptr<TreeNode>>>> d_states;
    return StateMachine();
}