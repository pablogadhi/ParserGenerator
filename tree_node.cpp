#include "tree_node.h"
#include "utils.h"
#include <iostream>

TreeNode::TreeNode()
{
}

TreeNode::TreeNode(int name, int symbol) : node_name(name), node_symbol(symbol)
{
}

TreeNode::TreeNode(int name, int symbol, shared_ptr<TreeNode> left_ptr)
    : node_name(name), node_symbol(symbol), left_child(left_ptr)
{
}

TreeNode::TreeNode(int name, int symbol, shared_ptr<TreeNode> left_ptr, shared_ptr<TreeNode> right_ptr)
    : node_name(name), node_symbol(symbol), left_child(left_ptr), right_child(right_ptr)
{
}

shared_ptr<TreeNode> TreeNode::left()
{
    return left_child;
}

shared_ptr<TreeNode> TreeNode::right()
{
    return right_child;
}

void TreeNode::set_left(shared_ptr<TreeNode> node_ptr)
{
    left_child = node_ptr;
}

void TreeNode::set_right(shared_ptr<TreeNode> node_ptr)
{
    right_child = node_ptr;
}

map<string, any> TreeNode::get_info()
{
    return info;
}

void TreeNode::add_info_entry(string key, any value)
{
    info[key] = value;
}

int TreeNode::symbol()
{
    return node_symbol;
}

int TreeNode::name()
{
    return node_name;
}

vector<shared_ptr<TreeNode>> TreeNode::flatten()
{
    vector<shared_ptr<TreeNode>> result;
    result.push_back(make_shared<TreeNode>(*this));
    get_children_and_beyond_into_vec(result);
    return result;
}

void TreeNode::get_children_and_beyond_into_vec(vector<shared_ptr<TreeNode>> &vec)
{
    if (left_child != nullptr)
    {
        vec.push_back(left_child);
        left_child->get_children_and_beyond_into_vec(vec);
    }

    if (right_child != nullptr)
    {
        vec.push_back(right_child);
        right_child->get_children_and_beyond_into_vec(vec);
    }
}

shared_ptr<TreeNode> TreeNode::find(int name)
{
    for (auto &node : flatten())
    {
        if (node->name() == name)
        {
            return node;
        }
    }
    return nullptr;
}

vector<int> TreeNode::get_all_input_symbols()
{
    vector<int> symbols;
    for (auto &node : flatten())
    {
        auto s = node->symbol();
        // TODO Define is_in_alphabet() function
        if (!is_item_in_vector(s, symbols) && (isalnum((char)s)))
        {
            symbols.push_back(s);
        }
    }
    return symbols;
}

void TreeNode::print_info()
{
    for (auto &node : flatten())
    {
        auto node_info = node->get_info();
        cout << "Nodo " << node->name() << ":" << endl;
        cout << "Simbolo " << (char)(node->symbol()) << ":" << endl;
        cout << "Nullable: " << any_cast<bool>(node_info["nullable"]) << endl;

        auto pos_set = any_cast<Set<int>>(node_info["firstpos"]);
        string pos_str = "";
        for (auto &i : pos_set)
        {
            pos_str = pos_str + to_string(i) + ",";
        }
        cout << "Firstpost: " << pos_str << endl;

        pos_set = any_cast<Set<int>>(node_info["lastpos"]);
        pos_str = "";
        for (auto &i : pos_set)
        {
            pos_str = pos_str + to_string(i) + ",";
        }
        cout << "Lastpos: " << pos_str << endl;

        if (node_info.find("followpos") != node_info.end())
        {
            pos_set = any_cast<Set<int>>(node->get_info()["followpos"]);
            pos_str = "";
            for (auto &i : pos_set)
            {
                pos_str = pos_str + to_string(i) + ",";
            }
            cout << "Followpos: " << pos_str << endl;
        }
        cout << endl;
    }
}