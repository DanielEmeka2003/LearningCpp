#include <cctype>
#include <algorithm>

#include "pl0_lexer.h"
#include "stream/output.h"
#include "stream/stringinputstream.h"


namespace Pl0
{
    PL0Lexer::PL0Lexer(const std::string& inputFile): m_inputFile{inputFile} {}

    std::uint32_t PL0Lexer::getLine() const
    { return m_line; }

    std::uint32_t PL0Lexer::getColumn() const
    { return m_column; }

    std::uint32_t PL0Lexer::currentFilePosition() const
    { return m_filePosition; }

    const std::string& PL0Lexer::getTokenString() const
    { return m_token; }

    PL0Lexer::Token PL0Lexer::lex()
    {
        bool tryAgain{};
        do
        {
            tryAgain = false;
            try //main try block
            {
                while (std::isspace(m_inputFile.at(m_filePosition)))
                {
                    if (m_inputFile.at(m_filePosition) == '\n')
                    {
                        ++m_line;
                        m_column = {};
                    }
                    ++m_filePosition;
                    ++m_column;
                }            

                if (std::isalpha(m_inputFile.at(m_filePosition)) or m_inputFile.at(m_filePosition) == '_')
                return tokenize_words();
                if (std::isdigit(m_inputFile.at(m_filePosition)) or m_inputFile.at(m_filePosition) == '-')
                return tokenize_numbers();

                switch (m_inputFile.at(m_filePosition))
                {
                case +Token::LCURLY:
                    ignore_comments();
                    ++m_filePosition;
                    ++m_column;
                    tryAgain = true;
                    break;

                case +Token::DOT:
                case +Token::EQUAL:
                case +Token::COMMA:
                case +Token::SEMICOLON:
                case +Token::HASH:
                case +Token::LESSTHAN:
                case +Token::GREATERTHAN:
                case +Token::PLUS:
                case +Token::MINUS:
                case +Token::MULTIPLY:
                case +Token::DIVIDE:
                case +Token::LPAREN:
                case +Token::RPAREN:
                {
                    m_token.clear();
                    m_token.push_back(m_inputFile.at(m_filePosition));
                    ++m_filePosition;
                    ++m_column;
                    return Token{m_token[0]};
                }

                case +Token::ASSIGN:
                    {
                        ++m_filePosition;
                        ++m_column;
                    }
                    if (m_inputFile.at(m_filePosition) != +Token::EQUAL)
                    {
                        m_error.empty_str();
                        m_error.writews_endl("unknown token type:", "[\"", m_inputFile.at(m_filePosition), "\"]");
                        m_error.write("try \" := \" for assigning values to a variable");
                        errorOutput();
                    }
                    else
                    {
                        ++m_filePosition;
                        ++m_column;
                        m_token = ":=";
                    }
                    return Token::ASSIGN;

                default:
                    m_error.empty_str();
                    m_error.writews("unknown token type:", "[\"", m_inputFile.at(m_filePosition), "\"]");
                    errorOutput();
                }
            }
            catch(const std::exception&){} //main catch block
                
        } while (tryAgain);
        
        return Token::MISC;
    }

    void PL0Lexer::ignore_comments()
    {
        try
        {
            while (m_inputFile.at(m_filePosition) != +Token::RCURLY)
            {
                if (m_inputFile.at(m_filePosition) == '\n')
                {
                    ++m_line;
                    m_column = {};
                }
                ++m_filePosition;
                ++m_column;
            }   
        }
        catch(const std::exception&)
        {
            m_error.empty_str();
            m_error.write("unterminated comment");
            errorOutput();
        }
    }

    PL0Lexer::Token PL0Lexer::tokenize_words()
    {
        m_token.clear();
        auto formerFilePos{m_filePosition};

        ++m_filePosition; //move the file position to the next character
        ++m_column;
        try
        {
            while (std::isalnum(m_inputFile.at(m_filePosition)) or m_inputFile.at(m_filePosition) == '_')
            { 
                ++m_filePosition;
                ++m_column;
            }
        }
        catch(const std::exception&){}

        for (size_t i = formerFilePos; i < m_filePosition; i++)
        {
            m_token.push_back(m_inputFile.at(i));      
        }

        if (Token result{matchReservedWords()}; result != Token::MISC)
        { return result; }

        return Token::IDENTIFIER;
    }

    PL0Lexer::Token PL0Lexer::tokenize_numbers()
    {
        m_token.clear();
        auto formerFilePos{m_filePosition};

        ++m_filePosition; //move the file position to the next character
        ++m_column;
        try
        {
            while (std::isdigit(m_inputFile.at(m_filePosition)) or m_inputFile.at(m_filePosition) == '\'')
            { 
                ++m_filePosition;
                ++m_column;
            }
        }
        catch(const std::exception&){}

        for (size_t i = formerFilePos; i < m_filePosition; i++)
        {
            m_token.push_back(m_inputFile.at(i));
        }
        std::ranges::replace(m_token, '\'', '\0'); // remove the number delimiter

        //validate the integral number

        Streams::StringInputStream sis{std::istringstream{}};
        std::int64_t variable{};
        try
        {
            if (m_token.starts_with('0') and !(m_token == "0")) // to prevent misleading octal numbers
            {
                m_token.erase(m_token.begin());
            }
            sis.insert_str(sis.prepare_str_for_insertion(m_token));
            sis.read(variable);
        }
        catch(const std::exception& e)
        {
            m_error.empty_str();
            m_error.writews("invalid integral number:", "[\"", m_token, "\"]");
            errorOutput();
        }
        return Token::NUMBER;
    }

    PL0Lexer::Token PL0Lexer::matchReservedWords()
    {
        if (m_token == "const")
            return Token::CONST;
        else if (m_token == "var")
            return Token::VAR;
        else if (m_token == "procedure")
            return Token::PROCEDURE;
        else if (m_token == "call")
            return Token::CALL;
        else if (m_token == "begin")
            return Token::BEGIN;
        else if (m_token == "end")
            return Token::END;
        else if (m_token == "if")
            return Token::IF;
        else if (m_token == "then")
            return Token::THEN;
        else if (m_token == "while")
            return Token::WHILE;
        else if (m_token == "do")
            return Token::DO;
        else if (m_token == "odd")
            return Token::ODD;

        return Token::MISC;
    }

    void PL0Lexer::errorOutput()
    {
        System::coutput.write_endl("───────────────────────────────────────────────────────────────────────────────────────────────────");
        System::cerror.writews_endl("[TOKEN ERROR]", m_error.get_str());
        System::cerror.writewt("");
        System::cerror.writews_endl("Line:", m_line, "Column:", m_column);
        System::cerror.write_endl("");
        System::cerror.write_endl("/Compilation exited with an error/");
        System::coutput.write_endl("───────────────────────────────────────────────────────────────────────────────────────────────────");
        std::exit(EXIT_FAILURE);
    }
} // namespace Myfcn
