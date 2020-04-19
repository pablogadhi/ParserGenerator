#include "scanner.h"
#include "dfa_builder.h"
#include "iostream"
#include "tree_node.h"
#include "utils.h"

Token::Token()
{
}

Token::Token(string str) : t_val(str)
{
}

Token::~Token()
{
}

bool Token::empty()
{
    return t_val.empty();
}

string Token::value()
{
    return t_val;
}

void Token::add_char(char c)
{
    t_val.push_back(c);
}

token_type Token::type()
{
    return t_type;
}

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

TreeNode make_syntax_tree(char symbol, int &name_index, vector<TreeNode> children)
{

    if (children.size() == 1)
    {
        return TreeNode(-1, symbol, make_shared<TreeNode>(children[0]));
    }
    return TreeNode(-1, symbol, make_shared<TreeNode>(children[0]), make_shared<TreeNode>(children[1]));
}

TreeNode basic_syntax_node_generator(int symbol, int &name)
{
    auto node = TreeNode(name, symbol);
    name++;
    return node;
}

Scanner::Scanner()
{
}

Scanner::Scanner(string file_name)
{
    file_buffer.open(file_name);
    if (!file_buffer.is_open())
    {
        throw std::logic_error("Failed to open the file!");
    }

    // Initialize Character Map

    char_map["operators"] = Set<char>(vector<char>({'|', '*', '+', '?', '.'}));
    char_map["special"] = Set<char>(vector<char>{'"', '\\'});
    auto letter_set = Set<char>();
    for (char c = 'A'; c <= 'Z'; c++)
    {
        letter_set.add(c);
    }

    for (char c = 'a'; c <= 'z'; c++)
    {
        letter_set.add(c);
    }

    string all_letter_regex = "(";
    for (auto &c : letter_set.get_items())
    {
        all_letter_regex = all_letter_regex + c + "|";
    }
    all_letter_regex.pop_back();
    all_letter_regex = all_letter_regex + ")*";
    all_letter_regex = all_letter_regex + ".#";

    // Insert DFA initializer
    all_letter_regex =
        infix_to_postfix<char, function<int(char)>>(all_letter_regex, char_map["operators"], get_precedence);
    auto syntax_tree =
        postfix_eval<TreeNode, function<TreeNode(char, int &, vector<TreeNode>)>, function<TreeNode(int, int &)>, char>(
            all_letter_regex, make_syntax_tree, basic_syntax_node_generator, char_map["operators"]);
    finder = dfa_from_syntax_tree(syntax_tree);
    // finder.draw_machine("Scanner_DFA");

    read_into_string_buffer(buffer_0);
    current_buffer = buffer_0;
    prev_buffer = buffer_0;
    lexeme_begin_idx = 0;
    forward = current_buffer.begin();
    n_token = next_token();
}

Scanner::~Scanner()
{
    file_buffer.close();
}

void Scanner::read_into_string_buffer(string &out_buffer)
{
    char *temp_buffer = new char[8];
    file_buffer.read(temp_buffer, 8);
    out_buffer = string(temp_buffer);
    // out_buffer = out_buffer + '\0';
    delete temp_buffer;
}

void Scanner::next_char()
{
    forward++;
    if (current_buffer == buffer_0 && forward == current_buffer.end())
    {
        read_into_string_buffer(buffer_1);
        current_buffer = buffer_1;
        forward = current_buffer.begin();
    }
    else if (current_buffer == buffer_1 && forward == current_buffer.end())
    {
        read_into_string_buffer(buffer_0);
        current_buffer = buffer_0;
        forward = current_buffer.begin();
    }
}

Token Scanner::scan()
{
    c_token = n_token;
    n_token = next_token();
    return c_token;
}

Token Scanner::next_token()
{
    prev_buffer = current_buffer;
    while (*forward == ' ' || *forward == '\t' || *forward == '\n' || *forward == '\r')
    {
        next_char();
    }

    lexeme_begin_idx = forward - current_buffer.begin();

    while (true)
    {
        finder.move(*forward);
        if (!finder.current().is_accepting())
        {
            break;
        }
        next_char();
    }

    string lexeme_str;
    if (current_buffer != prev_buffer)
    {
        auto l_end_len = forward - current_buffer.begin();
        lexeme_str = prev_buffer.substr(lexeme_begin_idx) + current_buffer.substr(0, l_end_len);
    }
    else
    {
        auto l_len = forward - current_buffer.begin() - lexeme_begin_idx;
        lexeme_str = current_buffer.substr(lexeme_begin_idx, l_len);
    }

    finder.reset_movements();
    return Token(lexeme_str);
}