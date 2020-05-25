#ifndef S_CORE_GENERATOR_H
#define S_CORE_GENERATOR_H

#include "scanner.h"
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

    bool is_regex_operator(Token<Set<char>>);
    bool is_special_char(Token<Set<char>>);
    vector<Token<Set<char>>> infix_to_postfix(vector<Token<Set<char>>>, function<int(char)>);
    TreeNode<int> make_syntax_tree(Token<Set<char>>, int &, vector<TreeNode<int>>);
    TreeNode<int> tree_from_set(Token<Set<char>>, int &);
    TreeNode<int> basic_syntax_node_generator(Token<Set<char>>, int &);
    TreeNode<int> postfix_eval(vector<Token<Set<char>>>);
    vector<Token<Set<char>>> format_regex(vector<Token<Set<char>>>);

  public:
    SCoreGenerator();
    ~SCoreGenerator();
    DFA generate_dfa_finder(unordered_map<string, pair<vector<Token<Set<char>>>, bool>>, bool append_operators = false,
                            bool debug = false);
    Set<char> operators();
};

#endif