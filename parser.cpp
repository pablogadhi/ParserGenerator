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

vector<Token<string>> Production::body()
{
    return p_body;
}

Parser::Parser(Scanner &sc) : scanner(sc)
{
    scanner.get_finder().draw_machine("Scanner_DFA");
    scanner.scan();
}

Parser::~Parser()
{
}

string Parser::compiler_name()
{
    return new_compiler_name;
}

void Parser::get()
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
            // TODO Add Syntatic Error to error list
        }
    }
}

bool Parser::soft_expect(string str)
{
    get();
    return current_token.name() == str;
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

    // Ignore
    if (soft_expect("IGNORE"))
    {
        ignore_decl();
    }

    // Productions
    expect("PRODUCTIONS");
    start_s_name = scanner.look_ahead().value();
    while (scanner.look_ahead().name() == "ident")
    {
        prod_decl();
    }
    fill_parsing_table();

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
            ANY = diff_between_sets(ANY, generator.operators());
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
                syn_error();
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

    if (!soft_expect("."))
    {
        syn_error();
    }
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
            // TODO Handle unknown sets
            current_op(new_table.char_sets()[current_token.value()]);
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
    while (scanner.look_ahead().name() != ".")
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
                syn_error();
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
            ANY = diff_between_sets(ANY, generator.operators());
            regex.push_back(Token<Set<char>>("ANY", ANY));
        }
        else if (current_token.name() == "EXCEPT")
        {
            while (scanner.look_ahead().name() != ".")
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

    while (scanner.look_ahead().name() != ".")
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
            vector<Token<Set<char>>> regex;
            auto t_val_str = current_token.value().substr(1, current_token.value().size() - 2);
            for (auto &c : t_val_str)
            {
                regex.push_back(Token<Set<char>>(t_val_str, Set<char>{c}));
            }
            token_regex_map[t_val_str] = make_pair(regex, false);
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
    if (parsing_table.find(prod_name) == parsing_table.end())
    {
        parsing_table[prod_name] = unordered_map<string, int>{};
        first_pos[prod_name] = Set<string>{};
        follow_pos[prod_name] = Set<string>{};
    }

    get();
}

void Parser::compute_first_pos(Token<string> symbol, Production last_prod, int symbol_idx, Set<string> &result,
                               vector<string> prev_computed)
{
    int first_idx = 0;
    if (symbol.name() == "ident" && token_regex_map.find(symbol.value()) == token_regex_map.end())
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
        if (token.name() == "ident" && token_regex_map.find(token.value()) == token_regex_map.end())
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

void Parser::fill_parsing_table()
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

    for (auto &prod : productions)
    {
        for (auto &terminal : first_pos[prod.name()])
        {
            parsing_table[prod.name()][terminal] = production_indices[prod.name()];
        }

        if (first_pos[prod.name()].has_item("") != -1)
        {
            for (auto &terminal : follow_pos[prod.name()])
            {
                parsing_table[prod.name()][terminal] = production_indices[prod.name()];
            }
        }
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

void Parser::write_parser()
{

    ifstream frame_file("parser.frame");
    ofstream header(new_compiler_name + "/parser.h");
    ofstream cpp_file(new_compiler_name + "/parser.cpp");
    streampos f_header_pos, f_impl_pos;

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
    f_impl_pos = frame_file.tellg();

    // // Declaration
    // header << ident_str(prod_name + "(" + args + ");", 4) << endl;

    // // Implementation
    // cpp_file << "void Parser::" + prod_name + "(" + args + ")" << endl << "{" << endl;

    // while (scanner.look_ahead().name() != ".")
    // {
    //     get();

    //     if (current_token.name() == "semAction")
    //     {
    //         cpp_file << ident_str(current_token.value().substr(2, current_token.value().size() - 4), 4) << endl;
    //     }
    //     else if (current_token.name() == "ident")
    //     {
    //         if (token_regex_map.find(current_token.value()) != token_regex_map.end())
    //         {
    //             cpp_file << ident_str("expect(", 4) + current_token.value() + ");" << endl;
    //         }
    //         else
    //         {
    //             string ident_args = "";
    //             if (scanner.look_ahead().name() == "attr")
    //             {
    //                 auto attr_val = scanner.look_ahead().value();
    //                 ident_args = attr_val.substr(1, attr_val.size() - 2);
    //                 get();
    //             }
    //             cpp_file << ident_str(last_token().value(), 4) + "(" + ident_args + ");" << endl;
    //         }
    //     }
    //     else if (current_token.name() == "{")
    //     {
    //         string whl_condition = "";
    //         vector<string> cond_bodies = vector<string>{""};
    //         int cond_idx = 0;
    //         while (current_token.name() != "\x3")
    //         {
    //             get();
    //             if (current_token.name() == "string" || current_token.name() == "char")
    //             {
    //                 whl_condition += "scanner.look_ahead().name() == \"" +
    //                                  current_token.value().substr(1, current_token.value().size() - 2) + "\"";
    //             }
    //             else if (current_token.name() == "ident")
    //             {
    //                 cond_bodies[cond_idx] = cond_bodies[cond_idx] + current_token.value() + "();\n";
    //             }
    //             else if (current_token.name() == "attr")
    //             {
    //                 auto prev_str = cond_bodies[cond_idx];
    //                 string attr_val = current_token.value().substr(1, current_token.value().size() - 2);
    //                 cond_bodies[cond_idx] = prev_str.insert(prev_str.size() - 3, attr_val);
    //             }
    //             else if (current_token.name() == "semAction")
    //             {
    //                 string sem_act_val = current_token.value().substr(2, current_token.value().size() - 4);
    //                 cond_bodies[cond_idx] = cond_bodies[cond_idx] + sem_act_val + "\n";
    //             }
    //             else if (current_token.name() == "\x2")
    //             {
    //                 whl_condition += " || ";
    //                 cond_bodies.push_back("");
    //                 cond_idx++;
    //             }
    //         }
    //         cpp_file << ident_str("while(" + whl_condition + ")", 4) << endl << ident_str("{", 4) << endl;
    //         if (cond_idx == 0)
    //         {
    //             cpp_file << ident_str(cond_bodies[0], 8);
    //         }
    //         else
    //         {
    //             for (auto &body_str : cond_bodies)
    //             {
    //                 auto pos = whl_condition.find("||");
    //                 auto if_cond = whl_condition.substr(0, pos);
    //                 whl_condition.erase(0, pos + 3);
    //                 cpp_file << ident_str("if (" + if_cond + ")", 8) << endl << ident_str("{", 8) << endl;
    //                 cpp_file << ident_str("get();", 12) << endl;
    //                 cpp_file << ident_str(body_str, 12);
    //                 cpp_file << ident_str("}", 8) << endl;
    //             }
    //         }
    //         cpp_file << ident_str("}", 4) << endl;
    //     }
    //     else if (current_token.name() == "[")
    //     {
    //         expect("string"); // TODO Expect string or char
    //         cpp_file << ident_str("if (scanner.look_ahead().name() == ", 4) + current_token.value() + ")" << endl
    //                  << ident_str("{", 4) << endl;
    //         cpp_file << ident_str("get();", 8) << endl;
    //         while (current_token.name() != "\x4")
    //         {
    //             get();
    //             if (current_token.name() == "ident")
    //             {
    //                 string ident_args = "";
    //                 if (scanner.look_ahead().name() == "attr")
    //                 {
    //                     auto attr_val = scanner.look_ahead().value();
    //                     ident_args = attr_val.substr(1, attr_val.size() - 2);
    //                     get();
    //                 }
    //                 cpp_file << ident_str(last_token().value(), 8) + "(" + ident_args + ");" << endl;
    //             }
    //             else if (current_token.name() == "semAction")
    //             {
    //                 cpp_file << ident_str(current_token.value().substr(2, current_token.value().size() - 4), 8) <<
    //                 endl;
    //             }
    //         }
    //         cpp_file << ident_str("}", 4) << endl;
    //     }
    //     else if (current_token.name() == "\x5")
    //     {
    //     }
    //     else if (current_token.name() == "=")
    //     {
    //         // TODO Handle error (Production was probably not closed)
    //     }
    // }

    // cpp_file << "}" << endl << endl;
}

void Parser::syn_error()
{
    auto prev_token = last_token();
    auto error =
        Error("Syntatic error found after (" + prev_token.name() + ", " + prev_token.value() + ")", SYNTACTIC_ERROR);
    cout << error.value() << endl;
    cout << "Expected token: (\".\", \'.\')" << endl;
}