#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "tree_node.h"

class Parser
{
  private:
    Scanner &scanner;
    Token c_token;
    Token la_token;
    TreeNode<int> syntax_tree_head;

  public:
    Parser(Scanner &);
    ~Parser();
    void parse();
    void write_scanner();
};

#endif