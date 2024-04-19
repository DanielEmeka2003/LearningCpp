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

#include "readfile.h"
#include "ncLexer.h"
#include "ncParser.h"
// #include "ncCodeGen.h"

using namespace std::string_literals;
using namespace std::string_view_literals;

void signal_handler(int signal)
{
    System::cerror.writews_endl("Caught Signal", signal, "[std::exit(signal) called]");
    std::exit(signal);
}

void checkPLOFile(const std::filesystem::path& path);

main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    using namespace Streams;
    
    std::signal(SIGINT, signal_handler);
    std::signal(SIGSEGV, signal_handler);
    
    // if (argc != 2)
    // {
    //     System::cerror.write_endl("Usage: ncc [file].nc");
    //     return EXIT_FAILURE;
    // }
    
    try
    {
        //integrate noexcept in all code
        /*Find out why function identifers can be bound to both lvalue and rvalue references*/
        
        //checkPLOFile(argv[1]);
        
        Myfcn::ReadFile ncFile{argv[1]};
        
        Nc::Lexer lex{ ncFile.storeFileContents().retrieveBuffer() };
        lex.logBoxStyle(Nc::Log::BoxLineStyle::bold);
        lex.lexFileBuffer();
        
        System::coutput.boolalpha();
        for (auto &&i : lex.getTokenList())
        System::coutput.writews_endl(i.token, i.optLine.value_or(0), i.line, i.relativeColumn, i.absoluteColumn, i.isReserved, i.isIdentifier, i.literalType, i.isSymbol);
        
        System::coutput.write_endl("\n\n");
        std::exit(0);

        Nc::Parser parse{lex};//change the parser so it can reference the lexer
        parse.logBoxStyle(Nc::Log::BoxLineStyle::bold);
        parse.parseTokenList();

        System::coutput.writewl(parse.getAST().operator Nc::Root &());
        
        // Nc::NcCodeGen codegen{ std::move(parse.getAST()) };
        // codegen.generate();
        
        /*Brace initialization is not to be used with function template type:
        main.cpp:125:74: error: invalid initialization of non-const reference of type 'std::basic_ostream<char>& (&)(std::basic_ostream<char>&)' from an rvalue of type '<brace-enclosed initializer list>'
        125 |         [[maybe_unused]] auto&& g{std::endl<char, std::char_traits<char>>};
            |                                                                               
                
        [[maybe_unused]] auto&& g{std::endl<char, std::char_traits<char>>};
        */
    }
    catch(const std::exception& e)
    {
        System::cerror.writews_endl('\n', std::source_location::current().function_name(), "Exception{", e.what(), '}');
    }
    catch(...)
    {
        System::cerror.writews_endl('\n', std::source_location::current().function_name(), "Inside catch all exception");
    }
    
    return EXIT_SUCCESS;
}

void checkPLOFile(const std::filesystem::path& path)
{
    if (path.has_extension() and (path.extension() != ".pl0"))
    {
        System::cerror.writews_endl("File must end in '.pl0'");
        std::exit(EXIT_FAILURE);
    }
    else if (!path.has_extension())
    {
        System::cerror.writews_endl("File must end in '.pl0'");
        std::exit(EXIT_FAILURE);
    }
}
