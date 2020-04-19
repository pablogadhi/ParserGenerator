#include "parser.h"
#include "scanner.h"
#include <iostream>
#include <memory>

using namespace std;

// bool is_char_in_vector(vector<char> vector_to_check, char c)
// {
//     return find(vector_to_check.begin(), vector_to_check.end(), c) != vector_to_check.end();
// }

// NFA nfa_operations(char op, int &node_index, vector<NFA> operands)
// {
//     if (op == '|')
//     {
//         return nfa_from_or(operands[0], operands[1], node_index);
//     }
//     else if (op == '.')
//     {
//         return nfa_from_concat(operands[0], operands[1]);
//     }
//     else if (op == '*')
//     {
//         return nfa_from_kleene(operands[0], node_index);
//     }
//     else if (op == '+')
//     {
//         auto r_copy = operands[0].make_copy(node_index);
//         auto kleene_r = nfa_from_kleene(operands[0], node_index);
//         return nfa_from_concat(r_copy, kleene_r);
//     }
//     else if (op == '?')
//     {
//         auto e_transition = nfa_from_transition(36, node_index);
//         return nfa_from_or(operands[0], e_transition, node_index);
//     }
//     return NFA(NULL, NULL);
// }

int main(int argc, char const *argv[])
{
    string test_file = "Coco.atg";
    auto scanner = Scanner(test_file);
    auto parser = Parser(scanner);
    parser.parse();
    return 0;
}