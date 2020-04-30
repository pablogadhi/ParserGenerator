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

SymbolTable &Scanner::symbols()
{
    return s_table;
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

    // Initialize Character Map
    s_table.add_char_set("charCh", Set<char>{'\t', ' ', '!', '\"', '#', '$', '%', '&', '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '~'});
    s_table.add_char_set("stringCh", Set<char>{'\t', ' ', '!', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '~'});
    s_table.add_char_set("hex", Set<char>{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'});
    s_table.add_char_set("attrCh", Set<char>{'\t', '\n', '\r', ' ', '!', '\"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '=', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '~'});
    s_table.add_char_set("printable", Set<char>{' ', '!', '\"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '~'});
    s_table.add_char_set("lf", Set<char>{'\n'});
    s_table.add_char_set("letter", Set<char>{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '_'});
    s_table.add_char_set("tab", Set<char>{'\t'});
    s_table.add_char_set("digit", Set<char>{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});
    s_table.add_char_set("cr", Set<char>{'\r'});

    // Initialize keywords
    s_table.add_keyword("EXCEPT", "EXCEPT");
    s_table.add_keyword("ANY", "ANY");
    s_table.add_keyword("CHARACTERS", "CHARACTERS");
    s_table.add_keyword("COMPILER", "COMPILER");
    s_table.add_keyword("KEYWORDS", "KEYWORDS");
    s_table.add_keyword("TOKENS", "TOKENS");
    s_table.add_keyword("END", "END");

    // Initialize DFA
    unordered_map<int, shared_ptr<State>> state_ptrs;
    auto state_0 = State(0, 0, "");
    state_ptrs[0] = make_shared<State>(state_0);
    auto state_1 = State(1, 0, "");
    state_ptrs[1] = make_shared<State>(state_1);
    auto state_18 = State(18, 1, "attr");
    state_ptrs[18] = make_shared<State>(state_18);
    auto state_2 = State(2, 0, "");
    state_ptrs[2] = make_shared<State>(state_2);
    auto state_19 = State(19, 1, "string");
    state_ptrs[19] = make_shared<State>(state_19);
    auto state_20 = State(20, 0, "");
    state_ptrs[20] = make_shared<State>(state_20);
    auto state_3 = State(3, 0, "");
    state_ptrs[3] = make_shared<State>(state_3);
    auto state_21 = State(21, 0, "");
    state_ptrs[21] = make_shared<State>(state_21);
    auto state_25 = State(25, 1, "char");
    state_ptrs[25] = make_shared<State>(state_25);
    auto state_22 = State(22, 0, "");
    state_ptrs[22] = make_shared<State>(state_22);
    auto state_26 = State(26, 0, "");
    state_ptrs[26] = make_shared<State>(state_26);
    auto state_4 = State(4, 1, "(");
    state_ptrs[4] = make_shared<State>(state_4);
    auto state_5 = State(5, 1, ")");
    state_ptrs[5] = make_shared<State>(state_5);
    auto state_6 = State(6, 1, "+");
    state_ptrs[6] = make_shared<State>(state_6);
    auto state_7 = State(7, 1, "-");
    state_ptrs[7] = make_shared<State>(state_7);
    auto state_8 = State(8, 1, ".");
    state_ptrs[8] = make_shared<State>(state_8);
    auto state_23 = State(23, 1, "..");
    state_ptrs[23] = make_shared<State>(state_23);
    auto state_9 = State(9, 1, "number");
    state_ptrs[9] = make_shared<State>(state_9);
    auto state_10 = State(10, 1, "=");
    state_ptrs[10] = make_shared<State>(state_10);
    auto state_11 = State(11, 1, "ident");
    state_ptrs[11] = make_shared<State>(state_11);
    auto state_12 = State(12, 1, "ident");
    state_ptrs[12] = make_shared<State>(state_12);
    auto state_24 = State(24, 1, "ident");
    state_ptrs[24] = make_shared<State>(state_24);
    auto state_27 = State(27, 1, "ident");
    state_ptrs[27] = make_shared<State>(state_27);
    auto state_28 = State(28, 1, "char");
    state_ptrs[28] = make_shared<State>(state_28);
    auto state_13 = State(13, 1, "[");
    state_ptrs[13] = make_shared<State>(state_13);
    auto state_14 = State(14, 1, "{");
    state_ptrs[14] = make_shared<State>(state_14);
    auto state_15 = State(15, 1, "]");
    state_ptrs[15] = make_shared<State>(state_15);
    auto state_16 = State(16, 1, "|");
    state_ptrs[16] = make_shared<State>(state_16);
    auto state_17 = State(17, 1, "}");
    state_ptrs[17] = make_shared<State>(state_17);
    state_ptrs[0]->add_t_function(make_pair(60, state_ptrs[1]));
    state_ptrs[0]->add_t_function(make_pair(34, state_ptrs[2]));
    state_ptrs[0]->add_t_function(make_pair(39, state_ptrs[3]));
    state_ptrs[0]->add_t_function(make_pair(40, state_ptrs[4]));
    state_ptrs[0]->add_t_function(make_pair(41, state_ptrs[5]));
    state_ptrs[0]->add_t_function(make_pair(43, state_ptrs[6]));
    state_ptrs[0]->add_t_function(make_pair(45, state_ptrs[7]));
    state_ptrs[0]->add_t_function(make_pair(46, state_ptrs[8]));
    state_ptrs[0]->add_t_function(make_pair(48, state_ptrs[9]));
    state_ptrs[0]->add_t_function(make_pair(49, state_ptrs[9]));
    state_ptrs[0]->add_t_function(make_pair(50, state_ptrs[9]));
    state_ptrs[0]->add_t_function(make_pair(51, state_ptrs[9]));
    state_ptrs[0]->add_t_function(make_pair(52, state_ptrs[9]));
    state_ptrs[0]->add_t_function(make_pair(53, state_ptrs[9]));
    state_ptrs[0]->add_t_function(make_pair(54, state_ptrs[9]));
    state_ptrs[0]->add_t_function(make_pair(55, state_ptrs[9]));
    state_ptrs[0]->add_t_function(make_pair(56, state_ptrs[9]));
    state_ptrs[0]->add_t_function(make_pair(57, state_ptrs[9]));
    state_ptrs[0]->add_t_function(make_pair(61, state_ptrs[10]));
    state_ptrs[0]->add_t_function(make_pair(65, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(66, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(67, state_ptrs[12]));
    state_ptrs[0]->add_t_function(make_pair(68, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(69, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(70, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(71, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(72, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(73, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(74, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(75, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(76, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(77, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(78, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(79, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(80, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(81, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(82, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(83, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(84, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(85, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(86, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(87, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(88, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(89, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(90, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(91, state_ptrs[13]));
    state_ptrs[0]->add_t_function(make_pair(95, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(97, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(98, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(99, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(100, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(101, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(102, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(103, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(104, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(105, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(106, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(107, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(108, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(109, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(110, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(111, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(112, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(113, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(114, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(115, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(116, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(117, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(118, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(119, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(120, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(121, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(122, state_ptrs[11]));
    state_ptrs[0]->add_t_function(make_pair(123, state_ptrs[14]));
    state_ptrs[0]->add_t_function(make_pair(93, state_ptrs[15]));
    state_ptrs[0]->add_t_function(make_pair(124, state_ptrs[16]));
    state_ptrs[0]->add_t_function(make_pair(125, state_ptrs[17]));
    state_ptrs[1]->add_t_function(make_pair(9, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(10, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(13, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(32, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(33, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(34, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(35, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(36, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(37, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(38, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(39, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(40, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(41, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(42, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(43, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(44, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(45, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(46, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(47, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(48, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(49, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(50, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(51, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(52, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(53, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(54, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(55, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(56, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(57, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(58, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(59, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(61, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(63, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(64, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(65, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(66, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(67, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(68, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(69, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(70, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(71, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(72, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(73, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(74, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(75, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(76, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(77, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(78, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(79, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(80, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(81, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(82, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(83, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(84, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(85, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(86, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(87, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(88, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(89, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(90, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(91, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(92, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(94, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(95, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(96, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(97, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(98, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(99, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(100, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(101, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(102, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(103, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(104, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(105, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(106, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(107, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(108, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(109, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(110, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(111, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(112, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(113, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(114, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(115, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(116, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(117, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(118, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(119, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(120, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(121, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(122, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(123, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(126, state_ptrs[1]));
    state_ptrs[1]->add_t_function(make_pair(62, state_ptrs[18]));
    state_ptrs[2]->add_t_function(make_pair(60, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(9, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(32, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(33, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(34, state_ptrs[19]));
    state_ptrs[2]->add_t_function(make_pair(35, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(36, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(37, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(38, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(39, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(40, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(41, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(42, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(43, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(44, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(45, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(46, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(47, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(48, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(49, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(50, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(51, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(52, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(53, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(54, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(55, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(56, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(57, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(58, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(59, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(61, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(63, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(64, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(65, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(66, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(67, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(68, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(69, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(70, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(71, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(72, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(73, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(74, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(75, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(76, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(77, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(78, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(79, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(80, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(81, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(82, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(83, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(84, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(85, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(86, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(87, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(88, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(89, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(90, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(91, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(92, state_ptrs[20]));
    state_ptrs[2]->add_t_function(make_pair(94, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(95, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(96, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(97, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(98, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(99, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(100, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(101, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(102, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(103, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(104, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(105, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(106, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(107, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(108, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(109, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(110, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(111, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(112, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(113, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(114, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(115, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(116, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(117, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(118, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(119, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(120, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(121, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(122, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(123, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(126, state_ptrs[2]));
    state_ptrs[2]->add_t_function(make_pair(62, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(60, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(32, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(33, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(34, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(35, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(36, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(37, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(38, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(39, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(40, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(41, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(42, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(43, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(44, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(45, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(46, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(47, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(48, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(49, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(50, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(51, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(52, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(53, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(54, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(55, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(56, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(57, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(58, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(59, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(61, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(63, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(64, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(65, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(66, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(67, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(68, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(69, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(70, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(71, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(72, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(73, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(74, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(75, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(76, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(77, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(78, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(79, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(80, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(81, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(82, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(83, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(84, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(85, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(86, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(87, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(88, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(89, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(90, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(91, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(92, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(94, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(95, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(96, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(97, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(98, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(99, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(100, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(101, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(102, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(103, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(104, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(105, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(106, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(107, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(108, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(109, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(110, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(111, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(112, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(113, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(114, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(115, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(116, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(117, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(118, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(119, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(120, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(121, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(122, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(123, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(126, state_ptrs[2]));
    state_ptrs[20]->add_t_function(make_pair(62, state_ptrs[2]));
    state_ptrs[3]->add_t_function(make_pair(60, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(9, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(32, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(33, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(34, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(35, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(36, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(37, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(38, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(40, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(41, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(42, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(43, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(44, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(45, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(46, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(47, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(48, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(49, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(50, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(51, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(52, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(53, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(54, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(55, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(56, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(57, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(58, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(59, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(61, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(63, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(64, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(65, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(66, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(67, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(68, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(69, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(70, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(71, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(72, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(73, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(74, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(75, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(76, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(77, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(78, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(79, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(80, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(81, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(82, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(83, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(84, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(85, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(86, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(87, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(88, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(89, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(90, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(91, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(92, state_ptrs[22]));
    state_ptrs[3]->add_t_function(make_pair(94, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(95, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(96, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(97, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(98, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(99, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(100, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(101, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(102, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(103, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(104, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(105, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(106, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(107, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(108, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(109, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(110, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(111, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(112, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(113, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(114, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(115, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(116, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(117, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(118, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(119, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(120, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(121, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(122, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(123, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(126, state_ptrs[21]));
    state_ptrs[3]->add_t_function(make_pair(62, state_ptrs[21]));
    state_ptrs[21]->add_t_function(make_pair(39, state_ptrs[25]));
    state_ptrs[22]->add_t_function(make_pair(60, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(32, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(33, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(34, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(35, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(36, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(37, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(38, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(39, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(40, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(41, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(42, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(43, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(44, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(45, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(46, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(47, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(48, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(49, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(50, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(51, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(52, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(53, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(54, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(55, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(56, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(57, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(58, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(59, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(61, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(63, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(64, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(65, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(66, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(67, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(68, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(69, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(70, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(71, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(72, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(73, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(74, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(75, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(76, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(77, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(78, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(79, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(80, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(81, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(82, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(83, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(84, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(85, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(86, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(87, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(88, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(89, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(90, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(91, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(92, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(94, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(95, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(96, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(97, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(98, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(99, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(100, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(101, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(102, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(103, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(104, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(105, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(106, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(107, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(108, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(109, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(110, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(111, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(112, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(113, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(114, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(115, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(116, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(117, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(118, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(119, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(120, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(121, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(122, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(123, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(126, state_ptrs[26]));
    state_ptrs[22]->add_t_function(make_pair(62, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(39, state_ptrs[25]));
    state_ptrs[26]->add_t_function(make_pair(48, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(49, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(50, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(51, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(52, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(53, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(54, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(55, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(56, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(57, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(97, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(98, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(99, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(100, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(101, state_ptrs[26]));
    state_ptrs[26]->add_t_function(make_pair(102, state_ptrs[26]));
    state_ptrs[8]->add_t_function(make_pair(46, state_ptrs[23]));
    state_ptrs[9]->add_t_function(make_pair(48, state_ptrs[9]));
    state_ptrs[9]->add_t_function(make_pair(49, state_ptrs[9]));
    state_ptrs[9]->add_t_function(make_pair(50, state_ptrs[9]));
    state_ptrs[9]->add_t_function(make_pair(51, state_ptrs[9]));
    state_ptrs[9]->add_t_function(make_pair(52, state_ptrs[9]));
    state_ptrs[9]->add_t_function(make_pair(53, state_ptrs[9]));
    state_ptrs[9]->add_t_function(make_pair(54, state_ptrs[9]));
    state_ptrs[9]->add_t_function(make_pair(55, state_ptrs[9]));
    state_ptrs[9]->add_t_function(make_pair(56, state_ptrs[9]));
    state_ptrs[9]->add_t_function(make_pair(57, state_ptrs[9]));
    state_ptrs[11]->add_t_function(make_pair(48, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(49, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(50, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(51, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(52, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(53, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(54, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(55, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(56, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(57, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(65, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(66, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(67, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(68, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(69, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(70, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(71, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(72, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(73, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(74, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(75, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(76, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(77, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(78, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(79, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(80, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(81, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(82, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(83, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(84, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(85, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(86, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(87, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(88, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(89, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(90, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(95, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(97, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(98, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(99, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(100, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(101, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(102, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(103, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(104, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(105, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(106, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(107, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(108, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(109, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(110, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(111, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(112, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(113, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(114, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(115, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(116, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(117, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(118, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(119, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(120, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(121, state_ptrs[11]));
    state_ptrs[11]->add_t_function(make_pair(122, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(48, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(49, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(50, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(51, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(52, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(53, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(54, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(55, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(56, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(57, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(65, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(66, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(67, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(68, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(69, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(70, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(71, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(72, state_ptrs[24]));
    state_ptrs[12]->add_t_function(make_pair(73, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(74, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(75, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(76, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(77, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(78, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(79, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(80, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(81, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(82, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(83, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(84, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(85, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(86, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(87, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(88, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(89, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(90, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(95, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(97, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(98, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(99, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(100, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(101, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(102, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(103, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(104, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(105, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(106, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(107, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(108, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(109, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(110, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(111, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(112, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(113, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(114, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(115, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(116, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(117, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(118, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(119, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(120, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(121, state_ptrs[11]));
    state_ptrs[12]->add_t_function(make_pair(122, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(48, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(49, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(50, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(51, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(52, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(53, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(54, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(55, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(56, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(57, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(65, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(66, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(67, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(68, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(69, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(70, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(71, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(72, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(73, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(74, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(75, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(76, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(77, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(78, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(79, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(80, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(81, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(82, state_ptrs[27]));
    state_ptrs[24]->add_t_function(make_pair(83, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(84, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(85, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(86, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(87, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(88, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(89, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(90, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(95, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(97, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(98, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(99, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(100, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(101, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(102, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(103, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(104, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(105, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(106, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(107, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(108, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(109, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(110, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(111, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(112, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(113, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(114, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(115, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(116, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(117, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(118, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(119, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(120, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(121, state_ptrs[11]));
    state_ptrs[24]->add_t_function(make_pair(122, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(48, state_ptrs[28]));
    state_ptrs[27]->add_t_function(make_pair(49, state_ptrs[28]));
    state_ptrs[27]->add_t_function(make_pair(50, state_ptrs[28]));
    state_ptrs[27]->add_t_function(make_pair(51, state_ptrs[28]));
    state_ptrs[27]->add_t_function(make_pair(52, state_ptrs[28]));
    state_ptrs[27]->add_t_function(make_pair(53, state_ptrs[28]));
    state_ptrs[27]->add_t_function(make_pair(54, state_ptrs[28]));
    state_ptrs[27]->add_t_function(make_pair(55, state_ptrs[28]));
    state_ptrs[27]->add_t_function(make_pair(56, state_ptrs[28]));
    state_ptrs[27]->add_t_function(make_pair(57, state_ptrs[28]));
    state_ptrs[27]->add_t_function(make_pair(65, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(66, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(67, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(68, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(69, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(70, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(71, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(72, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(73, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(74, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(75, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(76, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(77, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(78, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(79, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(80, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(81, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(82, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(83, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(84, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(85, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(86, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(87, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(88, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(89, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(90, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(95, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(97, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(98, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(99, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(100, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(101, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(102, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(103, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(104, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(105, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(106, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(107, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(108, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(109, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(110, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(111, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(112, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(113, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(114, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(115, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(116, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(117, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(118, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(119, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(120, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(121, state_ptrs[11]));
    state_ptrs[27]->add_t_function(make_pair(122, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(48, state_ptrs[28]));
    state_ptrs[28]->add_t_function(make_pair(49, state_ptrs[28]));
    state_ptrs[28]->add_t_function(make_pair(50, state_ptrs[28]));
    state_ptrs[28]->add_t_function(make_pair(51, state_ptrs[28]));
    state_ptrs[28]->add_t_function(make_pair(52, state_ptrs[28]));
    state_ptrs[28]->add_t_function(make_pair(53, state_ptrs[28]));
    state_ptrs[28]->add_t_function(make_pair(54, state_ptrs[28]));
    state_ptrs[28]->add_t_function(make_pair(55, state_ptrs[28]));
    state_ptrs[28]->add_t_function(make_pair(56, state_ptrs[28]));
    state_ptrs[28]->add_t_function(make_pair(57, state_ptrs[28]));
    state_ptrs[28]->add_t_function(make_pair(65, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(66, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(67, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(68, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(69, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(70, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(71, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(72, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(73, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(74, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(75, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(76, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(77, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(78, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(79, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(80, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(81, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(82, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(83, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(84, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(85, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(86, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(87, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(88, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(89, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(90, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(95, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(97, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(98, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(99, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(100, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(101, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(102, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(103, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(104, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(105, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(106, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(107, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(108, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(109, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(110, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(111, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(112, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(113, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(114, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(115, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(116, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(117, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(118, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(119, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(120, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(121, state_ptrs[11]));
    state_ptrs[28]->add_t_function(make_pair(122, state_ptrs[11]));

    finder = DFA(state_ptrs[0], nullptr);

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
    column++;
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
        if (*forward == '\n')
        {
            line++;
            column = 1;
        }
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
        return Token("<EOF>", string(1, '\0'));
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
        else if (finder.current().name() == -1)
        {
            error_list.push_back(Error(line, column, "Unexpected char: " + string(1, *forward)));
            finder.reset_movements();
            next_char();
            return Token("<ERROR>", "");
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

vector<Error> Scanner::errors()
{
    return error_list;
}
