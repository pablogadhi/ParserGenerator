#ifndef S_CORE_GENERATOR_H
#define S_CORE_GENERATOR_H

#include "set.h"
#include "state_machine.h"
#include "tree_node.h"
#include <vector>

using namespace std;

class SCoreGenerator
{
  private:
    Set<char> regex_operators = Set<char>{'|', '}', ']', '\0'};
    Set<char> special_chars = Set<char>{'(', ')', '{', '['};
    Set<string> coco_operators = Set<string>{".", "..", "=", "-", "+"};

    bool is_regex_operator(Set<char>);
    bool is_special_char(Set<char>);
    vector<Set<char>> infix_to_postfix(vector<Set<char>>, function<int(char)>);
    TreeNode<int> make_syntax_tree(Set<char>, int &, vector<TreeNode<int>>);
    TreeNode<int> tree_from_set(Set<char>, int &);
    TreeNode<int> basic_syntax_node_generator(Set<char>, int &);
    TreeNode<int> postfix_eval(vector<Set<char>>);
    vector<Set<char>> format_regex(vector<Set<char>>);
    TreeNode<int> expand_tree(shared_ptr<TreeNode<Set<char>>>, int &);

  public:
    SCoreGenerator();
    ~SCoreGenerator();
    DFA generate_dfa_finder(unordered_map<string, vector<Set<char>>>, bool append_operators = false,
                            bool debug = false);
    Set<char> operators();
};

#endif