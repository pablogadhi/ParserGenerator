#ifndef DFA_BUILDER_H
#define DFA_BUILDER_H

#include "state_machine.h"
#include "tree_node.h"

DFA dfa_from_nfa(NFA);
DFA dfa_from_syntax_tree(TreeNode);

#endif