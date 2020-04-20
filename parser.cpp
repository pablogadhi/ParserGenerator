#include "parser.h"
#include "dfa_builder.h"
#include <iostream>
#include <stack>

int get_precedence(string c)
{
    if (c == "|")
    {
        return 1;
    }
    if (c == "\0")
    {
        return 2;
    }
    else if (c == "*" || c == "+" || c == "?")
    {
        return 3;
    }
    return 0;
}

vector<Set<string>> infix_to_postfix(vector<Set<string>> infix_regex, unordered_map<string, Set<string>> language_map,
                                     function<int(string)> get_precedence)
{
    stack<Set<string>> char_stack;
    vector<Set<string>> output;

    auto operators = language_map["operator"];
    auto special_symbols = language_map["special"];

    for (auto &t : infix_regex)
    {
        if (intersec_between_sets<string>(t, operators).size() == 0 &&
            intersec_between_sets<string>(t, special_symbols).size() == 0)
        {
            output.push_back(t);
        }
        else if (t.has_item("(") != -1)
        {
            char_stack.push(t);
        }
        else if (t.has_item(")") != -1)
        {
            while (char_stack.top().has_item("("))
            {
                output.push_back(char_stack.top());
                char_stack.pop();
            }
            char_stack.pop();
        }
        else if (intersec_between_sets<string>(t, operators).size() != 0)
        {
            while (!char_stack.empty() &&
                   get_precedence(t.get_items()[0]) <= get_precedence(char_stack.top().get_items()[0]))
            {
                output.push_back(char_stack.top());
                char_stack.pop();
            }

            char_stack.push(t);
        }
    }

    while (!char_stack.empty())
    {
        output.push_back(char_stack.top());
        char_stack.pop();
    }

    return output;
}

TreeNode<int> make_syntax_tree(Set<string> symbol_as_set, int &name_index, vector<TreeNode<int>> children)
{
    // TODO Handle symbols with more than one character
    int symbol = (int)(symbol_as_set.get_items()[0].c_str()[0]);
    if (children.size() == 1)
    {
        return TreeNode<int>(-1, symbol, make_shared<TreeNode<int>>(children[0]));
    }
    return TreeNode<int>(-1, symbol, make_shared<TreeNode<int>>(children[0]), make_shared<TreeNode<int>>(children[1]));
}

TreeNode<int> tree_from_set(Set<string> set, int &name_index)
{
    stack<TreeNode<int>> str_stack, char_stack;
    for (auto &s : set)
    {
        for (auto &c : s)
        {
            auto right = TreeNode<int>(name_index, (int)c);
            name_index++;
            if (char_stack.empty())
            {
                char_stack.push(right);
            }
            else
            {
                auto left = char_stack.top();
                char_stack.pop();
                char_stack.push(
                    TreeNode<int>(-1, 0, make_shared<TreeNode<int>>(left), make_shared<TreeNode<int>>(right)));
            }
        }
        auto right_str_node = char_stack.top();
        char_stack.pop();
        if (str_stack.empty())
        {
            str_stack.push(right_str_node);
        }
        else
        {
            auto left_str_node = str_stack.top();
            str_stack.pop();
            str_stack.push(TreeNode<int>(-1, 124, make_shared<TreeNode<int>>(left_str_node),
                                         make_shared<TreeNode<int>>(right_str_node)));
        }
    }
    auto last = str_stack.top();
    auto kleene_node = TreeNode<int>(-1, 42, make_shared<TreeNode<int>>(last));
    return kleene_node;
}

TreeNode<int> basic_syntax_node_generator(Set<string> symbol, int &name)
{
    // auto node = TreeNode<Set<string>>(name, symbol);
    TreeNode<int> node;
    if (symbol.size() > 1)
    {
        node = tree_from_set(symbol, name);
    }
    else
    {
        // TODO Handle symbols with more than one character
        node = TreeNode<int>(name, (int)(symbol.get_items()[0].c_str()[0]));
        name++;
    }
    return node;
}

bool is_operator_binary(Set<string> op)
{
    if (op.get_items()[0] == "|" || op.get_items()[0] == "\0")
    {
        return true;
    }
    return false;
}

TreeNode<int> postfix_eval(vector<Set<string>> postfix_expr, Set<string> operators)
{
    stack<TreeNode<int>> m_stack;
    int state_index = 0;
    for (auto &t : postfix_expr)
    {
        TreeNode<int> a, b;
        if (intersec_between_sets<string>(t, operators).size() != 0)
        {
            a = m_stack.top();
            m_stack.pop();

            if (is_operator_binary(t))
            {
                b = m_stack.top();
                m_stack.pop();
                m_stack.push(make_syntax_tree(t, state_index, vector<TreeNode<int>>{b, a}));
            }
            else
            {
                m_stack.push(make_syntax_tree(t, state_index, vector<TreeNode<int>>{a}));
            }
        }
        else
        {
            // if (m_stack.size() >= 2)
            // {
            //     a = m_stack.top();
            //     m_stack.pop();
            //     b = m_stack.top();
            //     m_stack.pop();
            //     auto concat_set = Set<string>(vector<string>({"\0"}));
            //     m_stack.push(make_syntax_tree(concat_set, state_index, vector<TreeNode<Set<string>>>{b, a}));
            // }
            m_stack.push(basic_syntax_node_generator(t, state_index));
        }
    }
    return m_stack.top();
}

vector<Set<string>> format_regex(vector<Set<string>> regex, Set<string> operators)
{
    vector<Set<string>> result;
    for (int i = 0; i < regex.size() - 1; i++)
    {
        Set<string> first = regex[i], second = regex[i + 1];
        result.push_back(first);
        if (first.has_item("(") == -1 && second.has_item(")") == -1 &&
            intersec_between_sets(operators, second).size() == 0 && !is_operator_binary(first))
        {
            result.push_back(Set<string>(vector<string>{"\0"}));
        }
    }
    result.push_back(regex[regex.size() - 1]);
    return result;
}

TreeNode<int> expand_tree(shared_ptr<TreeNode<Set<string>>> root, int &name_index)
{
    shared_ptr<TreeNode<int>> left_child, right_child;
    if (root->left() != nullptr)
    {
        auto left = expand_tree(root->left(), name_index);
        left_child = make_shared<TreeNode<int>>(left);
    }

    if (root->right() != nullptr)
    {
        auto right = expand_tree(root->right(), name_index);
        right_child = make_shared<TreeNode<int>>(right);
    }

    TreeNode<int> new_tree;
    if (root->symbol().size() > 1)
    {
        new_tree = tree_from_set(root->symbol(), name_index);
    }
    else
    {
        // TODO Handle symbols with more than one character
        new_tree = TreeNode<int>(name_index, (int)(root->symbol()[0].c_str()[0]));
        name_index++;
        new_tree.set_left(left_child);
        new_tree.set_right(right_child);
    }
    return new_tree;
}

Parser::Parser(Scanner &sc) : scanner(sc)
{

    string all_letter_regex = "(";
    for (auto &c : scanner.get_char_map()["letter"].get_items())
    {
        all_letter_regex = all_letter_regex + c + "|";
    }
    all_letter_regex.pop_back();
    all_letter_regex = all_letter_regex + ")*";
    all_letter_regex = all_letter_regex + ".#";

    auto a = Set<string>(vector<string>{"a", "b", "c"});
    auto b = Set<string>(vector<string>{"1", "2", "3"});
    auto o = Set<string>(vector<string>{"|"});
    auto p1 = Set<string>(vector<string>{"("});
    auto p2 = Set<string>(vector<string>{")"});
    auto kleene = Set<string>(vector<string>{"*"});
    auto hashtag = Set<string>(vector<string>{"#"});
    auto test_regex = vector<Set<string>>{p1, a, o, b, p2, hashtag};

    test_regex = format_regex(test_regex, scanner.get_char_map()["operator"]);
    auto postfix_regex = infix_to_postfix(test_regex, scanner.get_char_map(), get_precedence);
    auto syntax_tree = postfix_eval(postfix_regex, scanner.get_char_map()["operator"]);
    auto dfa_b = DFABuilder(scanner.get_char_map());
    auto finder = dfa_b.dfa_from_syntax_tree(syntax_tree);
    finder.draw_machine("Scanner_DFA");
    scanner.set_finder(finder);
}

Parser::~Parser()
{
}

void Parser::parse()
{
    la_token = scanner.scan();
    for (int i = 0; i < 10; i++)
    {
        c_token = la_token;
        cout << c_token.value() << endl;
        la_token = scanner.scan();
    }
}