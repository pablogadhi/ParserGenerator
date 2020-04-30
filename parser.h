#ifndef PARSER_H
#define PARSER_H

#include "s_core_generator.h"
#include "scanner.h"

class Parser
{
  private:
    Scanner &scanner;
    SCoreGenerator generator;
    Token<string> current_token;
    vector<Token<string>> token_list;
    string new_compiler_name;
    SymbolTable new_table;
    unordered_map<string, vector<Token<Set<char>>>> token_regex_map;

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