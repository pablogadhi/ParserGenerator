#ifndef TREE_NODE_H
#define TREE_NODE_H

#include <any>
#include <map>
#include <memory>

using namespace std;

class TreeNode
{
  private:
    int node_symbol = 36;
    string node_name = "";
    shared_ptr<TreeNode> left_child = nullptr;
    shared_ptr<TreeNode> right_child = nullptr;
    map<string, any> info;

  public:
    TreeNode();
    TreeNode(string, int);
    TreeNode(string, int, shared_ptr<TreeNode>);
    TreeNode(string, int, shared_ptr<TreeNode>, shared_ptr<TreeNode>);
    shared_ptr<TreeNode> left();
    shared_ptr<TreeNode> right();
    void set_left(shared_ptr<TreeNode>);
    void set_right(shared_ptr<TreeNode>);
    map<string, any> get_info();
    void add_info_entry(string, any);
    int symbol();
    string name();
};

#endif