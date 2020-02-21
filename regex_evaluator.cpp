#include "dfa_builder.h"
#include "nfa_builder.h"
#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>

using namespace std;

const vector<char> OPERATORS = {'|', '*', '.'};

int get_precedence(char c)
{
    if (c == '|')
    {
        return 1;
    }
    else if (c == '.')
    {
        return 2;
    }
    else if (c == '*')
    {
        return 3;
    }
    return 0;
}

StateMachine nfa_operations(char op, int &node_index, vector<StateMachine> operands)
{
    if (op == '|')
    {
        return nfa_from_or(operands[0], operands[1], node_index);
    }
    else if (op == '.')
    {
        return nfa_from_concat(operands[0], operands[1]);
    }
    else if (op == '*')
    {
        return nfa_from_kleene(operands[0], node_index);
    }
    return StateMachine(NULL, NULL);
}

bool is_operator_binary(char op)
{
    if (op == '|' || op == '.')
    {
        return true;
    }
    return false;
}

bool is_char_in_vector(vector<char> vector_to_check, char c)
{
    return find(vector_to_check.begin(), vector_to_check.end(), c) != vector_to_check.end();
}

string infix_to_postfix(string infix_string)
{
    stack<char> char_stack;
    string output = "";

    for (char &c : infix_string)
    {
        if (isalnum(c))
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
        else if (is_char_in_vector(OPERATORS, c))
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

StateMachine postfix_eval(string postfix_expr)
{
    stack<StateMachine> m_stack;
    int state_index = 0;
    for (char &c : postfix_expr)
    {
        if (is_char_in_vector(OPERATORS, c))
        {
            StateMachine a, b;
            a = m_stack.top();
            m_stack.pop();

            if (is_operator_binary(c))
            {
                b = m_stack.top();
                m_stack.pop();
                m_stack.push(nfa_operations(c, state_index, vector<StateMachine>{b, a}));
            }
            else
            {
                m_stack.push(nfa_operations(c, state_index, vector<StateMachine>{a}));
            }
        }
        else
        {
            m_stack.push(nfa_from_transition((int)c, state_index));
        }
    }
    return m_stack.top();
}

string add_concat_char(string original_string)
{
    string out_string;
    char last_char;
    for (char &c : original_string)
    {
        if (last_char == '*' || (last_char == ')' && c == '(') || (isalnum(last_char) && isalnum(c)))
        {
            out_string.push_back('.');
        }
        out_string.push_back(c);
        last_char = c;
    }

    return out_string;
}

int main(int argc, char const *argv[])
{
    string user_input;
    cout << "Escriba la expresión:" << endl;
    cin >> user_input;
    user_input = add_concat_char(user_input);
    cout << "Con concat: " << user_input << endl;
    string postfix_expr = infix_to_postfix(user_input);
    cout << "Postfix: " << postfix_expr << endl;
    StateMachine machine = postfix_eval(postfix_expr);
    machine.print_machine();
    dfa_from_nfa(machine);

    cout << endl;
    // dfa.print_machine();
    // cout << "Resultado: " << postfix_eval(postfix_expr, OPERATORS) << endl;
    return 0;
}