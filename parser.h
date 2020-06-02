#ifndef PARSER_H
#define PARSER_H

#include "s_core_generator.h"
#include "scanner.h"

class Production
{
  private:
    string p_name;
    string p_attributes;
    vector<Token<string>> p_body;
    unordered_map<int, string> p_sem_actions;
    unordered_map<int, string> p_ident_attr;

  public:
    Production(string, string, vector<Token<string>>, unordered_map<int, string>, unordered_map<int, string>);
    Production();
    ~Production();
    string name();
    string attr();
    vector<Token<string>> body();
    unordered_map<int, string> sem_actions();
    unordered_map<int, string> ident_attr();
};

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
    string start_s_name;
    vector<Production> productions;
    unordered_map<string, int> production_indices;
    unordered_map<string, Set<string>> first_pos;
    unordered_map<string, Set<string>> follow_pos;
    unordered_map<string, unordered_map<string, int>> parsing_table;
    vector<Error> syntactic_errors;

  public:
    Parser(Scanner &);
    ~Parser();
    void parse();
    void get();
    Token<string> last_token();
    void expect(string);
    bool soft_expect(string);
    void set_decl();
    void keyword_decl();
    void token_decl();
    void ignore_decl();
    void prod_decl();
    void compute_first_pos(Token<string>, Production, int, Set<string> &, vector<string>);
    void compute_follow_pos(string);
    void fill_parsing_table();
    void write_scanner();
    void write_parser();
    string compiler_name();
    void syn_error();
};

#endif