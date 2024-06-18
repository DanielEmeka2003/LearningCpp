#include <filesystem>

#include <exception>
#include <stdexcept>

#include <string>
#include <string_view>
// #include <format>

// #include <array>
// #include <vector>
// #include <initializer_list>
// #include <list>

// #include <algorithm>
// #include <iterator>
// #include <numeric>
// #include <functional>
// #include <thread>

#include <regex>

#include <source_location>
#include <csignal>

#include "stream/i_o.h"

#include "ncLexer.h"
#include "ncParser.h"
// #include "ncCodeGen.h"


using namespace std::string_literals;
using namespace std::string_view_literals;

void signal_handler(int signal)
{
    io::cerr.writews_nl("Caught Signal", signal, "[std::exit(signal) called]");
    std::exit(signal);
}

void checkPLOFile(const std::filesystem::path& path);

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    using namespace Streams;
    
    std::signal(SIGINT, signal_handler);
    std::signal(SIGSEGV, signal_handler);
    
    if (argc != 2)
    {
        io::cerr.write_nl("Usage: ncc [file].nc");
        return EXIT_FAILURE;
    }
    
    try
    {   
        //checkPLOFile(argv[1]);
        
        Nc::Lexer lex{ misc::read_file(std::filesystem::path{ argv[1] }) };
        lex.logBoxStyle(Nc::Log::BoxLineStyle::bold);
        lex.lexFileBuffer();
        
        // io::cout.boolalpha();
        // for (auto &&i : lex.getTokenDataList())
        // io::cout.writews_nl("\033[40;32;1m", i.token, "\033[0m", i.optLine.value_or(0), '[', i.line, ']', '(', i.relativeColumn, '-', i.absoluteColumn, ')', "is_r:", i.isReserved, "is_i:", i.isIdentifier, "lT:", i.literalType, "is_s:", i.isSymbol);
        
        // std::exit(0);
        // io::cout.write_nl("\n\n");

        Nc::Parser parse{ lex };//change the parser so it can reference the lexer
        parse.logBoxStyle(Nc::Log::BoxLineStyle::bold);
        parse.parseTokenList();

        // io::cout.writewl(parse.getAST().operator Nc::Root &());
        
        // Nc::NcCodeGen codegen{ std::move(parse.getAST()) };
        // codegen.generate();
        
        /*Brace initialization is not to be used with function template type:
        main.cpp:125:74: error: invalid initialization of non-const reference of type 'std::basic_ostream<char>& (&)(std::basic_ostream<char>&)' from an rvalue of type '<brace-enclosed initializer list>'
        125 |         [[maybe_unused]] auto&& g{std::nl<char, std::char_traits<char>>};
            |                                                                               
                
        [[maybe_unused]] auto&& g{std::nl<char, std::char_traits<char>>};
        */
    }
    catch(const std::exception& e)
    {
        io::cerr.writews_nl('\n', std::source_location::current().function_name(), "Exception{", e.what(), '}');
    }
    catch(...){}
    
    return EXIT_SUCCESS;
}

void checkPLOFile(const std::filesystem::path& path)
{
    if (path.has_extension() and (path.extension() != ".pl0"))
    {
        io::cerr.writews_nl("File must end in '.pl0'");
        std::exit(EXIT_FAILURE);
    }
    else if (!path.has_extension())
    {
        io::cerr.writews_nl("File must end in '.pl0'");
        std::exit(EXIT_FAILURE);
    }
}
