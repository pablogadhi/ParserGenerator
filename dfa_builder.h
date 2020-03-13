#ifndef DFA_BUILDER_H
#define DFA_BUILDER_H

#include "state_machine.h"
#include "tree_node.h"

StateMachine dfa_from_nfa(StateMachine);
StateMachine dfa_from_syntax_tree(TreeNode);

#endif