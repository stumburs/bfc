#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>

int main(int argc, char *argv[])
{
    bool run_after_compiling = false;
    bool keep_intermediary_files = false;
    bool optimize_for_speed = false;
    bool cpp_file_only = false;

    if (argc < 2)
    {
        std::cerr << "No input file supplied.\nUse \"bfc --help\" for more information\n";
        return 1;
    }

    if (std::string(argv[1]) == "--help")
    {
        std::cout << "Syntax:\n\tbfc <input_file> [options]\n\n";
        std::cout << "Example usage:\n";
        std::cout << "\tbfc bf_file.bf -fast -run\tCompiles \"bf_file.bf\" using the -O3 parameter for g++, then runs the executable.\n\n";
        std::cout << "Available commands:\n";

        std::cout << "\t--help\t\tDisplays this screen\n";
        std::cout << "\t--version\tDisplays installed version\n\n";

        std::cout << "Compile arguments:\n";
        std::cout << "\t-cpp\t\tSkip compiling to executable, instead translate only to a .cpp file\n";
        std::cout << "\t-fast\t\tAdds -O3 parameter to g++\n";
        std::cout << "\t-keep\t\tKeeps the intermediary .cpp files after compiling\n";
        std::cout << "\t-run\t\tRuns the program after compiling\n\n\n";
        return 0;
    }

    if (std::string(argv[1]) == "--version")
    {
        std::cout << "bfc 1.0\nhttps://github.com/stumburs\n";
        return 0;
    }

    for (int i = 2; i < argc; i++)
    {
        std::string arg = std::string(argv[i]);

        if (arg == "-run")
            run_after_compiling = true;

        if (arg == "-keep")
            keep_intermediary_files = true;

        if (arg == "-fast")
            optimize_for_speed = true;

        if (arg == "-cpp")
            cpp_file_only = true;
    }

    // Create input file
    std::string file_path = argv[1];
    std::ifstream input_file(file_path);
    std::string file_name = file_path.substr(0, file_path.find_last_of('.'));
    std::string output_file_path = file_name + ".temp.cpp";
    std::ofstream output_file(output_file_path);

    // Check if file successfully opened
    if (!input_file.is_open())
    {
        std::cout << "Failed to open file.\n";
        return 1;
    }

    // Preprocess file to remove non-Brainfuck characters
    std::string program((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    std::erase_if(program, [](char c)
                  { return !strchr("><+-.,[]", c); });
    input_file.close();

    // Initialize boilerplate code
    output_file << "#include <iostream>\n";
    output_file << "#include <array>\n\n";
    output_file << "constexpr std::size_t mem_size = 30000;\n";
    output_file << "std::array<uint8_t, mem_size> mem = {0};\n";
    output_file << "uint8_t* ptr = mem.data();\n\n";
    output_file << "int main()\n";
    output_file << "{\n";

    // Transpile program and write to file
    std::size_t i = 0;
    std::size_t indent_level = 1;
    while (i < program.length())
    {
        switch (program[i])
        {
        case '>':
            output_file << std::string(indent_level, '\t') << "++ptr;\n";
            break;
        case '<':
            output_file << std::string(indent_level, '\t') << "--ptr;\n";
            break;
        case '+':
            output_file << std::string(indent_level, '\t') << "++*ptr;\n";
            break;
        case '-':
            output_file << std::string(indent_level, '\t') << "--*ptr;\n";
            break;
        case '.':
            output_file << std::string(indent_level, '\t') << "std::cout << *ptr;\n";
            break;
        case ',':
            output_file << std::string(indent_level, '\t') << "std::cin >> *ptr;\n";
            break;
        case '[':
            output_file << std::string(indent_level, '\t') << "while (*ptr != 0)\n"
                        << std::string(indent_level, '\t') << "{\n";
            indent_level++;
            break;
        case ']':
            indent_level--;
            output_file << std::string(indent_level, '\t') << "}\n";
        default:
            break;
        }
        ++i;
    }

    // Close main function
    output_file << "}\n";
    output_file.close();

    if (cpp_file_only)
    {
        std::cout << "Translating to .cpp successful\n";
        return 0;
    }

    // Run g++ compiler to compile program
    if (output_file.good())
    {
        std::string compile_command = "g++ " + output_file_path + " -o " + file_name + " -Wall -Wextra -pedantic";

        if (optimize_for_speed)
            compile_command += " -O3";

        int compile_result = std::system(compile_command.c_str());

        if (compile_result == 0)
        {
            std::cout << "Compilation successful.\n"
                      << std::endl;
        }
        else
        {
            std::cerr << "Compilation failed." << std::endl;
            return 1;
        }
    }

    // Delete intermediary C++ files if set
    if (!keep_intermediary_files)
        std::remove(output_file_path.c_str());

    // Run the compiled executable
    if (run_after_compiling)
        std::system(file_name.c_str());

    return 0;
}