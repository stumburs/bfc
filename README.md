# Brainfuck compiler (using g++)

bfc is a simple Brainfuck compiler written in C++. It compiles Brainfuck source code into an executable using the GNU C++ compiler (`g++`).

## Usage

To compile a Brainfuck source code file, use the following command:

`bfc <input_file> [options]`

### Options

- `-keep`: Keeps the intermediary .cpp files after compiling.
- `-run`: Runs the program after compiling.
- `-fast`: Adds the `-O3` optimization parameter to `g++`.

### Example

Compile a Brainfuck file while optimizing for size, then run the program:

`bfc bf_file.bf -small -run`

## Help

To display usage and available options, use the `--help` command:

`bfc --help`


## Requirements

- C++ compiler (e.g., g++)
- GNU C++ Standard Library

## License

This project is licensed under the [MIT License](LICENSE).
