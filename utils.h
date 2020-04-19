#ifndef UTILS_H
#define UTILS_H

#include "set.h"
#include <stack>
#include <string>
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

template <class OpertatorType, class PrecedenceFunc>
string infix_to_postfix(string infix_string, Set<OpertatorType> operators, PrecedenceFunc get_precedence)
{
    stack<char> char_stack;
    string output = "";

    for (char &c : infix_string)
    {
        // TODO Chapuz corregir
        if (isalnum(c) || c == '$' || c == '#')
        {
            output.push_back(c);
        }
        else if (c == '(')
        {
            char_stack.push(c);
        }
        else if (c == ')')
        {
            char op;
            while (char_stack.top() != '(')
            {
                output.push_back(char_stack.top());
                char_stack.pop();
            }
            char_stack.pop();
        }
        else if (operators.has_item(c) != -1)
        {
            while (!char_stack.empty() && get_precedence(c) <= get_precedence(char_stack.top()))
            {
                output.push_back(char_stack.top());
                char_stack.pop();
            }

            char_stack.push(c);
        }
    }

    while (!char_stack.empty())
    {
        output.push_back(char_stack.top());
        char_stack.pop();
    }

    return output;
}

bool is_operator_binary(char op)
{
    if (op == '|' || op == '.')
    {
        return true;
    }
    return false;
}

template <class Operand, class OP_Function, class Gen_Function, class OpertatorType>
Operand postfix_eval(string postfix_expr, OP_Function operations, Gen_Function basic_generator,
                     Set<OpertatorType> operators)
{
    stack<Operand> m_stack;
    int state_index = 0;
    for (char &c : postfix_expr)
    {
        if (operators.has_item(c) != -1)
        {
            Operand a, b;
            a = m_stack.top();
            m_stack.pop();

            if (is_operator_binary(c))
            {
                b = m_stack.top();
                m_stack.pop();
                m_stack.push(operations(c, state_index, vector<Operand>{b, a}));
            }
            else
            {
                m_stack.push(operations(c, state_index, vector<Operand>{a}));
            }
        }
        else
        {
            m_stack.push(basic_generator((int)c, state_index));
        }
    }
    return m_stack.top();
}
} // namespace
#endif