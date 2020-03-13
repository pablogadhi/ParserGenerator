#include "tree_node.h"

TreeNode::TreeNode()
{
}

TreeNode::TreeNode(string name, int symbol) : node_name(name), node_symbol(symbol)
{
}

TreeNode::TreeNode(string name, int symbol, shared_ptr<TreeNode> left_ptr)
    : node_name(name), node_symbol(symbol), left_child(left_ptr)
{
}

TreeNode::TreeNode(string name, int symbol, shared_ptr<TreeNode> left_ptr, shared_ptr<TreeNode> right_ptr)
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

string TreeNode::name()
{
    return node_name;
}