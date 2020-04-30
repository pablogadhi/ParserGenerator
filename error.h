#ifndef ERROR_H
#define ERROR_H

#include <string>

using namespace std;

enum error_type
{
    LEXICAL_ERROR,
    SYNTACTIC_ERROR,
    SEMANTIC_ERROR
};

class Error
{
  private:
    int e_line = -1;
    int e_column = -1;
    error_type e_type = LEXICAL_ERROR;
    string e_val = "";

  public:
    Error();
    Error(string, error_type type = LEXICAL_ERROR);
    Error(int, int, string, error_type type = LEXICAL_ERROR);
    ~Error();
    int line();
    int column();
    error_type type();
    string value();
};

#endif