#ifndef UTILS_H
#define UTILS_H

#include <vector>

namespace
{

template <class T> bool is_item_in_vector(T item, std::vector<T> vec)
{
    for (T i : vec)
    {
        if (i == item)
        {
            return true;
        }
    }
    return false;
}

template <class T> bool is_item_ptr_in_vector(T item, std::vector<T> vec)
{
    for (T i : vec)
    {
        if (*i == *item)
        {
            return true;
        }
    }
    return false;
}

char escaped_symbol_to_char(char symb)
{
    switch (symb)
    {
    case 'r':
        return '\r';
    case 'n':
        return '\n';
    case 't':
        return '\t';
    case '"':
        return '\"';
    case '\'':
        return '\'';
    case '\\':
        return '\\';
    default:
        return '\0';
    }
}

char str_to_char(string str)
{
    auto str_size = str.size();
    auto str_no_quote = str.substr(1, str_size - 2).c_str();
    if (str_no_quote[0] == '\\')
    {
        return escaped_symbol_to_char(str_no_quote[1]);
    }
    return str_no_quote[0];
}

} // namespace
#endif