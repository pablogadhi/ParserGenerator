#ifndef SCANNER_H
#define SCANNER_H

#include "state_machine.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

enum token_type
{
    KEYWORD,
    IDENTIFIER,
    CONSTANT,
    OPERATOR,
};

class Token
{
  private:
    string t_val;
    token_type t_type;

  public:
    Token();
    Token(string);
    ~Token();
    string value();
    void add_char(char);
    bool empty();
    token_type type();
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
    vector<Token> token_list;
    DFA finder = DFA();
    unordered_map<string, Set<char>> char_map;

    void read_into_string_buffer(string &);

  public:
    Scanner();
    Scanner(string);
    ~Scanner();

    Token scan();
    void next_char();
    Token next_token();
    Token current();
    Token look_ahead();
};

#endif