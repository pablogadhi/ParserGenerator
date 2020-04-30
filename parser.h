#ifndef PARSER_H
#define PARSER_H

#include "s_core_generator.h"
#include "scanner.h"

class Parser
{
  private:
    Scanner &scanner;
    SCoreGenerator generator;
    Token current_token;
    vector<Token> token_list;
    string new_compiler_name;
    SymbolTable new_table;
    unordered_map<string, vector<Set<char>>> token_regex_map;

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