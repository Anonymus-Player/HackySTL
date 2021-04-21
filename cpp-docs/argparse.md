# Argument parsing support

## Argument Parser introduced in the library
Implemented in header [ArgParser.hpp](../cpp/ArgParse.hpp)

## Classes:

### Parser stream

#### Definition:
```cpp
class parser_stream;
```

#### Description:
It is a stream which contains a range of values from `argv` and it's values will be parsed by the user later on

#### Member functions:
| Method | Arguments | Return type | Description |
| :----- | :-------- | :---------- | :---------- |
| `parser_stream` | `N/A` | `/*compiler defined*/` | Default constructor, initialized the argument buffer |
| `parser_stream` | `const parser_stream&` | `/*compiler defined*/` | Copy constructor (deleted) |
| `parser_stream` | `parser_stream&&` | `/*compiler defined*/` | Move constructor (deleted) |
| `operator=` | `const parser_stream&` | `parser_stream&` | attribution (deleted) |
| `operator=` | `parser_stream&&` | `parser_stream&` | attribution (deleted) |
| `set_data` | `Args&... args` | `Result<void, runtime_error>` | Parses the value hold in the argument buffer and set the values `args...` according to their type |
| `emplace` | `const string_view& arg` | `void` | Adds an argument to the buffer |
| `clear` | `N/A` | `void` | Clears the buffer |

### Argument parser

#### Definition:
```cpp
class argument_parser;
```

#### Description:
Splits the arguments into `parser_stream`s of different length which will be passed individually into functions invoked inside `parse`

#### Member functions:
| Method | Arguments | Return type | Description |
| :----- | :-------- | :---------- | :---------- |
| `argument_parser` | `const string_view& info` | `/*compiler defined*/` | Adds the first information into the dedicated buffer |
| `argument_parser` | `const argument_parser&` | `/*compiler defined*/` | Copy constructor (deleted) |
| `argument_parser` | `argument_parser&&` | `/*compiler defined*/` | Move constructor (deleted) |
| `operator=` | `const argument_parser&` | `argument_parser&` | attribution (deleted) |
| `operator=` | `argument_parser&&` | `argument_parser&` | attribution (deleted) |
| `add` | `const string_view& argument`, `usize num_args`, `function<void(parser_stream&)>&& func`, `const string_view& help`| `void` | Adds an argument with the following informations: what flag is, how many arguments can take, a function `func` responsible for parsing the stream mentioned earlier and helpful informations about that flag |
| `parse` | `i32 argc`, `const char** argv` | `void` | Parser the command line arguments into `parser_stream`s which will be used in functions invoked by the flags |

#### Example:
```cpp
#include <ArgParse.hpp>
#include <String.hpp>

int main(int argc, const char** argv)
{
    auto arg_parser = hsd::argument_parser{"This is a test"};

    arg_parser.add(
        "-clear", // Command flag
        0, // How many arguments can take after
        [](hsd::parser_stream&) 
        {       
            // Function responsible
            // for parsing the data
            printf("\e[1;1H\e[2J"); 
        }, 
        "-clear: Clears the console" // Helpful info about the flag
    );

    arg_parser.add(
        "-exec", // Command flag
        1, // How many arguments can take after
        [](hsd::parser_stream& stream)
        {
            // Function responsible
            // for parsing the data
            hsd::string command;
            // Actual parsing
            stream.set_data(command).unwrap(HSD_FUNCTION_NAME);
            system(command.c_str());
        }, 
        "-exec: executes a command" // Helpful info about the flag
    );

    // Parse all the arguments
    arg_parser.parse(argc, argv);
}
```