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

void SymbolTable::add_char_set(string set_name, Set<char> new_set)
{
    char_set_map[set_name] = new_set;
}
void SymbolTable::add_keyword(string name, string new_keyword)
{
    keywords_map[name] = new_keyword;
}
unordered_map<string, Set<char>> &SymbolTable::char_sets()
{
    return char_set_map;
}
unordered_map<string, string> &SymbolTable::keywords()
{
    return keywords_map;
}

Scanner::Scanner()
{
}

Scanner::Scanner(string file_name)
{
    file_buffer.open(file_name);
    if (!file_buffer.is_open())
    {
        cout << "Failed to open the file!" << endl;
        throw std::logic_error("Failed to open the file!");
    }

    Set<char> operators = Set<char>{'|', '}', ']', '\0'};
    Set<char> special_chars = Set<char>{'(', ')', '{', '['};

    // Initialize Character Map
    s_table.add_char_set("=", Set<char>{'='});
    s_table.add_char_set("-", Set<char>{'-'});
    s_table.add_char_set(".", Set<char>{'.'});
    s_table.add_char_set("|", Set<char>{'|'});
    s_table.add_char_set("{", Set<char>{'{'});
    s_table.add_char_set("}", Set<char>{'}'});
    s_table.add_char_set("[", Set<char>{'['});
    s_table.add_char_set("]", Set<char>{']'});
    s_table.add_char_set("(", Set<char>{'('});
    s_table.add_char_set(")", Set<char>{')'});
    s_table.add_char_set("\"", Set<char>{'\"'});
    s_table.add_char_set("\'", Set<char>{'\''});
    s_table.add_char_set("\\", Set<char>{'\\'});
    s_table.add_char_set("cr", Set<char>{'\r'});
    s_table.add_char_set("lf", Set<char>{'\n'});
    s_table.add_char_set("tab", Set<char>{'\t'});
    s_table.add_char_set("space", Set<char>{' '});
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
    s_table.add_char_set("letter", letter_set);

    auto digit_set = Set<char>();
    for (char c = '0'; c <= '9'; c++)
    {
        digit_set.add(c);
    }
    s_table.add_char_set("digit", digit_set);

    auto hex_letter = Set<char>();
    for (char c = 'a'; c <= 'f'; c++)
    {
        hex_letter.add(c);
    }
    s_table.add_char_set("hex", union_between_sets(digit_set, hex_letter));

    auto printable = Set<char>();
    for (char c = ' '; c <= '~'; c++)
    {
        printable.add(c);
    }
    printable = diff_between_sets(printable, operators);
    printable = diff_between_sets(printable, special_chars);
    s_table.add_char_set("printable", printable);

    auto ANY = Set<char>();
    for (auto &[key, set] : s_table.char_sets())
    {
        ANY = union_between_sets<char>(ANY, set);
    }
    ANY = diff_between_sets(ANY, operators);
    ANY = diff_between_sets(ANY, special_chars);
    s_table.add_char_set("ANY", ANY);

    auto stringCh = diff_between_sets(ANY, s_table.char_sets()["\""]);
    stringCh = diff_between_sets(ANY, s_table.char_sets()["\\"]);
    stringCh = diff_between_sets(ANY, s_table.char_sets()["cr"]);
    stringCh = diff_between_sets(ANY, s_table.char_sets()["lf"]);
    s_table.add_char_set("stringCh", stringCh);

    auto charCh = diff_between_sets(ANY, s_table.char_sets()["\'"]);
    charCh = diff_between_sets(ANY, s_table.char_sets()["\\"]);
    charCh = diff_between_sets(ANY, s_table.char_sets()["cr"]);
    charCh = diff_between_sets(ANY, s_table.char_sets()["lf"]);
    s_table.add_char_set("charCh", charCh);

    // Initialize keywords
    s_table.add_keyword("COMPILER", "COMPILER");
    s_table.add_keyword("CHARACTERS", "CHARACTERS");
    s_table.add_keyword("KEYWORDS", "KEYWORDS");
    s_table.add_keyword("EXCEPT", "EXCEPT");
    s_table.add_keyword("TOKENS", "TOKENS");
    s_table.add_keyword("END", "END");
    s_table.add_keyword("ANY", "ANY");

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

DFA &Scanner::get_finder()
{
    return finder;
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

void Scanner::ignore_all_blank_chars()
{
    while (*forward == ' ' || *forward == '\t' || *forward == '\n' || *forward == '\r')
    {
        next_char();
    }
}

Token Scanner::next_token()
{
    prev_buffer = current_buffer;
    ignore_all_blank_chars();

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

    string token_name = finder.current().reference_name();
    if (s_table.keywords().find(lexeme_str) != s_table.keywords().end())
    {
        ignore_all_blank_chars();
        // QuickFix for reading keywords in the COCOL grammar
        if (*forward != '=')
        {
            token_name = s_table.keywords()[lexeme_str];
        }
    }

    finder.reset_movements();
    return Token(token_name, lexeme_str);
}

Token Scanner::current()
{
    return c_token;
}

Token Scanner::look_ahead()
{
    return n_token;
}

SymbolTable &Scanner::symbols()
{
    return s_table;
}