#ifndef PL0_LEXER_H
#define PL0_LEXER_H

#pragma once

#include <utility>
#include <cstdint>
#include <string>

#include "stream/stringoutputstream.h"


namespace Pl0
{
    class PL0Lexer
    {
    public:
    enum class Token: char
    {
        IDENTIFIER = 'I', NUMBER ='N', CONST = 'C', VAR = 'V', PROCEDURE = 'P', CALL = 'c', BEGIN = 'B',  END = 'E', IF	= 'i',  THEN = 'T',
        WHILE = 'W', DO	= 'D', ODD = 'O', DOT	= '.', EQUAL = '=', COMMA = ',', SEMICOLON = ';', ASSIGN = ':', HASH = '#', LESSTHAN = '<',
        GREATERTHAN	= '>', PLUS = '+', MINUS = '-', MULTIPLY = '*', DIVIDE = '/', LPAREN = '(', RPAREN = ')', LCURLY = '{', RCURLY = '}',
        MISC = '\0'
    };

    private:
        std::uint32_t m_line{1};
        std::uint32_t m_column{1};
        std::uint32_t m_filePosition{};
        std::string m_token{};
        std::string m_inputFile{};
        Streams::StringOutputStream<char> m_error{std::ostringstream{}};

    public:
        PL0Lexer(const std::string& inputFile);
        ~PL0Lexer() = default;

        std::uint32_t getLine() const;
        std::uint32_t getColumn() const;
        std::uint32_t currentFilePosition() const;
        const std::string& getTokenString() const;
        Token lex();
        
    private:
        void ignore_comments();
        Token tokenize_words();
        Token tokenize_numbers();
        Token matchReservedWords();
        void errorOutput();
    };

    constexpr char operator+(const PL0Lexer::Token& x)
    { return std::to_underlying(x); }
} // namespace pl0

#endif