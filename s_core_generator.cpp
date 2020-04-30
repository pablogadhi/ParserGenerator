#include "s_core_generator.h"
#include "dfa_builder.h"
#include <stack>

SCoreGenerator::SCoreGenerator()
{
}

SCoreGenerator::~SCoreGenerator()
{
}

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
    else if (c == '}' || c == ']')
    {
        return 3;
    }
    return 0;
}

bool SCoreGenerator::is_regex_operator(Token<Set<char>> token)
{
    auto set = token.value();
    return (intersec_between_sets(set, regex_operators).size() != 0 && set.size() == 1 &&
            (string(1, set[0]) == token.name() || token.name() == ""));
}

bool SCoreGenerator::is_special_char(Token<Set<char>> token)
{
    auto set = token.value();
    return (intersec_between_sets(set, special_chars).size() != 0 && set.size() == 1 &&
            string(1, set[0]) == token.name());
}

vector<Token<Set<char>>> SCoreGenerator::infix_to_postfix(vector<Token<Set<char>>> infix_regex,
                                                          function<int(char)> get_precedence)
{
    stack<Token<Set<char>>> char_stack;
    vector<Token<Set<char>>> output;

    for (auto &t : infix_regex)
    {
        if (!is_regex_operator(t) && !is_special_char(t))
        {
            output.push_back(t);
        }
        else if (t.name() == "(")
        {
            char_stack.push(t);
        }
        else if (t.name() == ")")
        {
            while (char_stack.top().name() != "(")
            {
                output.push_back(char_stack.top());
                char_stack.pop();
            }
            char_stack.pop();
        }
        else if (is_regex_operator(t))
        {
            while (!char_stack.empty() && get_precedence(t.value()[0]) <= get_precedence(char_stack.top().value()[0]))
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

TreeNode<int> SCoreGenerator::make_syntax_tree(Token<Set<char>> symbol_as_set, int &name_index,
                                               vector<TreeNode<int>> children)
{
    // TODO Handle symbols with more than one character
    int symbol = (int)(symbol_as_set.value()[0]);
    if (children.size() == 1)
    {
        return TreeNode<int>(-1, symbol, make_shared<TreeNode<int>>(children[0]));
    }
    return TreeNode<int>(-1, symbol, make_shared<TreeNode<int>>(children[0]), make_shared<TreeNode<int>>(children[1]));
}

TreeNode<int> SCoreGenerator::tree_from_set(Token<Set<char>> token, int &name_index)
{
    stack<TreeNode<int>> char_stack;
    for (auto &c : token.value())
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

TreeNode<int> SCoreGenerator::basic_syntax_node_generator(Token<Set<char>> symbol, int &name)
{
    TreeNode<int> node;
    if (symbol.value().size() > 1)
    {
        node = tree_from_set(symbol, name);
    }
    else
    {
        // TODO Handle symbols with more than one character
        node = TreeNode<int>(name, (int)(symbol.value()[0]));
        name++;
    }
    return node;
}

bool is_operator_binary(Token<Set<char>> op)
{
    return (op.name() == "|" || op.name() == "\0");
}

TreeNode<int> SCoreGenerator::postfix_eval(vector<Token<Set<char>>> postfix_expr)
{
    stack<TreeNode<int>> m_stack;
    int state_index = 0;
    for (auto &t : postfix_expr)
    {
        TreeNode<int> a, b;
        if (is_regex_operator(t))
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
            m_stack.push(basic_syntax_node_generator(t, state_index));
        }
    }
    return m_stack.top();
}

vector<Token<Set<char>>> SCoreGenerator::format_regex(vector<Token<Set<char>>> regex)
{
    vector<Token<Set<char>>> result;
    for (int i = 0; i < regex.size() - 1; i++)
    {
        Token<Set<char>> first = regex[i], second = regex[i + 1];
        result.push_back(first);
        if (first.name() != "(" && second.name() != ")" && !is_regex_operator(second) && !is_operator_binary(first))
        {
            result.push_back(Token<Set<char>>("\0", Set<char>{'\0'}));
        }
    }
    result.push_back(regex[regex.size() - 1]);
    return result;
}

DFA SCoreGenerator::generate_dfa_finder(unordered_map<string, vector<Token<Set<char>>>> all_regex,
                                        bool append_operators, bool debug)
{
    vector<DFA> regex_dfa;
    auto builder = DFABuilder();
    auto hashtag = Token<Set<char>>("#", Set<char>{'#'});
    auto left_p = Token<Set<char>>("(", Set<char>{'('});
    auto right_p = Token<Set<char>>(")", Set<char>{')'});

    int state_name = 0;
    int dfa_idx = 0;
    for (auto &[r_name, regex] : all_regex)
    {
        regex.insert(regex.begin(), left_p);
        regex.push_back(right_p);
        regex.push_back(hashtag);

        auto formated_regex = format_regex(regex);
        auto postfix_regex = infix_to_postfix(formated_regex, get_precedence);
        auto syntax_tree = postfix_eval(postfix_regex);
        auto dfa = builder.dfa_from_syntax_tree(syntax_tree, state_name, r_name);
        string d_name = "DFA_" + to_string(dfa_idx);
        if (debug)
        {
            dfa.draw_machine(d_name);
        }
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

    if (append_operators)
    {
        // Apend a dfa for each opearator and special character, except for the 0 character
        for (auto &op : diff_between_sets(union_between_sets(regex_operators, special_chars), Set<char>{'\0'}))
        {
            auto start = State(state_name);
            auto end = State(state_name + 1);
            state_name += 2;
            end.set_as_accepting(true);
            end.set_reference_name(string(1, op));
            start.add_t_function(make_pair((int)op, make_shared<State>(end)));
            first_state->add_t_function(make_pair(1, make_shared<State>(start)));
        }

        // Apend a dfa for each adititonal regex neede to read a COCOL file
        for (auto &op_regex : coco_operators)
        {
            shared_ptr<State> current_state = make_shared<State>(State(state_name));
            state_name++;
            first_state->add_t_function(make_pair(1, current_state));
            for (int i = 0; i < op_regex.size(); i++)
            {
                auto state = State(state_name);
                state_name++;
                if (i == op_regex.size() - 1)
                {
                    state.set_as_accepting(true);
                    state.set_reference_name(op_regex);
                }
                auto state_ptr = make_shared<State>(state);
                current_state->add_t_function(make_pair((int)op_regex[i], state_ptr));
                current_state = state_ptr;
            }
        }
    }

    auto nfa = NFA(first_state, nullptr);
    state_name = 0;
    auto final_dfa = builder.dfa_from_nfa(nfa, state_name);

    return final_dfa;
}

Set<char> SCoreGenerator::operators()
{
    return regex_operators;
}
