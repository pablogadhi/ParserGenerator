#include "error.h"

Error::Error()
{
}

Error::Error(string val, error_type type) : e_val(val), e_type(type)
{
}

Error::Error(int line, int col, string val, error_type type) : e_line(line), e_column(col), e_val(val), e_type(type)
{
}

Error::~Error()
{
}

int Error::line()
{
    return e_line;
}

int Error::column()
{
    return e_column;
}

string Error::value()
{
    return e_val;
}

error_type Error::type()
{
    return e_type;
}