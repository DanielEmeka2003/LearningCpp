#include <filesystem>
#include <stdexcept>

#include "pl0_parser.h"
#include "stream/output.h"

namespace Pl0
{
    PL0Parser::PL0Parser(const std::string& inputFile): m_lexer{inputFile}
    {
        std::filesystem::current_path("pl0cTempFiles");
        m_cppFile.open("cppFile.cpp");

        if (m_cppFile.is_open())
        {
            codeGenerator_startMainFcn();   
        }
        else
        {
            throw std::runtime_error{"pl0cTempFiles/cppFile.cpp could not be opened for writing"};
        }
        
    }
    
    void PL0Parser::displayLexedFile()
    {
        while ((m_lexReturnValue = m_lexer.lex()) != Pl0::PL0Lexer::Token::MISC)
        {
            System::coutput.writews_endl("Lex m_lexReturnValue:", +m_lexReturnValue);
            System::coutput.writews_endl("token:", m_lexer.getTokenString(), "line:", m_lexer.getLine(), "column:", m_lexer.getColumn());
        }
    }

    void  PL0Parser::parse()
    {
        program();
    }

    void PL0Parser::program()
    {
        next();
        block();
        expect(PL0Lexer::Token::DOT, "expected symbol dot(.) at the end of the block section");

        if (m_lexReturnValue != PL0Lexer::Token::MISC)
        {
            m_error.empty_str();
            m_error.write("extra tokens are not allowed at the end of a block section");
            errorOutput();   
        }
        else
        {
            codeGenerator_endMainFcn();
        }   
    }

    void PL0Parser::block()
    {
        // ++m_depthLevel;
        // if (m_depthLevel > maxDepthLevel)
        // {
        //     m_error.empty_str();
        //     m_error.write("max procedure depth level(80) exceeded, dail back a little");
        //     errorOutput();
        // }
        if (m_lexReturnValue == PL0Lexer::Token::CONST)
        {
            expect(PL0Lexer::Token::CONST);
            codeGenerator_constLLIdentifier();
            expect(PL0Lexer::Token::IDENTIFIER, "expected an identifier");
            expect(PL0Lexer::Token::EQUAL, "expected an equal sign(=)");
            codeGenerator_intializeIdentifier();
            expect(PL0Lexer::Token::NUMBER, "expected an integral number");

            while (m_lexReturnValue == PL0Lexer::Token::COMMA)
            {
                expect(PL0Lexer::Token::COMMA);
                codeGenerator_commaSeperatedIdentifier();
                expect(PL0Lexer::Token::IDENTIFIER, "expected an identifier");
                expect(PL0Lexer::Token::EQUAL, "expected an equal sign(=)");
                codeGenerator_intializeIdentifier();
                expect(PL0Lexer::Token::NUMBER, "expected an integral number");
            }
            codeGenerator_endWithSemiColon();
            expect(PL0Lexer::Token::SEMICOLON, "expected a semicolon");
        }

        if (m_lexReturnValue == PL0Lexer::Token::VAR)
        {
            expect(PL0Lexer::Token::VAR);
            codeGenerator_LLIdentifier();
            expect(PL0Lexer::Token::IDENTIFIER, "expected an identifier");

            while (m_lexReturnValue == PL0Lexer::Token::COMMA)
            {
                expect(PL0Lexer::Token::COMMA);
                codeGenerator_commaSeperatedIdentifier();
                expect(PL0Lexer::Token::IDENTIFIER, "expected an identifier");
            }
            codeGenerator_endWithSemiColon();
            expect(PL0Lexer::Token::SEMICOLON, "expected a semicolon");
        }

        while (m_lexReturnValue == PL0Lexer::Token::PROCEDURE)
        {
            expect(PL0Lexer::Token::PROCEDURE);
            expect(PL0Lexer::Token::IDENTIFIER, "expected an identifier");
            expect(PL0Lexer::Token::SEMICOLON, "expected a semicolon");

            block();

            expect(PL0Lexer::Token::SEMICOLON, "expected a semicolon");
        }
        
        statement();
    }

    void PL0Parser::statement()
    {
        switch (m_lexReturnValue)
        {
        case PL0Lexer::Token::IDENTIFIER:
            expect(PL0Lexer::Token::IDENTIFIER);
            expect(PL0Lexer::Token::ASSIGN, "expected an assign(:=)");
            expression();
            break;
        case PL0Lexer::Token::CALL:
            expect(PL0Lexer::Token::CALL);
            expect(PL0Lexer::Token::IDENTIFIER, "expected an identifier");
            break;
        case PL0Lexer::Token::BEGIN:
            expect(PL0Lexer::Token::BEGIN);
            statement();
            while (m_lexReturnValue == PL0Lexer::Token::SEMICOLON)
            {
                expect(PL0Lexer::Token::SEMICOLON);
                statement();
            }
            expect(PL0Lexer::Token::END, "expected an end");
            break;
        case PL0Lexer::Token::IF:
            expect(PL0Lexer::Token::IF);
            condition();
            expect(PL0Lexer::Token::THEN, "expected a then");
            statement();
            break;
        case PL0Lexer::Token::WHILE:
            expect(PL0Lexer::Token::WHILE);
            condition();
            expect(PL0Lexer::Token::DO, "expected a do");
            statement();
            break;
        default:
            break;
        }
    }

    void PL0Parser::condition()
    {
        if (m_lexReturnValue == PL0Lexer::Token::ODD)
        {
            expression();
        }
        else
        {
            expression();
            switch (m_lexReturnValue) 
            {
		    case PL0Lexer::Token::EQUAL:
		    case PL0Lexer::Token::HASH:
		    case PL0Lexer::Token::LESSTHAN:
		    case PL0Lexer::Token::GREATERTHAN:
		    	next();
		    	break;
		    default:
                m_error.empty_str();
                m_error.writews("invalid relational operator", m_lexer.getTokenString());
		    	errorOutput();   
            }
            expression();
        }
    }

    void PL0Parser::expression()
    {
        switch (m_lexReturnValue)
        {
        case PL0Lexer::Token::PLUS:
        case PL0Lexer::Token::MINUS:
            next();
            break;
        default:
        break;
        }

        term();

        while (m_lexReturnValue == PL0Lexer::Token::PLUS or m_lexReturnValue == PL0Lexer::Token::MINUS)
        {
            next();
            term();
        }
    }

    void PL0Parser::term()
    {
        factor();
        while (m_lexReturnValue == PL0Lexer::Token::MULTIPLY or m_lexReturnValue == PL0Lexer::Token::DIVIDE)
        {
            next();
            factor();
        }
    }

    void PL0Parser::factor()
    {
        switch (m_lexReturnValue)
        {
        case PL0Lexer::Token::IDENTIFIER:
        case PL0Lexer::Token::NUMBER:
            next();
            break;
        case PL0Lexer::Token::LPAREN:
            expect(PL0Lexer::Token::LPAREN);
            expression();
            expect(PL0Lexer::Token::RPAREN, "expected right paranthesis");
            break;
        default:
            break;
        }
    }

    void PL0Parser::expect(PL0Lexer::Token exceptedMatch, [[maybe_unused]] const char* optionalErrorMessage)
    {
        if (m_lexReturnValue != exceptedMatch)
        {
            m_error.empty_str();
            m_error.writews(optionalErrorMessage, "-> before \"", m_lexer.getTokenString(), "\" in code");
            errorOutput();
        }
        next();
    }

    void PL0Parser::next()
    { m_lexReturnValue = m_lexer.lex(); }

    void PL0Parser::errorOutput()
    {
        System::coutput.write_endl("───────────────────────────────────────────────────────────────────────────────────────────────────");
        System::coutput.writews_endl("[SYNTAX ERROR]", m_error.get_str());
        System::cerror.writewt("");
        System::cerror.writews_endl("Line:", m_lexer.getLine(), "Column:", m_lexer.getColumn());
        System::cerror.write_endl("");
        System::cerror.write_endl("/Compilation exited with an error/");
        System::coutput.write_endl("───────────────────────────────────────────────────────────────────────────────────────────────────");
        std::exit(EXIT_FAILURE);
    }

    void PL0Parser::codeGenerator_startMainFcn()
    {
        m_cppFile.write_endl("#include <cstdint>");
        m_cppFile.write_endl("\n\n\n");
        m_cppFile.write_endl("int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])");
        m_cppFile.write_endl("{");
    }
    void PL0Parser::codeGenerator_endMainFcn()
    {
        m_cppFile.write_endl("");
        m_cppFile.write('}');
    }
    void PL0Parser::codeGenerator_constLLIdentifier()
    {
        m_cppFile.writewt("");
        m_cppFile.write("const std::int64_t ", m_lexer.getTokenString());
    }
    void PL0Parser::codeGenerator_LLIdentifier()
    {
        m_cppFile.writewt("");
        m_cppFile.write("std::int64_t ", m_lexer.getTokenString());
    }
    void PL0Parser::codeGenerator_intializeIdentifier()
    {
        m_cppFile.write('{', m_lexer.getTokenString(), '}');
    }
    void PL0Parser::codeGenerator_commaSeperatedIdentifier()
    {
        m_cppFile.write(", ", m_lexer.getTokenString());
    }
    void PL0Parser::codeGenerator_endWithSemiColon()
    {
        m_cppFile.write_endl(';');
    }
} // namespace Pl0
