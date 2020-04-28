#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "tree_node.h"

class Parser
{
  private:
    Scanner &scanner;
    Set<char> operators = Set<char>{'|', '}', ']', '\0'};
    Set<char> special_chars = Set<char>{'(', ')', '{', '['};
    Set<string> coco_operators = Set<string>{".", "..", "=", "-", "+"};
    Token current_token;
    vector<Token> token_list;
    string new_compiler_name;
    SymbolTable new_table;
    vector<pair<string, vector<Set<char>>>> token_regex_list;

    DFA generate_dfa_finder(vector<pair<string, vector<Set<char>>>>, bool, bool);

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