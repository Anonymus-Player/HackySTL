#include <Json.hpp>
#include <Io.hpp>

static const char* test_filename = "Tests/Json/TestFile.json";

int main() 
{
    using namespace hsd::string_literals;
    setlocale(LC_ALL, "en_US.UTF-8");
    hsd::JsonStream<hsd::wchar> lexer;
    
    // Lex the string
    lexer.lex_file(test_filename).unwrap();
    lexer.push_eot().unwrap();

    hsd::JsonParser parser(lexer);
    // Parse the string
    auto value = parser.parse_next().unwrap();

    // Assert that the whole object is complete
    assert(value->is_complete());
    
    auto res = (*value)[L"LoS"_sv][0][L"age"_sv].as_num().unwrap();
}