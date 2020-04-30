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

bool SCoreGenerator::is_regex_operator(Set<char> set)
{
    return (intersec_between_sets(set, regex_operators).size() != 0 && set.size() == 1);
}

bool SCoreGenerator::is_special_char(Set<char> set)
{
    return (intersec_between_sets(set, special_chars).size() != 0 && set.size() == 1);
}

vector<Set<char>> SCoreGenerator::infix_to_postfix(vector<Set<char>> infix_regex, function<int(char)> get_precedence)
{
    stack<Set<char>> char_stack;
    vector<Set<char>> output;

    for (auto &t : infix_regex)
    {
        if (!is_regex_operator(t) && !is_special_char(t))
        {
            output.push_back(t);
        }
        else if (t.has_item('(') != -1 && t.size() == 1)
        {
            char_stack.push(t);
        }
        else if (t.has_item(')') != -1 && t.size() == 1)
        {
            while (char_stack.top().has_item('(') && t.size() == 1)
            {
                output.push_back(char_stack.top());
                char_stack.pop();
            }
            char_stack.pop();
        }
        else if (is_regex_operator(t))
        {
            while (!char_stack.empty() && get_precedence(t[0]) <= get_precedence(char_stack.top()[0]))
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

TreeNode<int> SCoreGenerator::make_syntax_tree(Set<char> symbol_as_set, int &name_index, vector<TreeNode<int>> children)
{
    // TODO Handle symbols with more than one character
    int symbol = (int)(symbol_as_set[0]);
    if (children.size() == 1)
    {
        return TreeNode<int>(-1, symbol, make_shared<TreeNode<int>>(children[0]));
    }
    return TreeNode<int>(-1, symbol, make_shared<TreeNode<int>>(children[0]), make_shared<TreeNode<int>>(children[1]));
}

TreeNode<int> SCoreGenerator::tree_from_set(Set<char> set, int &name_index)
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

TreeNode<int> SCoreGenerator::basic_syntax_node_generator(Set<char> symbol, int &name)
{
    TreeNode<int> node;
    if (symbol.size() > 1)
    {
        node = tree_from_set(symbol, name);
    }
    else
    {
        // TODO Handle symbols with more than one character
        node = TreeNode<int>(name, (int)(symbol[0]));
        name++;
    }
    return node;
}

bool is_operator_binary(Set<char> op)
{
    if (op.size() == 1 && (op[0] == '|' || op[0] == '\0'))
    {
        return true;
    }
    return false;
}

TreeNode<int> SCoreGenerator::postfix_eval(vector<Set<char>> postfix_expr)
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

vector<Set<char>> SCoreGenerator::format_regex(vector<Set<char>> regex)
{
    vector<Set<char>> result;
    for (int i = 0; i < regex.size() - 1; i++)
    {
        Set<char> first = regex[i], second = regex[i + 1];
        result.push_back(first);
        if ((first.size() != 1 || first.has_item('(') == -1) && (second.size() != 1 || second.has_item(')') == -1) &&
            !is_regex_operator(second) && !is_operator_binary(first))
        {
            result.push_back(Set<char>{'\0'});
        }
    }
    result.push_back(regex[regex.size() - 1]);
    return result;
}

TreeNode<int> SCoreGenerator::expand_tree(shared_ptr<TreeNode<Set<char>>> root, int &name_index)
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

DFA SCoreGenerator::generate_dfa_finder(unordered_map<string, vector<Set<char>>> all_regex, bool append_operators,
                                        bool debug)
{
    vector<DFA> regex_dfa;
    auto builder = DFABuilder();
    auto hashtag = Set<char>{'#'};
    auto left_p = Set<char>{'('};
    auto right_p = Set<char>{')'};

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
