#include "parser.h"
#include "scanner.h"
#include <filesystem>
#include <iostream>
#include <memory>

using namespace std;
using namespace filesystem;

const vector<path> files_to_copy = {"set.h",      "state.h", "state.cpp", "state_machine.h", "state_machine.cpp",
                                    "main.frame", "utils.h", "error.h",   "error.cpp"};

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        cout << "Invalid number of arguments!" << endl;
        return 0;
    }

    auto scanner = Scanner(argv[1]);
    auto parser = Parser(scanner);

    parser.parse();
    // Handle Errors
    if (scanner.errors().size() != 0)
    {
        for (auto &error : scanner.errors())
        {
            cout << "LEXICAL ERROR found in line " << error.line() << " and column " << error.column() << ": "
                 << error.value() << endl;
        }
        return -1;
    }

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

    ifstream cmake_template("CMakeLists.frame");
    ofstream cmake_file(compiler_path / "CMakeLists.txt");
    string line;

    getline(cmake_template, line);
    while (line != "-->executable_line")
    {
        cmake_file << line << endl;
        getline(cmake_template, line);
    }

    string compile_line = "add_executable(" + compiler_name + " " + compiler_name + ".cpp ";
    for (auto &file : files_to_copy)
    {
        if (file.extension() != ".frame")
        {
            compile_line = compile_line + file.string() + " ";
        }
    }

    compile_line = compile_line + "scanner.cpp parser.cpp)";
    cmake_file << compile_line << endl;

    while (!cmake_template.eof())
    {
        getline(cmake_template, line);
        cmake_file << line << endl;
    }

    cmake_file.close();
    cmake_template.close();

    cout << "A new scanner and parser were generated!" << endl;

    return 0;
}