#include "tree_node.h"
#include <iostream>

template <class T> TreeNode<T>::TreeNode()
{
}

template <class T> TreeNode<T>::TreeNode(int name, T symbol) : node_name(name), node_symbol(symbol)
{
}

template <class T>
TreeNode<T>::TreeNode(int name, T symbol, shared_ptr<TreeNode<T>> left_ptr)
    : node_name(name), node_symbol(symbol), left_child(left_ptr)
{
}

template <class T>
TreeNode<T>::TreeNode(int name, T symbol, shared_ptr<TreeNode<T>> left_ptr, shared_ptr<TreeNode<T>> right_ptr)
    : node_name(name), node_symbol(symbol), left_child(left_ptr), right_child(right_ptr)
{
}

template <class T> shared_ptr<TreeNode<T>> TreeNode<T>::left()
{
    return left_child;
}

template <class T> shared_ptr<TreeNode<T>> TreeNode<T>::right()
{
    return right_child;
}

template <class T> void TreeNode<T>::set_left(shared_ptr<TreeNode<T>> node_ptr)
{
    left_child = node_ptr;
}

template <class T> void TreeNode<T>::set_right(shared_ptr<TreeNode<T>> node_ptr)
{
    right_child = node_ptr;
}

template <class T> map<string, any> TreeNode<T>::get_info()
{
    return info;
}

template <class T> void TreeNode<T>::add_info_entry(string key, any value)
{
    info[key] = value;
}

template <class T> T TreeNode<T>::symbol()
{
    return node_symbol;
}

template <class T> int TreeNode<T>::name()
{
    return node_name;
}

template <class T> vector<shared_ptr<TreeNode<T>>> TreeNode<T>::flatten()
{
    vector<shared_ptr<TreeNode<T>>> result;
    result.push_back(make_shared<TreeNode<T>>(*this));
    get_children_and_beyond_into_vec(result);
    return result;
}

template <class T> void TreeNode<T>::get_children_and_beyond_into_vec(vector<shared_ptr<TreeNode<T>>> &vec)
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

template <class T> shared_ptr<TreeNode<T>> TreeNode<T>::find(int name)
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

template class TreeNode<int>;
template class TreeNode<Set<char>>;