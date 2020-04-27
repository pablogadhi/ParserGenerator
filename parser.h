#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "tree_node.h"

class Parser
{
  private:
    Scanner &scanner;
    Token current_token;
    vector<Token> token_list;
    string new_compiler_name;
    SymbolTable new_table;
    vector<pair<string, vector<Set<char>>>> token_regex_list;

  public:
    Parser(Scanner &);
    ~Parser();
    void parse();
    void get();
    void expect(string);
    void set_decl();
    void keyword_decl();
    void token_decl();
    void write_scanner();
    string compiler_name();
};

#endif