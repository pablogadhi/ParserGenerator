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
    unordered_map<string, pair<vector<Token<Set<char>>>, bool>> token_regex_map;
    vector<Error> syntactic_errors;

  public:
    Parser(Scanner &);
    ~Parser();
    void parse();
    void get();
    void expect(string);
    bool soft_expect(string);
    void set_decl();
    void keyword_decl();
    void token_decl();
    void ignore_decl();
    void write_scanner();
    string compiler_name();
    void syn_error();
};

#endif