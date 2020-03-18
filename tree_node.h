#ifndef TREE_NODE_H
#define TREE_NODE_H

#include <any>
#include <map>
#include <memory>
#include <vector>

using namespace std;

class TreeNode
{
  private:
    int node_symbol = 36;
    int node_name = -1;
    shared_ptr<TreeNode> left_child = nullptr;
    shared_ptr<TreeNode> right_child = nullptr;
    map<string, any> info;

  public:
    TreeNode();
    TreeNode(int, int);
    TreeNode(int, int, shared_ptr<TreeNode>);
    TreeNode(int, int, shared_ptr<TreeNode>, shared_ptr<TreeNode>);
    shared_ptr<TreeNode> left();
    shared_ptr<TreeNode> right();
    void set_left(shared_ptr<TreeNode>);
    void set_right(shared_ptr<TreeNode>);
    map<string, any> get_info();
    void add_info_entry(string, any);
    int symbol();
    int name();
    vector<shared_ptr<TreeNode>> flatten();
    void get_children_and_beyond_into_vec(vector<shared_ptr<TreeNode>> &);
    shared_ptr<TreeNode> find(int);
    vector<int> get_all_input_symbols();
};

#endif