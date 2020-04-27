#include "parser.h"
#include "dfa_builder.h"
#include "utils.h"
#include <iostream>
#include <stack>

int get_precedence(char c)
{
    if (c == '|')
    {
        return 1;
    }
    if (c == '\0')
    {
        return 2;
    }
    else if (c == '*' || c == '+' || c == '?')
    {
        return 3;
    }
    return 0;
}

vector<Set<char>> infix_to_postfix(vector<Set<char>> infix_regex, unordered_map<string, Set<char>> language_map,
                                   function<int(char)> get_precedence)
{
    stack<Set<char>> char_stack;
    vector<Set<char>> output;

    auto operators = language_map["operator"];
    auto special_symbols = language_map["special"];

    for (auto &t : infix_regex)
    {
        if (intersec_between_sets(t, operators).size() == 0 && intersec_between_sets(t, special_symbols).size() == 0)
        {
            output.push_back(t);
        }
        else if (t.has_item('(') != -1)
        {
            char_stack.push(t);
        }
        else if (t.has_item(')') != -1)
        {
            while (char_stack.top().has_item('('))
            {
                output.push_back(char_stack.top());
                char_stack.pop();
            }
            char_stack.pop();
        }
        else if (intersec_between_sets(t, operators).size() != 0)
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

TreeNode<int> make_syntax_tree(Set<char> symbol_as_set, int &name_index, vector<TreeNode<int>> children)
{
    // TODO Handle symbols with more than one character
    int symbol = (int)(symbol_as_set.get_items()[0]);
    if (children.size() == 1)
    {
        return TreeNode<int>(-1, symbol, make_shared<TreeNode<int>>(children[0]));
    }
    return TreeNode<int>(-1, symbol, make_shared<TreeNode<int>>(children[0]), make_shared<TreeNode<int>>(children[1]));
}

TreeNode<int> tree_from_set(Set<char> set, int &name_index)
{
    stack<TreeNode<int>> char_stack;
    for (auto &c : set)
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
                TreeNode<int>(-1, 124, make_shared<TreeNode<int>>(left), make_shared<TreeNode<int>>(right)));
        }
    }
    return char_stack.top();
}

TreeNode<int> basic_syntax_node_generator(Set<char> symbol, int &name)
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
        node = TreeNode<int>(name, (int)(symbol.get_items()[0]));
        name++;
    }
    return node;
}

bool is_operator_binary(Set<char> op)
{
    if (op.get_items()[0] == '|' || op.get_items()[0] == '\0')
    {
        return true;
    }
    return false;
}

TreeNode<int> postfix_eval(vector<Set<char>> postfix_expr, Set<char> operators)
{
    stack<TreeNode<int>> m_stack;
    int state_index = 0;
    for (auto &t : postfix_expr)
    {
        TreeNode<int> a, b;
        if (intersec_between_sets(t, operators).size() != 0)
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

vector<Set<char>> format_regex(vector<Set<char>> regex, Set<char> operators)
{
    vector<Set<char>> result;
    for (int i = 0; i < regex.size() - 1; i++)
    {
        Set<char> first = regex[i], second = regex[i + 1];
        result.push_back(first);
        if (first.has_item('(') == -1 && second.has_item(')') == -1 &&
            intersec_between_sets(operators, second).size() == 0 && !is_operator_binary(first))
        {
            result.push_back(Set<char>(vector<char>{'\0'}));
        }
    }
    result.push_back(regex[regex.size() - 1]);
    return result;
}

TreeNode<int> expand_tree(shared_ptr<TreeNode<Set<char>>> root, int &name_index)
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
        new_tree = TreeNode<int>(name_index, (int)(root->symbol()[0]));
        name_index++;
        new_tree.set_left(left_child);
        new_tree.set_right(right_child);
    }
    return new_tree;
}

DFA generate_dfa_finder(vector<pair<string, vector<Set<char>>>> all_regex, unordered_map<string, Set<char>> &char_map)
{
    vector<DFA> regex_dfa;
    auto builder = DFABuilder(char_map);
    auto hashtag = Set<char>(vector<char>{'#'});

    int state_name = 0;
    int dfa_idx = 0;
    for (auto &regex : all_regex)
    {
        regex.second.push_back(hashtag);

        auto formated_regex = format_regex(regex.second, char_map["operator"]);
        auto postfix_regex = infix_to_postfix(formated_regex, char_map, get_precedence);
        auto syntax_tree = postfix_eval(postfix_regex, char_map["operator"]);
        auto dfa = builder.dfa_from_syntax_tree(syntax_tree, state_name, regex.first);
        string d_name = "DFA_" + to_string(dfa_idx);
        // dfa.draw_machine(d_name);
        dfa_idx++;
        regex_dfa.push_back(dfa);
        // The name doesn't get updated on the last creation of a state,
        // that's why it's updated here. All the state names in every dfa must
        // be unique in order to avoid conflicts when creating a nfa from those DFAs.
        state_name++;
    }

    shared_ptr<State> first_state = make_shared<State>(state_name);
    state_name++;
    for (auto &dfa : regex_dfa)
    {
        first_state->add_t_function(make_pair(1, dfa.start()));
    }
    auto nfa = NFA(first_state, nullptr);
    state_name = 0;
    auto final_dfa = builder.dfa_from_nfa(nfa, state_name);

    // Apend a dfa for each opearator and special character, except for the 0 character
    for (auto &op : diff_between_sets(union_between_sets(char_map["operator"], char_map["special"]),
                                      Set<char>(vector<char>{'\0'})))
    {
        state_name++;
        auto state = State(state_name);
        state.set_as_accepting(true);
        state.set_reference_name(string(1, op));
        // TODO Handle multiple character operators
        final_dfa.start()->add_t_function(make_pair((int)op, make_shared<State>(state)));
    }

    return final_dfa;
}

Parser::Parser(Scanner &sc) : scanner(sc)
{
    auto char_map = scanner.symbols().char_sets();

    // Generate token regexes
    auto kleene = Set<char>(vector<char>{'*'});
    auto left_p = Set<char>(vector<char>{'('});
    auto right_p = Set<char>(vector<char>{')'});
    auto or_op = Set<char>(vector<char>{'|'});

    // TODO make ident accept digits
    auto ident =
        vector<Set<char>>{char_map["letter"], left_p, char_map["letter"], or_op, char_map["digit"], right_p, kleene};
    auto string_regex =
        vector<Set<char>>{char_map["\""], left_p, char_map["stringCh"], or_op, char_map["\\"], char_map["printable"],
                          right_p,        kleene, char_map["\""]};
    auto char_regex = vector<Set<char>>{
        char_map["\'"], left_p,  char_map["charCh"], or_op, char_map["\\"], char_map["printable"], char_map["hex"],
        kleene,         right_p, char_map["\'"]};
    auto dot_regex = vector<Set<char>>{char_map["."]};
    auto equal = vector<Set<char>>{char_map["="]};
    auto minus = vector<Set<char>>{char_map["-"]};
    auto double_dot = vector<Set<char>>{char_map["."], char_map["."]};

    auto all_regex = vector<pair<string, vector<Set<char>>>>{
        make_pair("ident", ident),   make_pair("string", string_regex), make_pair("char", char_regex),
        make_pair("..", double_dot), make_pair(".", dot_regex),         make_pair("-", minus),
        make_pair("=", equal)};

    auto finder = generate_dfa_finder(all_regex, char_map);
    // finder.print_machine();
    finder.draw_machine("Scanner_DFA");
    scanner.set_finder(finder);
    scanner.scan();
}

Parser::~Parser()
{
}

void Parser::get()
{
    current_token = scanner.scan();
    token_list.push_back(current_token);
}

void Parser::expect(string str)
{
    while (true)
    {
        get();
        if (current_token.name() == str || current_token.name() == "EOF")
        {
            break;
        }
        else
        {
            // TODO Add Syntatic Error to error list
        }
    }
}

void Parser::parse()
{
    // Compiler Name
    expect("COMPILER");
    expect("ident");
    new_compiler_name = current_token.value();

    // TODO Handle Semantic Action

    // Character Sets
    expect("CHARACTERS");
    while (scanner.look_ahead().name() == "ident")
    {
        set_decl();
    }

    // Keywords
    expect("KEYWORDS");
    while (scanner.look_ahead().name() == "ident")
    {
        keyword_decl();
    }

    // Tokens
    expect("TOKENS");
    while (scanner.look_ahead().name() == "ident")
    {
        token_decl();
    }

    // Productions

    // END
    expect("END");
    expect("EOF");

    int i = 0;

    // while (la_token.name() != "EOF")
    // {
    //     c_token = la_token;
    //     cout << "(" << c_token.name() << ", " << c_token.value() << ")" << endl;
    //     la_token = scanner.scan();
    // }
    // cout << "<" << la_token.name() << ">" << endl;
}

void Parser::set_decl()
{
    get();
    string set_name = current_token.value();
    Set<char> char_set;
    function<void(char)> single_op_func = [&](char c) { char_set.add(c); };
    function<void(Set<char>)> set_op_func = [&](Set<char> set) { char_set = union_between_sets(char_set, set); };
    expect("=");
    while (scanner.look_ahead().name() != ".")
    {
        get();
        if (current_token.name() == "string")
        {
            for (auto &c : current_token.value().substr(1, current_token.value().size() - 2))
            {
                single_op_func(c);
            }
        }
        else if (current_token.name() == "char")
        {
            single_op_func(str_to_char(current_token.value()));
        }
        else if (current_token.name() == "+")
        {
            single_op_func = [&](char c) { char_set.add(c); };
            set_op_func = [&](Set<char> set) { char_set = union_between_sets(char_set, set); };
        }
        else if (current_token.name() == "-")
        {
            single_op_func = [&](char c) { char_set.remove(c); };
            set_op_func = [&](Set<char> set) { char_set = diff_between_sets(char_set, set); };
        }
        else if (current_token.name() == "..")
        {
            get();
            auto limit_char = str_to_char(current_token.value());
            for (char c = char_set[0] + 1; c <= limit_char; c++)
            {
                char_set.add(c);
            }
        }
        else if (current_token.name() == "ANY")
        {
            auto ANY = Set<char>();
            for (auto &[key, set] : new_table.char_sets())
            {
                ANY = union_between_sets(ANY, set);
            }
            char_set = union_between_sets(ANY, char_set);
        }
        else // The current token is an ident
        {
            auto prev_sets = new_table.char_sets();
            if (prev_sets.find(current_token.value()) != prev_sets.end())
            {
                auto other_set = prev_sets[current_token.value()];
                set_op_func(other_set);
            }
            else
            {
                // TODO Add Error: Set Not Found
            }
        }
    }
    new_table.add_char_set(set_name, char_set);
    get();
}

void Parser::keyword_decl()
{
    get();
    auto keyword_name = current_token.value();
    expect("=");
    expect("string");
    auto real_str = current_token.value().substr(1, current_token.value().size() - 2);
    new_table.add_keyword(keyword_name, real_str);
    expect(".");
}

void Parser::token_decl()
{
    get();
    auto token_name = current_token.value();
    vector<Set<char>> regex;
    expect("=");
    while (scanner.look_ahead().name() != ".")
    {
        get();
        if (current_token.name() == "ident")
        {
            regex.push_back(new_table.char_sets()[current_token.value()]);
        }
        else if (current_token.name() == "char")
        {
            regex.push_back(Set<char>(vector<char>{str_to_char(current_token.value())}));
        }
        else
        {
            regex.push_back(Set<char>(vector<char>{current_token.value()[0]}));
        }
    }
    token_regex_list.push_back(make_pair(token_name, regex));
    get();
}

string Parser::compiler_name()
{
    return new_compiler_name;
}

void Parser::write_scanner()
{
    ifstream frame_file("scanner.frame");
    ofstream header(new_compiler_name + "/scanner.h");
    ofstream cpp_file(new_compiler_name + "/scanner.cpp");
    string line;

    // Ignore comments
    while (line != "-->begin")
    {
        getline(frame_file, line);
    }

    // Copy header
    getline(frame_file, line);
    while (line != "-->implementation")
    {
        header << line << endl;
        getline(frame_file, line);
    }

    // Ignore comments
    while (line != "-->begin")
    {
        getline(frame_file, line);
    }

    // Copy the start of the cpp declaration
    getline(frame_file, line);
    while (line != "-->char_sets_decl")
    {
        cpp_file << line << endl;
        getline(frame_file, line);
    }

    // Generate the char set map
    for (auto &[key, set] : new_table.char_sets())
    {
        string set_str;
        string command_str = "    s_table.add_char_set(\"" + key + "\", Set<char>(vector<char>{";
        for (auto &c : set)
        {
            set_str = set_str + char_to_str(c) + ", ";
        }
        set_str.pop_back();
        set_str.pop_back();
        cpp_file << command_str << set_str << "}));" << endl;
    }

    // Copy comments
    getline(frame_file, line);
    while (line != "-->keywords_decl")
    {
        cpp_file << line << endl;
        getline(frame_file, line);
    }

    // Generate the keyword map
    for (auto &[key, val] : new_table.keywords())
    {
        cpp_file << "    s_table.add_keyword(\"" + key + "\", \"" + val + "\");" << endl;
    }

    // Copy comments
    getline(frame_file, line);
    while (line != "-->dfa_decl")
    {
        cpp_file << line << endl;
        getline(frame_file, line);
    }

    // Generate DFA declaration

    frame_file.close();
    header.close();
}