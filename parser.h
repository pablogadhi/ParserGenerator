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
    vector<pair<string, vector<Token<string>>>> productions;
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
    void prod_decl(ifstream &, ofstream &, ofstream &);
    void write_scanner();
    void write_parser_start(ifstream &, ofstream &, ofstream &, streampos &, streampos &);
    void write_parser_end(ifstream &, ofstream &, ofstream &, streampos &, streampos &);
    string compiler_name();
    void syn_error();
};

#endif