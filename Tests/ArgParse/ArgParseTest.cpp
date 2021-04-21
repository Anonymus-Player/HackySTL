#include <ArgParse.hpp>
#include <String.hpp>

int main(int argc, const char** argv)
{
    auto arg_parser = hsd::argument_parser{"This is a test"};

    arg_parser.add("-clear", 0, 
        [](hsd::parser_stream&){printf("\e[1;1H\e[2J");}, 
        "-clear: Clears the console"
    );

    arg_parser.add("-exec", 1, [](hsd::parser_stream& stream)
    {
        hsd::string command;
        stream.set_data(command).unwrap(HSD_FUNCTION_NAME);
        system(command.c_str());
    }, "-exec: executes a command");

    arg_parser.parse(argc, argv);
}