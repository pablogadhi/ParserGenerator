#ifndef SCANNER_H
#define SCANNER_H

#include "error.h"
#include "state_machine.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

template <class T> class Token
{
  private:
    string t_name;
    T t_val;

  public:
    Token();
    Token(string, T);
    ~Token();
    void set_name(string);
    string name();
    T value();
    bool operator==(Token &t)
    {
        return this->t_val == t.value();
    }
    bool operator<(Token<T> &t)
    {
        return t_name < t.name();
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
    unordered_map<string, Set<char>> &char_sets();
    unordered_map<string, string> &keywords();
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
    Token<string> c_token;
    Token<string> n_token;
    DFA finder = DFA();
    SymbolTable s_table;
    int line = 1;
    int column = 1;
    vector<Error> error_list;

    void read_into_string_buffer(string &);

  public:
    Scanner();
    Scanner(string);
    ~Scanner();

    void set_finder(DFA);
    DFA &get_finder();
    SymbolTable &symbols();
    Token<string> scan();
    void next_char();
    char peek_char();
    void ignore_all_blank_chars();
    Token<string> next_token();
    Token<string> current();
    Token<string> look_ahead();
    vector<Error> errors();
};

#endif