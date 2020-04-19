#include "dfa_builder.h"
#include <memory>

template <class T> Set<Set<T>> get_unmarked_states(Set<Set<T>> set)
{
    Set<Set<T>> unmarked_states;
    for (auto &state : set)
    {
        if (!state.is_marked())
        {
            unmarked_states.add(state);
        }
    }
    return unmarked_states;
}

template <class T> Set<Set<T>> mark_state(Set<Set<T>> set, Set<T> state_to_mark)
{
    auto set_with_marked_state = set;
    auto item_index = set_with_marked_state.has_item(state_to_mark);
    set_with_marked_state[item_index].mark();
    return set_with_marked_state;
}

bool is_set_acceptance(Set<State> set, int useless_param)
{
    for (auto &state : set)
    {
        if (state.is_accepting())
        {
            return true;
        }
    }
    return false;
}

template <class T, class F, class G>
DFA machine_from_transitions(vector<pair<pair<Set<T>, int>, Set<T>>> d_tran, F acceptance_check, G accept_criteria)
{
    Set<State> created_states;
    vector<shared_ptr<State>> state_ptrs;
    for (auto &trans : d_tran)
    {
        Set<T> states_in_tran[2] = {trans.first.first, trans.second};
        int indices[2];
        for (int i = 0; i < 2; i++)
        {
            auto new_state = State(states_in_tran[i].get_name());
            auto item_index = created_states.has_item(new_state);
            if (item_index == -1)
            {
                if (acceptance_check(states_in_tran[i], accept_criteria))
                {
                    new_state.set_as_accepting(true);
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

    return DFA(state_ptrs[0], state_ptrs[state_ptrs.size() - 1]);
}

DFA dfa_from_nfa(NFA nfa)
{
    Set<Set<State>> d_states;
    int name_index = 0;
    d_states.add(e_closure(Set(name_index, vector<State>{(*nfa.start())})));
    auto symbols = nfa.get_all_input_symbols();
    vector<pair<pair<Set<State>, int>, Set<State>>> d_tran;

    auto unmarked_states = get_unmarked_states<State>(d_states);
    while (!unmarked_states.empty())
    {
        auto T = unmarked_states[0];
        d_states = mark_state<State>(d_states, T);

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
                    U.set_name(name_index);
                    d_states.add(U);
                }
                else
                {
                    U.set_name(d_states[U_index].get_name());
                }
                d_tran.push_back(make_pair(make_pair(T, a), U));
            }
        }

        unmarked_states = get_unmarked_states<State>(d_states);
    }

    return machine_from_transitions<State, function<bool(Set<State>, int)>, int>(d_tran, is_set_acceptance, 0);
}

void nullable(shared_ptr<TreeNode> node)
{
    if (node == nullptr)
    {
        return;
    }
    nullable(node->left());
    nullable(node->right());

    bool is_nullable;
    switch (node->symbol())
    {
    case 36:
    case 42:
    case 63:
        is_nullable = true;
        break;
    case 124:
        is_nullable = any_cast<bool>(node->left()->get_info()["nullable"]) ||
                      any_cast<bool>(node->right()->get_info()["nullable"]);
        break;
    case 46:
        is_nullable = any_cast<bool>(node->left()->get_info()["nullable"]) &&
                      any_cast<bool>(node->right()->get_info()["nullable"]);
        break;
    case 43:
        is_nullable = any_cast<bool>(node->left()->get_info()["nullable"]);
        break;
    default:
        is_nullable = false;
        break;
    }
    node->add_info_entry("nullable", is_nullable);
}

void first_last_pos(shared_ptr<TreeNode>, string, bool);

Set<int> first_last_pos_or(shared_ptr<TreeNode> node_1, shared_ptr<TreeNode> node_2, string info_entry_key, bool last)
{

    return union_between_sets<int>(any_cast<Set<int>>(node_1->get_info()[info_entry_key]),
                                   any_cast<Set<int>>(node_2->get_info()[info_entry_key]));
}

void first_last_pos(shared_ptr<TreeNode> node, string info_entry_key, bool last)
{
    if (node == nullptr)
    {
        return;
    }
    first_last_pos(node->left(), info_entry_key, last);
    first_last_pos(node->right(), info_entry_key, last);

    Set<int> result;
    shared_ptr<TreeNode> important_child = node->left();
    if (last && node->right() != nullptr)
    {
        important_child = node->right();
    }

    switch (node->symbol())
    {
    case 36:
        break;
    case 124:
        result = first_last_pos_or(node->left(), node->right(), info_entry_key, last);
        break;
    case 46:
        if (any_cast<bool>(important_child->get_info()["nullable"]))
        {
            result = first_last_pos_or(node->left(), node->right(), info_entry_key, last);
        }
        else
        {
            result = any_cast<Set<int>>(important_child->get_info()[info_entry_key]);
        }
        break;
    case 42:
    case 43:
    case 63:
        result = any_cast<Set<int>>(important_child->get_info()[info_entry_key]);
        break;
    default:
        result.add(node->name());
        break;
    }
    node->add_info_entry(info_entry_key, result);
}

void followpos(shared_ptr<TreeNode> node)
{

    if (node == nullptr)
    {
        return;
    }
    followpos(node->left());
    followpos(node->right());

    if (node->symbol() == 46)
    {
        for (auto &position : any_cast<Set<int>>(node->left()->get_info()["lastpos"]))
        {
            auto prev_followpos = Set<int>();
            auto node_ptr = node->find(position);
            auto node_info = node_ptr->get_info();
            if (node_info.find("followpos") != node_info.end())
            {
                prev_followpos = any_cast<Set<int>>(node_info["followpos"]);
            }
            node_ptr->add_info_entry(
                "followpos",
                union_between_sets<int>(any_cast<Set<int>>(node->right()->get_info()["firstpos"]), prev_followpos));
        }
    }
    else if (node->symbol() == 42 || node->symbol() == 43)
    {
        for (auto &position : any_cast<Set<int>>(node->get_info()["lastpos"]))
        {
            auto prev_followpos = Set<int>();
            auto node_ptr = node->find(position);
            auto node_info = node_ptr->get_info();
            if (node_info.find("followpos") != node_info.end())
            {
                prev_followpos = any_cast<Set<int>>(node_info["followpos"]);
            }
            node_ptr->add_info_entry(
                "followpos", union_between_sets(any_cast<Set<int>>(node->get_info()["firstpos"]), prev_followpos));
        }
    }
}

vector<shared_ptr<TreeNode>> get_nodes_with_symbol_in_set(shared_ptr<TreeNode> root, Set<int> set, int symbol)
{
    vector<shared_ptr<TreeNode>> node_vector;
    for (auto &node_name : set)
    {
        auto node_ptr = root->find(node_name);
        if (node_ptr->symbol() == symbol)
        {
            node_vector.push_back(node_ptr);
        }
    }

    return node_vector;
}

bool has_set_sharp_node(Set<int> set, int sharp_pos)
{
    auto index = set.has_item(sharp_pos);
    if (index != -1)
    {
        return true;
    }
    return false;
}

DFA dfa_from_syntax_tree(TreeNode root)
{
    auto root_ptr = make_shared<TreeNode>(root);

    nullable(root_ptr);
    first_last_pos(root_ptr, "firstpos", false);
    first_last_pos(root_ptr, "lastpos", true);
    followpos(root_ptr);

    Set<Set<int>> d_states;
    int name_index = 0;
    d_states.add(any_cast<Set<int>>(root_ptr->get_info()["firstpos"]));
    d_states[0].set_name(name_index);
    vector<pair<pair<Set<int>, int>, Set<int>>> d_tran;

    auto unmarked_states = get_unmarked_states<int>(d_states);
    while (!unmarked_states.empty())
    {
        auto S = unmarked_states[0];
        d_states = mark_state(d_states, S);

        for (auto &a : root_ptr->get_all_input_symbols())
        {
            auto U = Set<int>();
            for (auto &node : get_nodes_with_symbol_in_set(root_ptr, S, a))
            {
                U = union_between_sets(any_cast<Set<int>>(node->get_info()["followpos"]), U);
            }

            if (U.size() == 0)
            {
                continue;
            }

            auto U_index = d_states.has_item(U);
            if (U_index == -1)
            {
                name_index++;
                U.set_name(name_index);
                d_states.add(U);
            }
            else
            {
                U.set_name(d_states[U_index].get_name());
            }

            d_tran.push_back(make_pair(make_pair(S, a), U));
        }

        unmarked_states = get_unmarked_states<int>(d_states);
    }

    // root_ptr->print_info();
    auto sharp_pos = root_ptr->right()->name();
    return machine_from_transitions<int, function<bool(Set<int>, int)>, int>(d_tran, has_set_sharp_node, sharp_pos);
}