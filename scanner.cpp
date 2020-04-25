#include "scanner.h"
#include "iostream"

Token::Token()
{
}

Token::Token(string name, string val) : t_name(name), t_val(val)
{
}

Token::~Token()
{
}

bool Token::empty()
{
    return t_val.empty();
}

string Token::name()
{
    return t_name;
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

    char_map["operator"] = Set<char>(vector<char>{'|', '*', '+', '?', '\0'});
    char_map["special"] = Set<char>(vector<char>{'(', ')'});
    char_map["="] = Set<char>(vector<char>{'='});
    char_map["-"] = Set<char>(vector<char>{'-'});
    char_map["."] = Set<char>(vector<char>{'.'});
    char_map["{"] = Set<char>(vector<char>{'{'});
    char_map["}"] = Set<char>(vector<char>{'}'});
    char_map["\""] = Set<char>(vector<char>{'\"'});
    char_map["\'"] = Set<char>(vector<char>{'\''});
    char_map["\\"] = Set<char>(vector<char>{'\\'});
    char_map["cr"] = Set<char>(vector<char>{'\r'});
    char_map["lf"] = Set<char>(vector<char>{'\n'});
    char_map["tab"] = Set<char>(vector<char>{'\t'});
    char_map["space"] = Set<char>(vector<char>{' '});
    auto letter_set = Set<char>();
    for (char c = 'A'; c <= 'Z'; c++)
    {
        letter_set.add(c);
    }

    for (char c = 'a'; c <= 'z'; c++)
    {
        letter_set.add(c);
    }
    letter_set.add('_');
    char_map["letter"] = letter_set;

    auto digit_set = Set<char>();
    for (char c = '0'; c <= '9'; c++)
    {
        digit_set.add(c);
    }
    char_map["digit"] = digit_set;

    auto hex_letter = Set<char>();
    for (char c = 'a'; c <= 'f'; c++)
    {
        hex_letter.add(c);
    }
    char_map["hex"] = union_between_sets(digit_set, hex_letter);

    auto printable = Set<char>();
    for (char c = ' '; c <= '~'; c++)
    {
        printable.add(c);
    }
    printable = diff_between_sets(printable, char_map["operator"]);
    printable = diff_between_sets(printable, char_map["special"]);
    char_map["printable"] = printable;

    read_into_string_buffer(buffer_0);
    current_buffer = buffer_0;
    prev_buffer = buffer_0;
    lexeme_begin_idx = 0;
    forward = current_buffer.begin();
}

Scanner::~Scanner()
{
    file_buffer.close();
}

void Scanner::set_finder(DFA dfa)
{
    finder = dfa;
}

unordered_map<string, Set<char>> Scanner::get_char_map()
{
    return char_map;
}

void Scanner::read_into_string_buffer(string &out_buffer)
{
    char *temp_buffer = new char[2048];
    file_buffer.read(temp_buffer, 2048);
    out_buffer = string(temp_buffer);
    // out_buffer = out_buffer + '\0';
    delete temp_buffer;
}

void Scanner::next_char()
{
    forward++;
    if (forward == current_buffer.end())
    {
        if (file_buffer.eof())
        {
            forward = current_buffer.end();
        }
        else if (current_buffer == buffer_0)
        {
            read_into_string_buffer(buffer_1);
            current_buffer = buffer_1;
            forward = current_buffer.begin();
        }
        else if (current_buffer == buffer_1)
        {
            read_into_string_buffer(buffer_0);
            current_buffer = buffer_0;
            forward = current_buffer.begin();
        }
    }
}

char Scanner::peek_char()
{
    char peeked;
    forward++;
    if (forward == current_buffer.end())
    {
        peeked = file_buffer.peek();
    }
    else
    {
        peeked = *forward;
    }
    forward--;
    return peeked;
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

    // Handle EOF
    if (forward == current_buffer.end())
    {
        return Token("EOF", string(1, '\0'));
    }

    lexeme_begin_idx = forward - current_buffer.begin();

    // Move until an accepting state is found
    bool accepting_found = false;
    State peeked_state = State();
    // TODO Change second criteria to "Couldn't go forward"
    while (!accepting_found || peeked_state.is_accepting())
    {
        finder.move(*forward);
        // TODO handle unknown characters
        if (finder.current().is_accepting())
        {
            accepting_found = true;
            char peeked_c = peek_char();
            peeked_state = finder.peek_move(peeked_c);
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
    return Token("", lexeme_str);
}