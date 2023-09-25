#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>

const std::string version = "1.2";

int main(int argc, char *argv[])
{
    bool run_after_compiling = false;
    bool keep_intermediary_files = false;
    bool optimize_for_speed = false;
    bool cpp_file_only = false;
    bool show_info = false;
    std::string custom_output_filename;
    bool verbose_logging = false;

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
        std::cout << "\t-info\t\tDisplay additional info\n";
        std::cout << "\t-keep\t\tKeeps the intermediary .cpp files after compiling\n";
        std::cout << "\t-o <filename>\tCompile with a specific executable name\n";
        std::cout << "\t-run\t\tRuns the program after compiling\n\n";
        std::cout << "\t-v\t\tEnable verbose output, providing detailed compilation progress and diagnostics.\n\n\n";
        return 0;
    }

    if (std::string(argv[1]) == "--version")
    {
        std::cout << "bfc " + version + "\nhttps://github.com/stumburs\n";
        return 0;
    }

    for (int i = 2; i < argc; i++)
    {
        std::string arg = std::string(argv[i]);

        if (arg == "-cpp")
            cpp_file_only = true;

        if (arg == "-fast")
            optimize_for_speed = true;

        if (arg == "-info")
            show_info = true;

        if (arg == "-keep")
            keep_intermediary_files = true;

        if (arg == "-run")
            run_after_compiling = true;

        if (arg == "-o")
        {
            // Check for agrument after -o
            if (argv[i + 1] != 0)
            {
                // Check if isn't a compile argument
                if (argv[i + 1][0] != '-')
                {
                    custom_output_filename = argv[i + 1];
                    i++;
                }
                else
                {
                    std::cerr << "No output filename provided" << std::endl;
                    return 1;
                }
            }
            else
            {
                std::cerr << "No output filename provided" << std::endl;
                return 1;
            }
        }

        if (arg == "-v")
            verbose_logging = true;
    }

    // Create input file
    std::string file_path = argv[1];
    std::ifstream input_file(file_path);
    std::string compiled_name = custom_output_filename.empty() ? file_path.substr(0, file_path.find_last_of('.')) : custom_output_filename;
    std::string output_file_path = compiled_name + ".temp.cpp";
    std::ofstream output_file(output_file_path);

    // Check if file successfully opened
    if (!input_file.is_open())
    {
        std::cout << "Failed to open file.\n";
        return 1;
    }

    // Verbose logging
    if (verbose_logging)
    {
        std::cout << "Args: " << std::endl;
        for (int i = 0; i < argc; i++)
            std::cout << "\t[" << i << "] " << argv[i] << std::endl;
        std::cout << std::endl;

        std::cout << "\tInput file path: " << file_path << std::endl;
        std::cout << "\tCompiled name: " << compiled_name << std::endl;
        std::cout << "\tIntermediary file path: " << output_file_path << std::endl;
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
    output_file << "char ch = 0;\n\n";
    output_file << "int main()\n";
    output_file << "{\n";

    // Extra info
    std::size_t value_increases = 0;
    std::size_t value_decreases = 0;
    std::size_t ptr_increases = 0;
    std::size_t ptr_decreases = 0;
    std::size_t print_statements = 0;
    std::size_t input_statements = 0;
    std::size_t loop_entries = 0;
    std::size_t loop_exits = 0;
    std::size_t unknown_characters = 0;

    // Transpile program and write to file
    std::size_t i = 0;
    std::size_t indent_level = 1;
    while (i < program.length())
    {
        switch (program[i])
        {
        case '>':
            output_file << std::string(indent_level, '\t') << "++ptr;\n";
            ptr_increases++;
            break;
        case '<':
            output_file << std::string(indent_level, '\t') << "--ptr;\n";
            ptr_decreases++;
            break;
        case '+':
            output_file << std::string(indent_level, '\t') << "++*ptr;\n";
            value_increases++;
            break;
        case '-':
            output_file << std::string(indent_level, '\t') << "--*ptr;\n";
            value_decreases++;
            break;
        case '.':
            output_file << std::string(indent_level, '\t') << "std::cout << *ptr;\n";
            print_statements++;
            break;
        case ',':
            output_file << std::string(indent_level, '\t') << "ch = std::cin.get();\n";
            output_file << std::string(indent_level, '\t') << "*ptr = ch;\n";
            input_statements++;
            break;
        case '[':
            output_file << std::string(indent_level, '\t') << "while (*ptr != 0)\n"
                        << std::string(indent_level, '\t') << "{\n";
            indent_level++;
            loop_entries++;
            break;
        case ']':
            indent_level--;
            output_file << std::string(indent_level, '\t') << "}\n";
            loop_exits++;
            break;
        default:
            unknown_characters++;
            break;
        }
        ++i;
    }

    // Close main function
    output_file << "}\n";
    output_file.close();

    // Show additional info
    if (show_info || verbose_logging)
    {
        std::cout << "Info:" << std::endl;
        std::cout << "\tCharacter count: " << program.length() << std::endl;
        std::cout << "\tValue increases: " << value_increases << std::endl;
        std::cout << "\tValue decreases: " << value_decreases << std::endl;
        std::cout << "\tPointer increases: " << ptr_increases << std::endl;
        std::cout << "\tPointer decreases: " << ptr_decreases << std::endl;
        std::cout << "\tPrint statements: " << print_statements << std::endl;
        std::cout << "\tInput statements: " << input_statements << std::endl;
        std::cout << "\tLoop entries: " << loop_entries << std::endl;
        std::cout << "\tLoop exits: " << loop_exits << std::endl;
        std::cout << "\tUnknown characters: " << unknown_characters << std::endl;
        std::cout << std::endl;
    }

    // Skip compiling
    if (cpp_file_only)
    {
        std::cout << "Translating to .cpp successful\n";
        return 0;
    }

    // Run g++ compiler to compile program
    if (output_file.good())
    {
        std::string compile_command = "g++ " + output_file_path + " -o " + compiled_name + " -Wall -Wextra -pedantic";

        if (optimize_for_speed)
            compile_command += " -O3";

        if (verbose_logging)
        {
            std::cout << "\tCompile command: " << compile_command << std::endl;
            std::cout << "\tCompiling..." << std::endl;
            std::cout << std::endl;
        }

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
    {
        if (verbose_logging)
            std::cout << "\tRemoving intermediary file..." << std::endl;
        std::remove(output_file_path.c_str());
    }

    // Run the compiled executable
    if (run_after_compiling)
    {
        if (verbose_logging)
            std::cout << "\tRunning..." << std::endl;
        std::system(compiled_name.c_str());
    }

    return 0;
}