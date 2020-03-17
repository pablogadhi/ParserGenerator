#include "dfa_builder.h"
#include "nfa_builder.h"
#include "tree_node.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <stack>
#include <vector>

using namespace std;

const vector<char> OPERATORS = {'|', '*', '.', '+', '?'};

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
    else if (c == '*' || c == '+' || c == '?')
    {
        return 3;
    }
    return 0;
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
        // TODO Chapuz corregir
        if (isalnum(c) || c == '$')
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

template <class T, class F, class G> T postfix_eval(string postfix_expr, F operations, G basic_generator)
{
    stack<T> m_stack;
    int state_index = 0;
    for (char &c : postfix_expr)
    {
        if (is_char_in_vector(OPERATORS, c))
        {
            T a, b;
            a = m_stack.top();
            m_stack.pop();

            if (is_operator_binary(c))
            {
                b = m_stack.top();
                m_stack.pop();
                m_stack.push(operations(c, state_index, vector<T>{b, a}));
            }
            else
            {
                m_stack.push(operations(c, state_index, vector<T>{a}));
            }
        }
        else
        {
            m_stack.push(basic_generator((int)c, state_index));
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
        // TODO Corregir chapuz de #
        if (last_char == '*' || last_char == '+' || last_char == '?' || (last_char == ')' && c == '(') ||
            (isalnum(last_char) && isalnum(c)) || c == '$')
        {
            out_string.push_back('.');
        }
        out_string.push_back(c);
        last_char = c;
    }

    return out_string;
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
    else if (op == '+')
    {
        auto r_copy = operands[0].make_copy(node_index);
        auto kleene_r = nfa_from_kleene(operands[0], node_index);
        return nfa_from_concat(r_copy, kleene_r);
    }
    else if (op == '?')
    {
        auto e_transition = nfa_from_transition(36, node_index);
        return nfa_from_or(operands[0], e_transition, node_index);
    }
    return StateMachine(NULL, NULL);
}

TreeNode make_syntax_tree(char symbol, int &name_index, vector<TreeNode> children)
{

    if (children.size() == 1)
    {
        return TreeNode("", symbol, make_shared<TreeNode>(children[0]));
    }
    return TreeNode("", symbol, make_shared<TreeNode>(children[0]), make_shared<TreeNode>(children[1]));
}

TreeNode basic_syntax_node_generator(int symbol, int &name)
{
    auto node = TreeNode(to_string(name), symbol);
    name++;
    return node;
}

int main(int argc, char const *argv[])
{
    string user_input;
    cout << "Escriba la expresión:" << endl;
    cin >> user_input;
    // user_input = add_concat_char(user_input);
    cout << "Con concat: " << user_input << endl;
    auto postfix_expr = infix_to_postfix(user_input);
    cout << "Postfix: " << postfix_expr << endl;
    auto nfa = postfix_eval<StateMachine, function<StateMachine(char, int &, vector<StateMachine>)>,
                            function<StateMachine(int, int &)>>(postfix_expr, nfa_operations, nfa_from_transition);
    nfa.draw_machine("NFA.dot");

    auto dfa_0 = dfa_from_nfa(nfa);
    dfa_0.draw_machine("DFA_FROM_NFA.dot");

    // auto postfix_expr_sharp = postfix_expr;
    // postfix_expr_sharp.push_back('#');
    // postfix_expr_sharp.push_back('.');
    // auto syntax_tree =
    //     postfix_eval<TreeNode, function<TreeNode(char, int &, vector<TreeNode>)>, function<TreeNode(int, int &)>>(
    //         postfix_expr_sharp, make_syntax_tree, basic_syntax_node_generator);
    // auto direct_dfa = dfa_from_syntax_tree(syntax_tree);

    return 0;
}