#ifndef PL0_PARSER_H
#define PL0_PARSER_H

#pragma once

#include "pl0_lexer.h"
#include "stream/stringoutputstream.h"
#include "stream/fileoutputstream.h"

/*
 * pl0c -- PL/0 compiler.
 *
 * program	    = block "." .
 * block	    = [ "const" ident "=" number { "," ident "=" number } ";" ]
 *		          [ "var" ident { "," ident } ";" ]
 *		          { "procedure" ident ";" block ";" } statement .
 * statement	= [ ident ":=" expression
 *		          | "call" ident
 *		          | "begin" statement { ";" statement } "end"
 *		          | "if" condition "then" statement
 *		          | "while" condition "do" statement ] .
 * condition	= "odd" expression
 *		          | expression ( "=" | "#" | "<" | ">" ) expression .
 * expression	= [ "+" | "-" ] term { ( "+" | "-" ) term } .
 * term		    = factor { ( "*" | "/" ) factor } .
 * factor	    = ident
 *		        | number
*		        | "(" expression ")" .
 */


namespace Pl0
{
    class PL0Parser
    {
    private:
        PL0Lexer::Token m_lexReturnValue{};
        PL0Lexer m_lexer;
        Streams::StringOutputStream<char> m_error{std::ostringstream{}};
        Streams::FileOutputStream<char> m_cppFile{std::ofstream{}};

    public:
        PL0Parser(const std::string& inputFile);
        ~PL0Parser() = default;

        void displayLexedFile();
        void parse();

    private:
        void program();
        void block();
        void statement();
        void condition();
        void expression();
        void term();
        void factor();

        void expect(PL0Lexer::Token exceptedMatch, [[maybe_unused]] const char* optionalErrorMessage = "");
        void next();
        void errorOutput();

        void codeGenerator_startMainFcn();
        void codeGenerator_endMainFcn();
        void codeGenerator_constLLIdentifier();
        void codeGenerator_LLIdentifier();
        void codeGenerator_intializeIdentifier();
        void codeGenerator_commaSeperatedIdentifier();
        void codeGenerator_endWithSemiColon();
    };
} // namespace Pl0

#endif