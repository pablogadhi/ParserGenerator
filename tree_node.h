#ifndef TREE_NODE_H
#define TREE_NODE_H

#include "set.h"
#include <any>
#include <map>
#include <memory>
#include <vector>

using namespace std;

template <class T> class TreeNode
{
  private:
    T node_symbol;
    int node_name = -1;
    shared_ptr<TreeNode<T>> left_child = nullptr;
    shared_ptr<TreeNode<T>> right_child = nullptr;
    map<string, any> info;

  public:
    TreeNode<T>();
    TreeNode<T>(int, T);
    TreeNode<T>(int, T, shared_ptr<TreeNode<T>>);
    TreeNode<T>(int, T, shared_ptr<TreeNode<T>>, shared_ptr<TreeNode<T>>);
    shared_ptr<TreeNode<T>> left();
    shared_ptr<TreeNode<T>> right();
    void set_left(shared_ptr<TreeNode<T>>);
    void set_right(shared_ptr<TreeNode<T>>);
    map<string, any> get_info();
    void add_info_entry(string, any);
    T symbol();
    int name();
    vector<shared_ptr<TreeNode<T>>> flatten();
    void get_children_and_beyond_into_vec(vector<shared_ptr<TreeNode<T>>> &);
    shared_ptr<TreeNode<T>> find(int);
};

#endif