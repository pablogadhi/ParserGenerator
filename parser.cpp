#include "parser.h"
#include "utils.h"
#include <iostream>

string ident_str(string str, int num)
{
    string identation = string(num, ' ');
    string result = "", str_copy = str;
    size_t pos = str.find("\n");

    do
    {
        auto sub_str = str_copy.substr(0, pos);
        result += identation + sub_str;
        str_copy.erase(0, pos + 1);
        if (str_copy.size() < str.size())
        {
            result += "\n";
        }
    } while ((pos = str_copy.find("\n")) != string::npos);

    return result;
}

int find_token_in_prod(Production prod, Token<string> token, int offset = 0)
{
    for (int i = offset; i < prod.body().size(); i++)
    {
        if (prod.body()[i].name() == token.name())
        {
            return i;
        }
    }
    return -1;
}

Production::Production(string name, string attr, vector<Token<string>> body, unordered_map<int, string> sem_actions,
                       unordered_map<int, string> ident_attr)
    : p_name(name), p_attributes(attr), p_body(body), p_sem_actions(sem_actions), p_ident_attr(ident_attr)
{
}

Production::Production()
{
}

Production::~Production()
{
}

string Production::name()
{
    return p_name;
}

string Production::attr()
{
    return p_attributes;
}

vector<Token<string>> &Production::body()
{
    return p_body;
}

unordered_map<int, string> Production::sem_actions()
{
    return p_sem_actions;
}

unordered_map<int, string> Production::ident_attr()
{
    return p_ident_attr;
}

Parser::Parser(Scanner &sc) : scanner(sc)
{
    scanner.get_finder().draw_machine("Scanner_DFA");
    scanner.scan();
}

Parser::~Parser()
{
}

void Parser::get()
{
    if (!error_fixes.empty())
    {
        current_token = error_fixes.back();
        error_fixes.pop_back();
        token_list.push_back(current_token);
    }
    else if (!watched_tokens.empty())
    {
        current_token = watched_tokens.back();
        watched_tokens.pop_back();
        token_list.push_back(current_token);
    }
    else
    {
        auto token_to_be = scanner.scan();
        if (token_to_be.name() != "<ERROR>")
        {
            current_token = token_to_be;
            token_list.push_back(current_token);
        }
        else
        {
            get();
        }
    }
}

Token<string> Parser::look_ahead()
{
    if (watched_tokens.empty())
    {
        return scanner.look_ahead();
    }
    else
    {
        return watched_tokens.back();
    }
}

Token<string> Parser::last_token()
{
    return *(token_list.end() - 2);
}

void Parser::expect(string str)
{
    // Refactor this code to be more efficient
    if (current_token.name() == str)
    {
        return;
    }

    while (true)
    {
        get();
        if (current_token.name() == str || current_token.name() == "<EOF>")
        {
            break;
        }
        else
        {
            syn_error(str);
        }
    }
}

bool Parser::soft_expect(string str)
{
    get();
    return current_token.name() == str;
}

string Parser::compiler_name()
{
    return new_compiler_name;
}

bool Parser::is_non_terminal(Token<string> token)
{
    return token.name() == "ident" && token_regex_map.find(token.value()) == token_regex_map.end();
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
    while (look_ahead().name() == "ident")
    {
        set_decl();
    }

    // Keywords
    expect("KEYWORDS");
    while (look_ahead().name() == "ident")
    {
        keyword_decl();
    }

    // Tokens
    expect("TOKENS");
    while (look_ahead().name() == "ident")
    {
        token_decl();
    }

    // Ignore
    if (soft_expect("IGNORE"))
    {
        ignore_decl();
    }

    // Productions
    expect("PRODUCTIONS");
    start_s_name = look_ahead().value();
    while (look_ahead().name() == "ident")
    {
        prod_decl();
    }
    fill_parsing_info();

    expect("END");
    get();
    expect("<EOF>");
}

void Parser::set_decl()
{
    get();
    string set_name = current_token.value();
    Set<char> char_set;
    function<void(char)> single_op_func = [&](char c) { char_set.add(c); };
    function<void(Set<char>)> set_op_func = [&](Set<char> set) { char_set = union_between_sets(char_set, set); };
    expect("=");
    while (look_ahead().name() != ".")
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
            if (current_token.value().size() >= 5 && current_token.value().substr(0, 3) == "CHR")
            {
                auto number = current_token.value().substr(4, current_token.value().size() - 5);
                single_op_func((char)stoi(number));
            }
            else
            {
                single_op_func(str_to_char(current_token.value()));
            }
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
                if (generator.operators().has_item(c) == -1)
                {
                    char_set.add(c);
                }
            }
        }
        else if (current_token.name() == "ANY")
        {
            auto ANY = Set<char>();
            for (auto &[key, set] : new_table.char_sets())
            {
                ANY = union_between_sets(ANY, set);
            }
            new_table.add_char_set("ANY", ANY);
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
                syn_error("ident", "Set " + current_token.value() + " was not defined!");
                break;
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

    if (new_compiler_name != "Coco")
    {
        auto keyword_regex = vector<Token<Set<char>>>();
        for (auto &c : real_str)
        {
            keyword_regex.push_back(Token<Set<char>>("char", Set<char>{c}));
        }
        token_regex_map[keyword_name] = make_pair(keyword_regex, false);
    }

    expect(".");
}

void Parser::ignore_decl()
{
    get();
    auto ignore_set = Set<char>();
    function<void(Set<char>)> current_op = [&](Set<char> set) { ignore_set = union_between_sets(ignore_set, set); };
    while (true)
    {
        if (current_token.name() == "ident")
        {
            if (new_table.char_sets().find(current_token.value()) != new_table.char_sets().end())
            {
                current_op(new_table.char_sets()[current_token.value()]);
            }
            else
            {
                syn_error("ident", "Set " + current_token.value() + " was not defined!");
            }
        }
        else if (current_token.name() == "+")
        {
            current_op = [&](Set<char> set) { ignore_set = union_between_sets(ignore_set, set); };
        }
        else if (current_token.name() == "-")
        {
            current_op = [&](Set<char> set) { ignore_set = diff_between_sets(ignore_set, set); };
        }
        else
        {
            break;
        }
        get();
    }
    new_table.char_sets()["<IGNORE>"] = ignore_set;
}

void Parser::token_decl()
{
    get();
    auto token_name = current_token.value();
    vector<Token<Set<char>>> regex;
    bool is_non_recursive = false;
    expect("=");
    while (look_ahead().name() != ".")
    {
        get();
        if (current_token.name() == "ident")
        {
            // If ident is in char map
            if (new_table.char_sets().find(current_token.value()) != new_table.char_sets().end())
            {
                regex.push_back(Token<Set<char>>(current_token.value(), new_table.char_sets()[current_token.value()]));
            }
            // if ident is a previously defined token
            // TODO Handle recursive declarations
            else if (token_regex_map.find(current_token.value()) != token_regex_map.end())
            {
                auto prev_regex = token_regex_map[current_token.value()];
                for (auto &set : prev_regex.first)
                {
                    regex.push_back(set);
                }
            }
            else
            {
                syn_error("ident", "Set or Token " + current_token.value() + " was not defined!");
                break;
            }
        }
        else if (current_token.name() == "char")
        {
            regex.push_back(Token<Set<char>>("char", Set<char>{str_to_char(current_token.value())}));
        }
        else if (current_token.name() == "string")
        {
            for (auto &c : current_token.value().substr(1, current_token.value().size() - 2))
            {
                regex.push_back(Token<Set<char>>("char", Set<char>{c}));
            }
        }
        else if (current_token.name() == "\x7" || current_token.name() == "\x8")
        {
            regex.push_back(Token<Set<char>>("\x5", Set<char>{'('}));
        }
        else if (current_token.name() == "\x3" || current_token.name() == "\x4")
        {
            regex.push_back(Token<Set<char>>("\x6", Set<char>{')'}));
            regex.push_back(Token<Set<char>>(current_token.name(), Set<char>{current_token.value()[0]}));
        }
        else if (current_token.name() == "ANY")
        {
            auto ANY = Set<char>();
            for (auto &[key, set] : new_table.char_sets())
            {
                ANY = union_between_sets(ANY, set);
            }
            new_table.add_char_set("ANY", ANY);
            regex.push_back(Token<Set<char>>("ANY", ANY));
        }
        else if (current_token.name() == "EXCEPT")
        {
            while (look_ahead().name() != ".")
            {
                get();
            }
            break;
        }
        else if (current_token.name() == "NONRECURSIVE")
        {
            is_non_recursive = true;
        }
        else
        {
            regex.push_back(Token<Set<char>>(current_token.name(), Set<char>{current_token.value()[0]}));
        }
    }
    token_regex_map[token_name] = make_pair(regex, is_non_recursive);
    get();
}

void Parser::prod_decl()
{
    get();

    auto prod_name = current_token.value();
    string args = "";
    vector<Token<string>> prod_body;
    unordered_map<int, string> sem_actions, ident_attr;
    int token_idx = 0;

    if (soft_expect("attr"))
    {
        args = current_token.value().substr(1, current_token.value().size() - 2);
    }

    expect("=");

    while (look_ahead().name() != ".")
    {
        get();
        if (current_token.name() == "char")
        {
            auto t_val_str = current_token.value().substr(1, current_token.value().size() - 2);
            token_regex_map[t_val_str] = make_pair(
                vector<Token<Set<char>>>{Token(t_val_str, Set<char>{str_to_char(current_token.value())})}, false);
            prod_body.push_back(Token<string>(current_token.name(), t_val_str));
            token_idx++;
        }
        else if (current_token.name() == "string")
        {
            auto t_val_str = current_token.value().substr(1, current_token.value().size() - 2);

            // See if the value could be a keyword
            auto finder = scanner.get_finder();
            for (auto &c : t_val_str)
            {
                finder.move(c);
            }

            if (finder.current().reference_name() == "ident")
            {
                new_table.add_keyword(t_val_str, t_val_str);
            }
            else
            {
                vector<Token<Set<char>>> regex;
                for (auto &c : t_val_str)
                {
                    regex.push_back(Token<Set<char>>(t_val_str, Set<char>{c}));
                }
                token_regex_map[t_val_str] = make_pair(regex, false);
            }
            finder.reset_movements();

            prod_body.push_back(Token<string>(current_token.name(), t_val_str));
            token_idx++;
        }
        else if (current_token.name() == "attr")
        {
            auto attr_val = current_token.value().substr(1, current_token.value().size() - 2);
            ident_attr[token_idx] = attr_val;
        }
        else if (current_token.name() == "semAction")
        {
            auto sem_act_val = current_token.value().substr(2, current_token.value().size() - 4);
            sem_actions[token_idx] = sem_act_val;
        }
        else
        {
            prod_body.push_back(current_token);
            token_idx++;
        }
    }

    production_indices[prod_name] = productions.size();
    productions.push_back(Production(prod_name, args, prod_body, sem_actions, ident_attr));
    if (first_pos.find(prod_name) == first_pos.end())
    {
        first_pos[prod_name] = Set<string>{};
        follow_pos[prod_name] = Set<string>{};
    }

    get();
}

void Parser::compute_first_pos(Token<string> symbol, Production last_prod, int symbol_idx, Set<string> &result,
                               vector<string> prev_computed)
{
    int first_idx = 0;
    if (is_non_terminal(symbol))
    {
        if (find(prev_computed.begin(), prev_computed.end(), symbol.value()) == prev_computed.end())
        {
            auto prod = productions[production_indices[symbol.value()]];
            prev_computed.push_back(symbol.value());
            compute_first_pos(prod.body()[0], prod, first_idx, result, prev_computed);
        }
    }
    else if (symbol.name() == "\x7" || symbol.name() == "\x5")
    {
        if (symbol.name() == "\x7")
        {
            int closing_t_idx = find_token_in_prod(last_prod, Token<string>("\x3", "}"));
            if (closing_t_idx == (last_prod.body().size() - 1))
            {
                result = union_between_sets(result, Set<string>{""});
            }
            else
            {
                compute_first_pos(last_prod.body()[closing_t_idx + 1], last_prod, closing_t_idx + 1, result,
                                  prev_computed);
            }
        }

        first_idx = symbol_idx + 1;
        while (true)
        {
            compute_first_pos(last_prod.body()[first_idx], last_prod, first_idx, result, prev_computed);
            // TODO Find closing tags first
            if ((first_idx = find_token_in_prod(last_prod, Token<string>("\x2", "|"), first_idx)) == -1)
            {
                break;
            }
            else
            {
                first_idx++;
            }
        }
    }
    else if (symbol.name() == "\x8")
    {
        int closing_t_idx = find_token_in_prod(last_prod, Token<string>("\x4", "]"));
        if (closing_t_idx == (last_prod.body().size() - 1))
        {
            result = union_between_sets(result, Set<string>{""});
        }
        else
        {
            compute_first_pos(last_prod.body()[closing_t_idx + 1], last_prod, closing_t_idx + 1, result, prev_computed);
        }
        compute_first_pos(last_prod.body()[symbol_idx + 1], last_prod, symbol_idx + 1, result, prev_computed);
    }
    else
    {
        result = union_between_sets(result, Set<string>{symbol.value()});
    }
}

void Parser::compute_follow_pos(string non_ter_name)
{
    if (non_ter_name == start_s_name)
    {
        follow_pos[non_ter_name] = union_between_sets(follow_pos[non_ter_name], Set<string>{"$"});
    }

    auto prod = productions[production_indices[non_ter_name]];
    for (int i = 0; i < prod.body().size(); i++)
    {
        auto token = prod.body()[i];
        if (is_non_terminal(token))
        {
            auto next_idx = i + 1;
            if (next_idx < prod.body().size() &&
                Set<string>{"\x3", "\x4", "\x6", "\x2"}.has_item(prod.body()[next_idx].name()) != -1)
            {
                for (int j = next_idx; j < prod.body().size(); j++)
                {
                    if (Set<string>{"\x3", "\x4", "\x6"}.has_item(prod.body()[j].name()) != -1)
                    {
                        next_idx = j + 1;
                        break;
                    }
                    next_idx = prod.body().size();
                }
            }

            if (next_idx < prod.body().size())
            {
                auto follow_set = Set<string>{};
                compute_first_pos(prod.body()[next_idx], prod, next_idx, follow_set, vector<string>{});

                if (follow_set.has_item("") != -1)
                {
                    follow_set = diff_between_sets(follow_set, Set<string>{""});
                    follow_set = union_between_sets(follow_set, follow_pos[non_ter_name]);
                }
                follow_pos[token.value()] = union_between_sets(follow_pos[token.value()], follow_set);
            }
            else
            {
                follow_pos[token.value()] = union_between_sets(follow_pos[token.value()], follow_pos[non_ter_name]);
            }
        }
    }
}

void Parser::fill_parsing_info()
{
    for (auto &prod : productions)
    {
        auto first_pos_set = Set<string>{};
        compute_first_pos(prod.body()[0], prod, 0, first_pos_set, vector<string>{prod.name()});
        first_pos[prod.name()] = first_pos_set;
    }

    for (auto &prod : productions)
    {
        compute_follow_pos(prod.name());
    }
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
        string command_str = "    s_table.add_char_set(\"" + key + "\", Set<char>{";
        for (auto &c : set)
        {
            set_str = set_str + char_to_str(c, true) + ", ";
        }
        set_str.pop_back();
        set_str.pop_back();
        cpp_file << command_str << set_str << "});" << endl;
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
    auto char_sets = new_table.char_sets();
    auto future_dfa = generator.generate_dfa_finder(token_regex_map, new_compiler_name == "Coco", false);
    future_dfa.draw_machine("Generated_DFA");
    auto all_states = future_dfa.flatten();
    for (auto &state : all_states)
    {
        auto state_name = to_string(state->name());
        auto is_accepting = to_string(state->is_accepting());
        auto ref_name = state->reference_name();
        auto non_r_path = to_string(state->start_non_recursive_path());
        cpp_file << "    auto state_" + state_name + " = State(" + state_name + ", " + is_accepting + ", " +
                        non_r_path + ", \"" + ref_name + "\");"
                 << endl;
        cpp_file << "    state_ptrs[" + state_name + "] = make_shared<State>(state_" + state_name + ");" << endl;
    }

    for (auto &state : all_states)
    {
        auto state_name = to_string(state->name());
        for (auto &tran : state->get_t_functions())
        {
            auto t_state_name = to_string(tran.second->name());
            cpp_file << "    state_ptrs[" + state_name + "]->add_t_function(make_pair(" + to_string(tran.first) + ", " +
                            "state_ptrs[" + t_state_name + "]));"
                     << endl;
        }
    }

    // Copy the rest of the file
    getline(frame_file, line);
    while (line != "-->end")
    {
        cpp_file << line << endl;
        getline(frame_file, line);
    }

    frame_file.close();
    header.close();
    cpp_file.close();
}

string Parser::write_ident_call(Token<string> token, int t_idx, Production &prod, int ident_level, ofstream &cpp_file,
                                bool write)
{
    auto ident_attr = prod.ident_attr();

    string ident_args = "";
    if (ident_attr.find(t_idx + 1) != ident_attr.end())
    {
        ident_args = ident_attr[t_idx + 1];
    }
    string final_str = ident_str(token.value(), ident_level) + "(" + ident_args + ");" + "\n";
    if (write)
    {
        cpp_file << final_str;
    }
    return final_str;
}

string Parser::write_expect_call(Token<string> token, int ident_level, ofstream &cpp_file, bool write)
{
    string final_str = ident_str("expect(\"", ident_level) + token.value() + "\");\n";
    if (write)
    {
        cpp_file << final_str;
    }
    return final_str;
}

string Parser::write_sem_action(int idx, Production &prod, int ident_level, ofstream &cpp_file, bool write)
{
    auto sem_actions = prod.sem_actions();
    if (sem_actions.find(idx) != sem_actions.end())
    {
        auto sem_act = sem_actions[idx];
        if (sem_act.find("\n") != string::npos && sem_act[sem_act.size() - 1] != '\n')
        {
            sem_act.push_back('\n');
        }
        string final_str = ident_str(sem_act, ident_level) + "\n";
        // final_str.erase(remove(final_str.begin(), final_str.end(), '\r'), final_str.end());
        if (write)
        {
            cpp_file << final_str;
        }
        return final_str;
    }
    return "";
}

string Parser::write_if_call(Token<string> token, Production &prod, vector<Token<string>>::iterator &body_ptr,
                             int ident_level, ofstream &cpp_file, bool write)
{
    body_ptr++;
    Token<string> inner_t = *body_ptr;
    int inner_t_idx = body_ptr - prod.body().begin();
    Set<string> first_pos_set;
    compute_first_pos(inner_t, prod, inner_t_idx, first_pos_set, vector<string>{});

    string cond_str = "";
    bool has_epsilon = false;
    for (auto &fi_item : first_pos_set)
    {
        if (fi_item != "")
        {
            cond_str += "look_ahead().name() == \"" + fi_item + "\" || ";
        }
        else
        {
            has_epsilon = true;
        }
    }
    cond_str = cond_str.substr(0, cond_str.size() - 4);

    string final_string = ident_str("if(", ident_level) + cond_str + ")\n";
    final_string += ident_str("{", ident_level) + "\n";
    final_string += ident_str("get();", ident_level + 4) + "\n";

    while (inner_t.name() != "\x4")
    {
        if (is_non_terminal(inner_t))
        {
            final_string += write_ident_call(inner_t, inner_t_idx, prod, ident_level + 4, cpp_file, false);
        }
        else
        {
            if (prod.body()[inner_t_idx - 1].name() != "\x8")
            {
                final_string += write_expect_call(inner_t, ident_level + 4, cpp_file, false);
            }
        }

        final_string += write_sem_action(inner_t_idx + 1, prod, ident_level + 4, cpp_file, false);

        body_ptr++;
        inner_t = *body_ptr;
        inner_t_idx = body_ptr - prod.body().begin();
    }

    final_string += ident_str("}", ident_level) + "\n";

    if (write)
    {
        cpp_file << final_string;
    }

    return final_string;
}

string Parser::write_kleene_par_call(Token<string> token, Production &prod, vector<Token<string>>::iterator &body_ptr,
                                     int ident_level, ofstream &cpp_file, bool write)
{

    string whl_condition = "";
    vector<string> inner_conditions;
    vector<string> inner_bodies;
    body_ptr++;
    while ((*body_ptr).name() != "\x3" && (*body_ptr).name() != "\x6")
    {
        Token<string> inner_t = *body_ptr;
        int inner_t_idx = body_ptr - prod.body().begin();
        Set<string> first_pos_set;
        compute_first_pos(inner_t, prod, inner_t_idx, first_pos_set, vector<string>{});

        string cond_str = "";
        bool has_epsilon = false;
        for (auto &fi_item : first_pos_set)
        {
            if (fi_item != "")
            {
                cond_str += "look_ahead().name() == \"" + fi_item + "\" || ";
                if (token.name() == "\x7")
                {
                    whl_condition += "look_ahead().name() == \"" + fi_item + "\" || ";
                }
            }
            else
            {
                has_epsilon = true;
            }
        }

        if (has_epsilon && is_non_terminal(inner_t))
        {
            Set<string> follow_pos_set;
            compute_follow_pos(inner_t.value());
            for (auto &fo_item : follow_pos_set)
            {
                cond_str += "current_token.name() == \"" + fo_item + "\" || ";
                if (token.name() == "\x7")
                {
                    whl_condition += "current_token.name() == \"" + fo_item + "\" || ";
                }
            }
        }

        cond_str = cond_str.substr(0, cond_str.size() - 4);
        inner_conditions.push_back(cond_str);

        string inner_body_str = "";
        while (inner_t.name() != "\x2" && inner_t.name() != "\x3" && inner_t.name() != "\x6")
        {
            if (is_non_terminal(inner_t))
            {
                inner_body_str += write_ident_call(inner_t, inner_t_idx, prod, ident_level + 4, cpp_file, false);
            }
            else if (inner_t.name() == "\x7" || inner_t.name() == "\x5")
            {
                int new_ident_level = ident_level;
                if (find_token_in_prod(prod, Token<string>("\x2", "|"), inner_t_idx) != -1)
                {
                    new_ident_level += 4;
                }
                inner_body_str += write_kleene_par_call(inner_t, prod, body_ptr, new_ident_level, cpp_file, false);
            }
            else if (inner_t.name() == "\x8")
            {
                inner_body_str += write_if_call(inner_t, prod, body_ptr, ident_level + 4, cpp_file, false);
            }
            else
            {
                inner_body_str += write_expect_call(inner_t, ident_level + 4, cpp_file, false);
            }

            inner_body_str += write_sem_action(inner_t_idx + 1, prod, ident_level + 4, cpp_file, false);

            body_ptr++;
            inner_t = *body_ptr;
            inner_t_idx = body_ptr - prod.body().begin();
        }
        inner_bodies.push_back(inner_body_str);

        if (inner_t.name() == "\x2")
        {
            body_ptr++;
        }
    }

    string final_str = "";
    if (whl_condition.size() > 0)
    {
        final_str = ident_str("while(", ident_level) + whl_condition.substr(0, whl_condition.size() - 4) + ")\n";
        final_str += ident_str("{", ident_level) + "\n";
        ident_level += 4;
    }

    if (inner_conditions.size() > 1)
    {
        for (int i = 0; i < inner_conditions.size(); i++)
        {
            string else_str = "";
            if (i > 0)
            {
                else_str = "else ";
            }
            final_str += ident_str(else_str, ident_level) + "if(" + inner_conditions[i] + ")\n";
            final_str += ident_str("{", ident_level) + "\n";
            // final_str += ident_str("get();", ident_level + 4) + "\n";
            final_str += ident_str(inner_bodies[i], ident_level - 4);
            final_str += ident_str("}", ident_level) + "\n";
        }
    }
    else
    {
        final_str += inner_bodies[0];
    }

    if (whl_condition.size() > 0)
    {
        final_str += ident_str("}", ident_level - 4) + "\n";
    }

    if (write)
    {
        cpp_file << final_str;
    }

    return final_str;
}

void Parser::write_parser()
{

    ifstream frame_file("parser.frame");
    ofstream header(new_compiler_name + "/parser.h");
    ofstream cpp_file(new_compiler_name + "/parser.cpp");
    streampos f_header_pos;

    string line;

    // Ignore comments
    while (line != "-->begin")
    {
        getline(frame_file, line);
    }

    // Copy the first part of the header
    getline(frame_file, line);
    while (line != "-->prod_declarations")
    {
        header << line << endl;
        getline(frame_file, line);
    }
    f_header_pos = frame_file.tellg();

    // Ignore the rest until the begining of the declaration file is reached
    while (line != "-->begin")
    {
        getline(frame_file, line);
    }

    // Copy the start of the cpp declaration
    getline(frame_file, line);
    while (line != "-->prod_implementations")
    {
        cpp_file << line << endl;
        getline(frame_file, line);
    }

    // Generate production code
    for (auto &prod : productions)
    {

        // Declaration
        header << ident_str("void ", 4) + prod.name() + "(" + prod.attr() + ");" << endl;

        // Implementation
        cpp_file << "void Parser::" + prod.name() + "(" + prod.attr() + ")" << endl << "{" << endl;

        vector<Token<string>>::iterator body_ptr = prod.body().begin();
        while (body_ptr != prod.body().end())
        {
            Token<string> token = *body_ptr;
            auto idx = body_ptr - prod.body().begin();

            write_sem_action(idx, prod, 4, cpp_file);

            if (is_non_terminal(token))
            {
                write_ident_call(token, idx, prod, 4, cpp_file);
            }
            else if (token.name() == "\x7" || token.name() == "\x5")
            {
                write_kleene_par_call(token, prod, body_ptr, 4, cpp_file);
            }
            else if (token.name() == "\x8")
            {
                write_if_call(token, prod, body_ptr, 4, cpp_file);
            }
            else
            {
                write_expect_call(token, 4, cpp_file);
            }
            body_ptr++;
        }

        write_sem_action(prod.body().size(), prod, 4, cpp_file);

        cpp_file << "}" << endl << endl;
    }

    // Finish copying implementation
    getline(frame_file, line);
    while (line != "-->first_prod")
    {
        cpp_file << line << endl;
        getline(frame_file, line);
    }

    cpp_file << ident_str(productions[0].name(), 4) + "(" + productions[0].attr() + ");" << endl;

    getline(frame_file, line);
    while (line != "-->end")
    {
        cpp_file << line << endl;
        getline(frame_file, line);
    }

    // Finish copying declaration
    frame_file.seekg(f_header_pos);
    getline(frame_file, line);
    while (line != "-->implementation")
    {
        header << line << endl;
        getline(frame_file, line);
    }

    frame_file.close();
    cpp_file.close();
    header.close();
}

void Parser::syn_error(string token_name, string message)
{
    auto prev_token = last_token();
    string error_str = "\nSyntatic error found after (" + prev_token.name() + ", " + prev_token.value() + ")\n";
    if (message == "")
    {
        error_str += "Expected token: " + token_name + "\n" + "Found token with name: " + current_token.name() +
                     ", and value: " + current_token.value() + "\n\n";
    }
    else
    {
        error_str += message + "\n\n";
    }
    cout << error_str;
    auto error = Error(error_str, SYNTACTIC_ERROR);
    syntactic_errors.push_back(error);
    fix_syn_error(token_name);
}

void Parser::fix_syn_error(string token_name)
{
    if (look_ahead().name() != token_name)
    {
        unordered_map<string, string>::iterator keyword_itr;
        if (token_name.size() == 1)
        {
            error_fixes.push_back(Token<string>(token_name, token_name));
            watched_tokens.push_back(current_token);
            token_list.pop_back();
        }
        else if ((keyword_itr = scanner.symbols().keywords().find(token_name)) != scanner.symbols().keywords().end())
        {
            error_fixes.push_back(Token<string>((*keyword_itr).second, (*keyword_itr).second));
            watched_tokens.push_back(current_token);
            token_list.pop_back();
        }
    }
}