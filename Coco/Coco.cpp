#include "scanner.h"
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
    if(argc != 2)
    {
        cout << "Invalid number of arguments!" << endl;
        return 0;
    }

    auto scanner = Scanner(argv[1]);
    auto token = Token();
    while (token.name() != "EOF")
    {
        token = token.scan();
        cout << "(" << token.name() << ", " << token.value() << ")" << endl;
    }

    return 0;
}