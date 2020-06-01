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

    ifstream frame_file("parser.frame");
    ofstream header(new_compiler_name + "/parser.h");
    ofstream cpp_file(new_compiler_name + "/parser.cpp");
    streampos f_header_pos, f_impl_pos;
    write_parser_start(frame_file, header, cpp_file, f_header_pos, f_impl_pos);

    // Productions
    expect("PRODUCTIONS");
    while (scanner.look_ahead().name() == "ident")
    {
        prod_decl(frame_file, header, cpp_file);
    }

    write_parser_end(frame_file, header, cpp_file, f_header_pos, f_impl_pos);
    frame_file.close();
    header.close();
    cpp_file.close();

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
        else if (current_token.name() == "{" || current_token.name() == "[")
        {
            regex.push_back(Token<Set<char>>("(", Set<char>{'('}));
        }
        else if (current_token.name() == "}" || current_token.name() == "]")
        {
            regex.push_back(Token<Set<char>>(")", Set<char>{')'}));
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

void Parser::prod_decl(ifstream &frame_file, ofstream &header, ofstream &cpp_file)
{
    get();

    auto prod_name = current_token.value();
    string args = "";

    if (soft_expect("attr"))
    {
        args = current_token.value().substr(1, current_token.value().size() - 2);
    }

    // Declaration
    header << ident_str(prod_name + "(" + args + ");", 4) << endl;

    // Implementation
    cpp_file << "void Parser::" + prod_name + "(" + args + ")" << endl << "{" << endl;

    expect("=");

    while (scanner.look_ahead().name() != ".")
    {
        get();

        if (current_token.name() == "semAction")
        {
            cpp_file << ident_str(current_token.value().substr(2, current_token.value().size() - 4), 4) << endl;
        }
        else if (current_token.name() == "ident")
        {
            if (token_regex_map.find(current_token.value()) != token_regex_map.end())
            {
                cpp_file << ident_str("expect(", 4) + current_token.value() + ");" << endl;
            }
            else
            {
                string ident_args = "";
                if (scanner.look_ahead().name() == "attr")
                {
                    auto attr_val = scanner.look_ahead().value();
                    ident_args = attr_val.substr(1, attr_val.size() - 2);
                    get();
                }
                cpp_file << ident_str(last_token().value(), 4) + "(" + ident_args + ");" << endl;
            }
        }
        else if (current_token.name() == "{")
        {
            string whl_condition = "";
            vector<string> cond_bodies = vector<string>{""};
            int cond_idx = 0;
            while (current_token.name() != "}")
            {
                get();
                if (current_token.name() == "string" || current_token.name() == "char")
                {
                    whl_condition += "scanner.look_ahead().name() == \"" +
                                     current_token.value().substr(1, current_token.value().size() - 2) + "\"";
                }
                else if (current_token.name() == "ident")
                {
                    cond_bodies[cond_idx] = cond_bodies[cond_idx] + current_token.value() + "();\n";
                }
                else if (current_token.name() == "attr")
                {
                    auto prev_str = cond_bodies[cond_idx];
                    string attr_val = current_token.value().substr(1, current_token.value().size() - 2);
                    cond_bodies[cond_idx] = prev_str.insert(prev_str.size() - 3, attr_val);
                }
                else if (current_token.name() == "semAction")
                {
                    string sem_act_val = current_token.value().substr(2, current_token.value().size() - 4);
                    cond_bodies[cond_idx] = cond_bodies[cond_idx] + sem_act_val + "\n";
                }
                else if (current_token.name() == "|")
                {
                    whl_condition += " || ";
                    cond_bodies.push_back("");
                    cond_idx++;
                }
            }
            cpp_file << ident_str("while(" + whl_condition + ")", 4) << endl << ident_str("{", 4) << endl;
            if (cond_idx == 0)
            {
                cpp_file << ident_str(cond_bodies[0], 8);
            }
            else
            {
                for (auto &body_str : cond_bodies)
                {
                    auto pos = whl_condition.find("||");
                    auto if_cond = whl_condition.substr(0, pos);
                    whl_condition.erase(0, pos + 3);
                    cpp_file << ident_str("if (" + if_cond + ")", 8) << endl << ident_str("{", 8) << endl;
                    cpp_file << ident_str("get();", 12) << endl;
                    cpp_file << ident_str(body_str, 12);
                    cpp_file << ident_str("}", 8) << endl;
                }
            }
            cpp_file << ident_str("}", 4) << endl;
        }
        else if (current_token.name() == "[")
        {
            expect("string"); // TODO Expect string or char
            cpp_file << ident_str("if (scanner.look_ahead().name() == ", 4) + current_token.value() + ")" << endl
                     << ident_str("{", 4) << endl;
            cpp_file << ident_str("get();", 8) << endl;
            while (current_token.name() != "]")
            {
                get();
                if (current_token.name() == "ident")
                {
                    string ident_args = "";
                    if (scanner.look_ahead().name() == "attr")
                    {
                        auto attr_val = scanner.look_ahead().value();
                        ident_args = attr_val.substr(1, attr_val.size() - 2);
                        get();
                    }
                    cpp_file << ident_str(last_token().value(), 8) + "(" + ident_args + ");" << endl;
                }
                else if (current_token.name() == "semAction")
                {
                    cpp_file << ident_str(current_token.value().substr(2, current_token.value().size() - 4), 8) << endl;
                }
            }
            cpp_file << ident_str("}", 4) << endl;
        }
        else if (current_token.name() == "(")
        {
        }
        else if (current_token.name() == "=")
        {
            // TODO Handle error (Production was probably not closed)
        }
    }

    cpp_file << "}" << endl << endl;

    // TODO Handle closing error ("}", ")" or "]" missing)
    // TODO expect string after "{" or "["
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
                            "state_ptrs[" + t_state_name + "]" + "));"
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

void Parser::write_parser_start(ifstream &frame_file, ofstream &header, ofstream &cpp_file, streampos &f_header_pos,
                                streampos &f_impl_pos)
{

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
}

void Parser::write_parser_start(ifstream &frame_file, ofstream &header, ofstream &cpp_file, streampos &f_header_pos,
                                streampos &f_impl_pos)
{
    string line;
    // Finish coping the implementation file
}

void Parser::syn_error()
{
    auto prev_token = last_token();
    auto error =
        Error("Syntatic error found after (" + prev_token.name() + ", " + prev_token.value() + ")", SYNTACTIC_ERROR);
    cout << error.value() << endl;
    cout << "Expected token: (\".\", \'.\')" << endl;
}