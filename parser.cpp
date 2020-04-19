#include "parser.h"
#include <iostream>

Parser::Parser(Scanner &sc) : scanner(sc)
{
}

Parser::~Parser()
{
}

void Parser::parse()
{
    la_token = scanner.scan();
    for (int i = 0; i < 10; i++)
    {
        c_token = la_token;
        cout << c_token.value() << endl;
        la_token = scanner.scan();
    }
}