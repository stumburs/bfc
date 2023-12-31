# Brainfuck compiler (using g++)

bfc is a simple Brainfuck compiler written in C++. It compiles Brainfuck source code into an executable using the GNU C++ compiler (`g++`).

## Usage

To compile a Brainfuck source code file, use the following command:

`bfc <input_file> [options]`

### Options

-   `-cpp`: Skip compiling to executable, instead translate only to a .cpp file.
-   `-fast`: Adds the `-O3` optimization parameter to `g++`.
-   `-info`: Displays additional info about the .bf file.
-   `-keep`: Keeps the intermediary .cpp files after compiling.
-   `-o <filename>` Compile with a specific executable name
-   `-run`: Runs the program after compiling.
-   `-v`: Enable verbose output, providing detailed compilation progress and diagnostics.

### Example

Compile a Brainfuck file while optimizing for speed, then run the program:

`bfc bf_file.bf -fast -run`

## Help

To display usage and available options, use the `--help` command:

`bfc --help`

## Requirements

-   g++ compiler
-   GNU C++ Standard Library

## License

This project is licensed under the [MIT License](LICENSE).
