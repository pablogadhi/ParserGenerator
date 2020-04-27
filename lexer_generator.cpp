#include "parser.h"
#include "scanner.h"
#include <filesystem>
#include <iostream>
#include <memory>

using namespace std;
using namespace filesystem;

const vector<path> files_to_copy = {"set.h",           "state.h",           "state.cpp",
                                    "state_machine.h", "state_machine.cpp", "main.frame"};

int main(int argc, char const *argv[])
{
    string test_file = "Coco.atg";

    auto scanner = Scanner(test_file);
    auto parser = Parser(scanner);

    parser.parse();
    auto compiler_name = parser.compiler_name();
    auto compiler_path = current_path() / compiler_name;
    create_directory(compiler_path);

    for (auto &file : files_to_copy)
    {
        auto abs_path = current_path() / file;
        copy(abs_path, compiler_path, copy_options::overwrite_existing);
    }

    rename(compiler_path / "main.frame", compiler_path / (compiler_name + ".cpp"));
    parser.write_scanner();

    return 0;
}