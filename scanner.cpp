#include "scanner.h"
#include "iostream"

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

    char_map["operator"] = Set<string>(vector<string>{"|", "*", "+", "?", "\0"});
    char_map["special"] = Set<string>(vector<string>{"(", ")", "\"", "\\"});
    auto letter_set = Set<string>();
    for (char c = 'A'; c <= 'Z'; c++)
    {
        letter_set.add(string(1, c));
    }

    for (char c = 'a'; c <= 'z'; c++)
    {
        letter_set.add(string(1, c));
    }
    char_map["letter"] = letter_set;
    char_map["COMPILER"] = Set<string>(vector<string>{"COMPILER"});
    char_map["CHARACTERS"] = Set<string>(vector<string>{"CHARACTERS"});
    char_map["KEYWORDS"] = Set<string>(vector<string>{"KEYWORDS"});

    read_into_string_buffer(buffer_0);
    current_buffer = buffer_0;
    prev_buffer = buffer_0;
    lexeme_begin_idx = 0;
    forward = current_buffer.begin();
    // n_token = next_token();
}

Scanner::~Scanner()
{
    file_buffer.close();
}

void Scanner::set_finder(DFA dfa)
{
    finder = dfa;
}

unordered_map<string, Set<string>> Scanner::get_char_map()
{
    return char_map;
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