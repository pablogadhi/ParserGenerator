#ifndef DFA_BUILDER_H
#define DFA_BUILDER_H

#include "state_machine.h"
#include "tree_node.h"

DFA dfa_from_nfa(NFA);

class DFABuilder
{
  private:
    unordered_map<string, Set<char>> char_map;

  public:
    DFABuilder(unordered_map<string, Set<char>>);
    ~DFABuilder();
    vector<int> get_all_input_symbols(shared_ptr<TreeNode<int>>);
    DFA dfa_from_nfa(NFA, int &);
    DFA dfa_from_syntax_tree(TreeNode<int>, int &, string);
    void print_info(shared_ptr<TreeNode<int>>);
};

#endif