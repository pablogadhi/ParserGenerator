#ifndef SCANNER_H
#define SCANNER_H

#include "state_machine.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class Token
{
  private:
    string t_name;
    string t_val;

  public:
    Token();
    Token(string, string);
    ~Token();
    void set_name(string);
    string name();
    string value();
    void add_char(char);
    bool empty();
    bool operator==(Token &t)
    {
        return t_val.compare(t.value()) == 0;
    }
    bool operator<(Token &t)
    {
        return t_val.compare(t.value()) > 0;
    }
};

class SymbolTable
{
  private:
    unordered_map<string, Set<char>> char_set_map;
    unordered_map<string, string> keywords_map;

  public:
    void add_char_set(string, Set<char>);
    void add_keyword(string, string);
    unordered_map<string, Set<char>> char_sets();
    unordered_map<string, string> keywords();
};

class Scanner
{
  private:
    ifstream file_buffer;
    string buffer_0;
    string buffer_1;
    string current_buffer;
    string prev_buffer;
    string::iterator forward;
    int lexeme_begin_idx;
    Token c_token;
    Token n_token;
    DFA finder = DFA();
    SymbolTable s_table;

    void read_into_string_buffer(string &);

  public:
    Scanner();
    Scanner(string);
    ~Scanner();

    void set_finder(DFA);
    SymbolTable &symbols();
    Token scan();
    void next_char();
    char peek_char();
    void ignore_all_blank_chars();
    Token next_token();
    Token current();
    Token look_ahead();
};

#endif
