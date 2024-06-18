#include <cctype>
#include <algorithm>
#include <iterator>
#include <bitset>
#include <regex>
#include <utility>
#include <map>

#include "ncLexer.h"
#include "stream/output.h"
#include "stream/stringinputstream.h"
#include "stream/fileoutputstream.h"
#include "stream/fileinputstream.h"
#include "boost/multiprecision/cpp_dec_float.hpp"

namespace Nc
{
    Lexer::Lexer(U8string& fileBuffer): m_fileBuffer{ std::move(fileBuffer) }{}

    Lexer::Lexer(U8string&& fileBuffer): m_fileBuffer{ std::move(fileBuffer) }{}

    void Lexer::lexFileBuffer()
    {
        while (lexify() != TokenType::_eot){}

        if (!m_log.isLogEmpty()) errorOutput();
    }
    
    std::vector<Lexer::TokenData>& Lexer::getTokenDataList()
    { return m_tokenDataList; }

    const std::vector<Lexer::TokenData>& Lexer::getTokenDataList() const
    { return m_tokenDataList; }
    
    Lexer::LineNumerToFilePositionMap& Lexer::getLineNumToFilePos()
    { return m_lineNumToFilePos; }
    const Lexer::LineNumerToFilePositionMap& Lexer::getLineNumToFilePos() const
    { return m_lineNumToFilePos; }
    
    U8string& Lexer::getFileBuffer()
    { return m_fileBuffer; }
    const U8string& Lexer::getFileBuffer() const
    { return m_fileBuffer; }

    void Lexer::logBoxStyle(Log::BoxLineStyle s)
    { m_log.changeBoxLineStyle(s); }

    bool acceptableFirstUnicodeCharacter(const U8char& x)
    {
        auto unicode_code_point = x.convertToUnicodeCodePoint();
        
        if
        (
            (unicode_code_point == 0xd7)/*multiplication-sign ×*/ or (unicode_code_point == 0xf7)/*division-sign ÷*/ or

            (unicode_code_point >= 0x2b0 and unicode_code_point <= 0x2ff)/*spacing-modifier-letters*/ or
            (unicode_code_point >= 0x300 and unicode_code_point <= 0x36f)/*combining-diacritical-marks*/ or
            (unicode_code_point >= 0x1ab0 and unicode_code_point <= 0x1aff)/*combining-diacritical-marks-extended*/ or
            (unicode_code_point >= 0x1dc0 and unicode_code_point <= 0x1dff)/*combining-diacritical-marks-supplement*/ or
            (unicode_code_point >= 0x2000 and unicode_code_point <= 0x206f)/*general-punctuation*/ or
            (unicode_code_point >= 0x2070 and unicode_code_point <= 0x209f)/*superscript-and-subscripts*/ or
            (unicode_code_point >= 0x20a0 and unicode_code_point <= 0x20cf)/*currency-symbols*/ or
            (unicode_code_point >= 0x20d0 and unicode_code_point <= 0x20ff)/*combining-diacritical-marks-for-symbols*/ or
            (unicode_code_point >= 0x2190 and unicode_code_point <= 0x21ff)/*arrows*/ or
            (unicode_code_point >= 0x2200 and unicode_code_point <= 0x22ff)/*mathematical-operators*/ or
            (unicode_code_point >= 0x2300 and unicode_code_point <= 0x23ff)/*miscellaneous-technical*/ or
            (unicode_code_point >= 0x2400 and unicode_code_point <= 0x243f)/*control-pictures*/ or
            (unicode_code_point >= 0x2440 and unicode_code_point <= 0x245f)/*optical-character-recognition*/ or
            (unicode_code_point >= 0x2500 and unicode_code_point <= 0x257f)/*box-drawing*/ or
            (unicode_code_point >= 0x2580 and unicode_code_point <= 0x259f)/*block-elements*/ or
            (unicode_code_point >= 0x25a0 and unicode_code_point <= 0x25ff)/*geometric-shapes*/ or
            (unicode_code_point >= 0x27c0 and unicode_code_point <= 0x27ef)/*miscellaneous-mathematical-symbolsA*/ or
            (unicode_code_point >= 0x27f0 and unicode_code_point <= 0x27ff)/*supplemental-arrowsA*/ or
            (unicode_code_point >= 0x2800 and unicode_code_point <= 0x28ff)/*braille-patterns*/ or
            (unicode_code_point >= 0x2900 and unicode_code_point <= 0x297f)/*supplemental-arrowsB*/ or
            (unicode_code_point >= 0x2980 and unicode_code_point <= 0x29ff)/*miscellaneous-mathematical-symbolsB*/ or
            (unicode_code_point >= 0x2a00 and unicode_code_point <= 0x2aff)/*supplemental-mathematical-operators*/ or
            (unicode_code_point >= 0x2b00 and unicode_code_point <= 0x2bff)/*miscellaneous-symbols-and-arrows*/ or
            (unicode_code_point >= 0x2e00 and unicode_code_point <= 0x2e7f)/*supplemental-punctuation*/ or
            (unicode_code_point >= 0xfe20 and unicode_code_point <= 0xfe2f)/*combining-half-marks*/ or
            (unicode_code_point >= 0xff00 and unicode_code_point <= 0xffef)/*half-and-full-width*/ or
            (unicode_code_point >= 0x16fe0 and unicode_code_point <= 0x16fff)/*ideographic-symbols-and-punctuation*/ or
            (unicode_code_point >= 0x1f800 and unicode_code_point <= 0x1f8ff)/*supplemental-arrowsC*/
        )
        return false;
        else
        {
            if (u8_misc::isBasicLatin(unicode_code_point))
            return u8_misc::isBasicLatinAlphabet(unicode_code_point) or x == '_'_u8;
            else
            return true;
        }
        
        std::unreachable();
    }
    
    bool acceptableRestOfUnicodeCharacter(const U8char& x)
    {
        auto unicode_code_point = x.convertToUnicodeCodePoint();
        /*
        * (to add) geometric shapes extended
        * Devoid of the combinging-diacritical-symbols
        */
        if
        (
            (unicode_code_point == 0xd7)/*multiplication-sign ×*/ or (unicode_code_point == 0xf7)/*division-sign ÷*/ or

            (unicode_code_point >= 0x2b0 and unicode_code_point <= 0x2ff)/*spacing-modifier-letters*/ or
            (unicode_code_point >= 0x2000 and unicode_code_point <= 0x206f)/*general-punctuation*/ or
            (unicode_code_point >= 0x2070 and unicode_code_point <= 0x209f)/*superscript-and-subscripts*/ or
            (unicode_code_point >= 0x20a0 and unicode_code_point <= 0x20cf)/*currency-symbols*/ or
            (unicode_code_point >= 0x2190 and unicode_code_point <= 0x21ff)/*arrows*/ or
            (unicode_code_point >= 0x2200 and unicode_code_point <= 0x22ff)/*mathematical-operators*/ or
            (unicode_code_point >= 0x2300 and unicode_code_point <= 0x23ff)/*miscellaneous-technical*/ or
            (unicode_code_point >= 0x2400 and unicode_code_point <= 0x243f)/*control-pictures*/ or
            (unicode_code_point >= 0x2440 and unicode_code_point <= 0x245f)/*optical-character-recognition*/ or
            (unicode_code_point >= 0x2500 and unicode_code_point <= 0x257f)/*box-drawing*/ or
            (unicode_code_point >= 0x2580 and unicode_code_point <= 0x259f)/*block-elements*/ or
            (unicode_code_point >= 0x25a0 and unicode_code_point <= 0x25ff)/*geometric-shapes*/ or
            (unicode_code_point >= 0x27c0 and unicode_code_point <= 0x27ef)/*miscellaneous-mathematical-symbolsA*/ or
            (unicode_code_point >= 0x27f0 and unicode_code_point <= 0x27ff)/*supplemental-arrowsA*/ or
            (unicode_code_point >= 0x2800 and unicode_code_point <= 0x28ff)/*braille-patterns*/ or
            (unicode_code_point >= 0x2900 and unicode_code_point <= 0x297f)/*supplemental-arrowsB*/ or
            (unicode_code_point >= 0x2980 and unicode_code_point <= 0x29ff)/*miscellaneous-mathematical-symbolsB*/ or
            (unicode_code_point >= 0x2a00 and unicode_code_point <= 0x2aff)/*supplemental-mathematical-operators*/ or
            (unicode_code_point >= 0x2b00 and unicode_code_point <= 0x2bff)/*miscellaneous-symbols-and-arrows*/ or
            (unicode_code_point >= 0x2e00 and unicode_code_point <= 0x2e7f)/*supplemental-punctuation*/ or
            (unicode_code_point >= 0xfe20 and unicode_code_point <= 0xfe2f)/*combining-half-marks*/ or
            (unicode_code_point >= 0xff00 and unicode_code_point <= 0xffef)/*half-and-full-width*/ or
            (unicode_code_point >= 0x16fe0 and unicode_code_point <= 0x16fff)/*ideographic-symbols-and-punctuation*/ or
            (unicode_code_point >= 0x1f800 and unicode_code_point <= 0x1f8ff)/*supplemental-arrowsC*/
        )
        return false;
        else
        {
            if (u8_misc::isBasicLatin(unicode_code_point))
            return u8_misc::isBasicLatinAlphabetNumeral(unicode_code_point) or x == '_'_u8;
            else
            return true;
        }
        
        std::unreachable();
    }

    Lexer::TokenType Lexer::lexify()
    {
        auto isStartOfRawIdentifier = [&]()
        {
            if (m_fileBuffer[m_filePosition] == '\\'_u8)
            {
                if (nextFilePosAndColumn(), !m_isEndofFile and (acceptableFirstUnicodeCharacter(m_fileBuffer[m_filePosition]) or m_fileBuffer[m_filePosition] == '"'))
                return prevFilePosAndColumn(), true;
                else
                return prevFilePosAndColumn(), false;
            }
            
            return false;
        };

        if (m_isEndofFile)
        return TokenType::_eot;

        //clear the token string before a lexing operation begins
        m_tokenString.clear();

        while (u8_misc::isBasicLatinWhiteSpace(std::uint32_t(m_fileBuffer[m_filePosition])))
        {
            if (m_fileBuffer[m_filePosition] == '\n'_u8)
            newLineFound();

            nextFilePosAndColumn();

            if (m_isEndofFile)
            return TokenType::_eot;
        }
        
        if (acceptableFirstUnicodeCharacter(m_fileBuffer[m_filePosition]) or isStartOfRawIdentifier())
        return tokenizeWords();
        else if (m_fileBuffer[m_filePosition] == '\''_u8)
        return tokenizeCharLiterals();
        else if (m_fileBuffer[m_filePosition] == '"'_u8)
        return tokenizeStrLiterals();
        else if (u8_misc::isBasicLatinDigit(std::uint32_t(m_fileBuffer[m_filePosition])))
        return tokenizeNumbers();
        else
        {
            if (singleLineDiscardComment() == TokenType::_comment)
            return TokenType::_comment;
            else if (multiLineDiscardComment() == TokenType::_comment)
            return TokenType::_comment;
            else if (tokenizeSymbols() == TokenType::symbol)
            return TokenType::symbol;
            else if (singleLineDocComment() == TokenType::_comment)
            return TokenType::_comment;
            else if (multiLineDocComment() == TokenType::_comment)
            return TokenType::_comment;
            else
            {
                if (m_isEndofFile)
                return TokenType::_eot;
                
                //keep the unrecognized token for error purposes
                m_tokenString.push_back(m_fileBuffer[m_filePosition]);
                nextFilePosAndColumn(); //move the file position past the symbol

                m_log.write("Unrecognized token \033[48;2;10;6;26;31;1m", m_tokenString, "\033[0m"), start_log();
                spaceLog();
                
                m_log.write("   Unicode & utf8 code point: \033[1m", m_tokenString.front().convertToUnicodeCodePoint(), "\033[0m & \033[1m", m_tokenString.front().convertToUtf8CodePoint(), "\033[0m");
                additionalLog();

                return TokenType::_miscellany;
            }
        }
    }

    void Lexer::newLineFound()
    {
        ++m_line;
        m_absoluteColumn = {};

        m_lineNumToFilePos[m_line] = m_filePosition + 1;
    }

    void Lexer::nextFilePosAndColumn(std::uint32_t number)
    {
        if ((m_filePosition += number) >= m_fileBuffer.size())
        {
            m_filePosition = m_fileBuffer.size() - 1, m_isEndofFile = true;

            if (m_AbsoluteColumnStaticBoolean)
            m_AbsoluteColumnStaticBoolean = false, m_absoluteColumn += 1;
        }
        else m_absoluteColumn += number;
    }

    void Lexer::prevFilePosAndColumn(std::uint32_t number)
    {
        if (int((m_absoluteColumn -= number, m_filePosition -= number)) < 0)
        m_filePosition = 0;
    }
    
    void Lexer::recordRelativeColumn()
    { m_relativeColumn = m_absoluteColumn; }

    void Lexer::addToTokenDataList(std::optional<std::uint32_t> optLine, std::uint32_t line, bool isReserved, bool isIdentifier, U8string literalType, bool isSymbol)
    {
        if (m_isDeadZone)
        return;
        
        //add the token to the list
        m_tokenDataList.push_back(TokenData{
            .token{ std::move(m_tokenString) },
            .optLine{ optLine },
            .line{ line },
            .relativeColumn{ m_relativeColumn },
            .absoluteColumn{ m_absoluteColumn },
            .isReserved{ isReserved }, 
            .isIdentifier{ isIdentifier },
            .literalType{ std::move(literalType) },
            .isSymbol{ isSymbol }
        });
    }

    Lexer::TokenType Lexer::singleLineDiscardComment()
    {
        if (m_fileBuffer[m_filePosition] == '/'_u8)
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == '/'_u8)
            {
                while (true)
                {
                    if (nextFilePosAndColumn(), m_fileBuffer[m_filePosition] == '\n'_u8)
                    {
                        newLineFound(), nextFilePosAndColumn();
                        return TokenType::_comment;
                    }

                    if (m_isEndofFile) return TokenType::_eot;
                }
            }
            else prevFilePosAndColumn();
        }
        return TokenType::_miscellany;
    }

    Lexer::TokenType Lexer::singleLineDocComment()
    {
        if (m_fileBuffer[m_filePosition] == '`'_u8)
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == '`'_u8)
            {
                while (true)
                {
                    if (nextFilePosAndColumn(), m_fileBuffer[m_filePosition] == '\n'_u8)
                    {
                        newLineFound(), nextFilePosAndColumn();
                        return TokenType::_comment;
                    }

                    if (m_isEndofFile) return TokenType::_eot;
                }
            }
            else prevFilePosAndColumn();
        }
        return TokenType::_miscellany;
    }

    Lexer::TokenType Lexer::multiLineDiscardComment()
    {
        if (m_fileBuffer[m_filePosition] == '/'_u8)
        {
            if (recordRelativeColumn(), nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == '*'_u8)
            {   
                auto savedLine = m_line; //for error purposes
                while (true)
                {
                    if (nextFilePosAndColumn(), multiLineDocComment()/*to support of multi-line nesting*/, m_isEndofFile)
                    {
                        m_log.write("Unterminated mutiline comment, expected this \033[48;2;10;6;26;32;1m*/\033[0m before EOF(end of file)");
                        savedLine == m_line? start_log() : start_multiLineLog(savedLine, m_relativeColumn);
                        
                        return TokenType::_eot;
                    }
                    else if (m_fileBuffer[m_filePosition] == '*'_u8 and (nextFilePosAndColumn(), /*!m_isEnofFile and*/ m_fileBuffer[m_filePosition] == '/'_u8))
                    {
                        nextFilePosAndColumn(); //for moving past symbol[ ` ]
                        return TokenType::_comment;
                    }
                    
                    if (m_fileBuffer[m_filePosition] == '\n'_u8)//it is directly under the second conditionally for a reason
                    newLineFound();
                }
            }
            else prevFilePosAndColumn();
        }
        return TokenType::_miscellany;
    }

    Lexer::TokenType Lexer::multiLineDocComment()
    {
        if (m_fileBuffer[m_filePosition] == '`'_u8)
        {
            if (recordRelativeColumn(), nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == '{'_u8)
            {   
                auto savedLine = m_line; //for error purposes
                while (true)
                {
                    if (nextFilePosAndColumn(), multiLineDocComment()/*to support of multi-line nesting*/, m_isEndofFile)
                    {
                        m_log.write("Unterminated mutiline comment, expected this \033[48;2;10;6;26;32;1m}`\033[0m before EOF(end of file)");
                        savedLine == m_line? start_log() : start_multiLineLog(savedLine, m_relativeColumn);
                        
                        return TokenType::_eot;
                    }
                    else if (m_fileBuffer[m_filePosition] == '}'_u8 and (nextFilePosAndColumn(), /*!m_isEnofFile and*/ m_fileBuffer[m_filePosition] == '`'_u8))
                    {
                        nextFilePosAndColumn(); //for moving past symbol[ ` ]
                        return TokenType::_comment;
                    }
                    
                    if (m_fileBuffer[m_filePosition] == '\n'_u8)//it is directly under the second conditionally for a reason
                    newLineFound();
                }
            }
            else prevFilePosAndColumn();
        }
        return TokenType::_miscellany;
    }


    Lexer::TokenType Lexer::tokenizeWords()
    {
        auto formerFilePos{m_filePosition};
        recordRelativeColumn(); //do not move from here

        while(nextFilePosAndColumn(), !m_isEndofFile and acceptableRestOfUnicodeCharacter(m_fileBuffer[m_filePosition]))
        {}
        
        //extract the token to m_tokenString
        {
            std::uint32_t count{};
            for (size_t i = formerFilePos; m_isEndofFile? i <= m_filePosition : i < m_filePosition; ++i)
            ++count;

            m_tokenString.assign(m_fileBuffer, formerFilePos, count);
        }

        if (!m_isEndofFile)
        {
            U8string literalType{};

            if (m_fileBuffer[m_filePosition] == '\''_u8)
            {
                literalType = m_tokenString;

                return tokenizeCharLiterals(&literalType);
            }
            else if (m_fileBuffer[m_filePosition] == '"'_u8)
            {
                literalType = m_tokenString;
                auto result = tokenizeStrLiterals(&literalType);

                if (literalType == U8string{'\\'_u8})
                {
                    if (!m_isDeadZone)
                    {
                        auto& currently_added_tokenData = m_tokenDataList.back();
                        currently_added_tokenData.isIdentifier = true, currently_added_tokenData.literalType.clear();
                    }
                }
                
                return result;
            }
        }
        
        if (!m_tokenString.starts_with('\\'_u8))
        {
            if (TokenType result{reservedWords()}; result != TokenType::_miscellany)
            return result;
        }

        addToTokenDataList(std::optional<std::uint32_t>{}, m_line, false, true);

        return TokenType::identifier;
    }
    
    Lexer::TokenType Lexer::reservedWords()
    {
        if
        (
            m_tokenString == rGive or m_tokenString == rGuess or m_tokenString == rBreak or m_tokenString == rContinue or m_tokenString == rIf or
            m_tokenString == rElse or m_tokenString == rDo or m_tokenString == rWhile or m_tokenString == rFor or m_tokenString == rAnd or
            m_tokenString == rOr or m_tokenString == rNot or m_tokenString == rXor or m_tokenString == rAnd or m_tokenString == rOr or
            m_tokenString == rNot or m_tokenString == rXor or m_tokenString == rOdd or m_tokenString == rOutput or m_tokenString == rInput or
            m_tokenString == rDefault or m_tokenString == rEnum or m_tokenString == rNamespace or m_tokenString == rTypeof or
            m_tokenString == rMatch or m_tokenString == rPanic or m_tokenString == rCatch or m_tokenString == rRaise or m_tokenString == rMv or
            m_tokenString == rCp or m_tokenString == rImut_lref or m_tokenString == rMut_lref or
            m_tokenString == rAlias or m_tokenString == rR_l or m_tokenString == rTcast or
            m_tokenString == rBcast or m_tokenString == rNand or m_tokenString == rNor or m_tokenString == rNxor or m_tokenString == rAnd_eq or
            m_tokenString == rOr_eq or m_tokenString == rXor_eq or m_tokenString == rNand_eq or m_tokenString == rNor_eq or
            m_tokenString == rNxor_eq or m_tokenString == rNamed_pt or m_tokenString == rNamed_st or m_tokenString == rUnamed_pt or
            m_tokenString == rUnamed_st or m_tokenString == rCase or m_tokenString == rIter or m_tokenString == rIter_no_end or
            m_tokenString == rNullptr or m_tokenString == rNone or m_tokenString == rType or m_tokenString == rFromType or
            m_tokenString == rVarg_start or m_tokenString == rVarg_end or m_tokenString == rFn
        )
        {
            addToTokenDataList(std::optional<std::uint32_t>{}, m_line, true);
            return TokenType::reserved;
        }
        else if (m_tokenString == rTrue or m_tokenString == rFalse)
        {
            addToTokenDataList(std::optional<std::uint32_t>{}, m_line, false, false, lBool, false);
            return TokenType::literal;
        }

        return TokenType::_miscellany;
    }

    
    Lexer::TokenType Lexer::tokenizeNumbers()
    {
        auto formerFilePos{m_filePosition}; //maintain intial index
        recordRelativeColumn(); //do not move

        while (nextFilePosAndColumn(), !m_isEndofFile and (u8_misc::isBasicLatinAlphabetNumeral(std::uint32_t(m_fileBuffer[m_filePosition])) or m_fileBuffer[m_filePosition] == '\''_u8 or m_fileBuffer[m_filePosition] == '.'_u8))
        {}
        
        //extract the token to m_tokenString
        {
            std::uint32_t count{};
            for (size_t i = formerFilePos; m_isEndofFile? i <= m_filePosition : i < m_filePosition; ++i)
            ++count;
            
            m_tokenString.assign(m_fileBuffer, formerFilePos, count);
        }
        
        U8string baseAndLiteralType{};
        std::optional<TokenData> optionalTokenData{};
        U8string trustedTokenString{std::move(m_tokenString)};
        std::uint32_t trustedAbsoluteColumn{};//it is trusted because it won't be modified outside this function by the likes of function tokenizeSymbols
        std::uint32_t trustedRelativeColumn{m_relativeColumn};//same as trustedAbsoluteColumn
        bool extract_exponent_once = false;
        
        m_tokenString.clear();
        while (true)
        {
            if (!extract_exponent_once and m_fileBuffer[m_filePosition] == '@'_u8) //test this
            {
                if (nextFilePosAndColumn(), m_fileBuffer[m_filePosition] == '+'_u8)
                baseAndLiteralType.append({'@'_u8, '+'_u8}), nextFilePosAndColumn();
                else if (m_fileBuffer[m_filePosition] == '-'_u8)
                baseAndLiteralType.append({'@'_u8, '-'_u8}), nextFilePosAndColumn();
                else
                baseAndLiteralType.push_back('@'_u8);

                extract_exponent_once = true;
            }

            trustedAbsoluteColumn = m_absoluteColumn;
            if (m_isEndofFile or u8_misc::isBasicLatinWhiteSpace(std::uint32_t(m_fileBuffer[m_filePosition]))) //the position of this loop terminating code is strategic, do not move
            break;
            
            if (auto result = (trustedAbsoluteColumn = m_absoluteColumn, tokenizeSymbols()); result == TokenType::symbol)
            {
                if (!m_isDeadZone) //if the deadZone flag is not set, remove the parsed symbol from the list, because we haven't added the possible number token to the list
                optionalTokenData.emplace(std::move(m_tokenDataList.back())), m_tokenDataList.pop_back();

                break;
            }

            baseAndLiteralType.push_back(m_fileBuffer[m_filePosition]), nextFilePosAndColumn();
        }

        m_tokenString = std::move(trustedTokenString), m_relativeColumn = trustedRelativeColumn, std::swap(trustedAbsoluteColumn, m_absoluteColumn);
        
        auto [base, literalType] = [&baseAndLiteralType]
        {
            if (auto result = std::ranges::find(baseAndLiteralType, '`'_u8); result != baseAndLiteralType.end())
            return std::pair{U8string{baseAndLiteralType.begin(), result}, U8string{result+1, baseAndLiteralType.end()}};
            else
            return std::pair{baseAndLiteralType, U8string{}};

            return std::pair{U8string{}, U8string{}};
        }();

        if (m_tokenString.contains('.'_u8) or base.contains('@'_u8))
        {
            if (validateRealNumberLiteral(base, literalType) == TokenType::literal)
            m_tokenString += baseAndLiteralType;
        }
        else
        {
            if (validateIntNumberLiteral(base, literalType) == TokenType::literal)
            m_tokenString += baseAndLiteralType;
        }
        
        addToTokenDataList(std::optional<std::uint32_t>{}, m_line, false, false, literalType, false);
        
        std::swap(trustedAbsoluteColumn, m_absoluteColumn); //perform a reswap, to get the absoluteColumn right back on track

        if (optionalTokenData.has_value())
        m_tokenDataList.push_back(optionalTokenData.value());

        return TokenType::literal;
    }

    Lexer::TokenType Lexer::validateIntNumberLiteral(const U8string& base, U8string& literalType)
    {
        std::uint32_t integer_literal_size = m_tokenString.size();

        if (m_tokenString.contains('\''_u8))
        {
            if (m_tokenString.ends_with('\''_u8))
            {
                auto absoluteColumn = m_relativeColumn + integer_literal_size;
                m_log.write("The digit seperator \033[48;2;10;6;26;33;1m'\033[0m should not end a number literal"), start_log(absoluteColumn - 1, absoluteColumn, m_line);
            }
            
            // remove the number seperator
            auto newEnd = std::remove(m_tokenString.begin(), m_tokenString.end(), '\'');
            m_tokenString.erase(newEnd, m_tokenString.end());
        }

        std::string baseDigitsAsStr{};
        std::uint8_t baseDigitsAsInt{};
        auto ascii_integer_literal = u8_misc::convertU8StrToAsciiStr(m_tokenString);

        auto isUnicodeBaseValid = [&base]
        {
            return std::ranges::all_of(base, [](const U8char& x){
                auto unicode_cp = x.convertToUnicodeCodePoint();
                return unicode_cp >= 0x2080 and unicode_cp <= 0x2089;
            }
            );
        };
        
        auto setBaseDigitsAsStr = [&]
        {
            if (base.empty())
            baseDigitsAsStr = "10";

            std::map<U8char, char> pseudoBaseMap
            {
                {"₀"_u8, '0'}, {"₁"_u8, '1'}, {"₂"_u8, '2'}, {"₃"_u8, '3'}, {"₄"_u8, '4'}, {"₅"_u8, '5'}, {"₆"_u8, '6'}, {"₇"_u8, '7'}, {"₈"_u8, '8'}, {"₉"_u8, '9'}
            };

            for (auto &&i : base)
            baseDigitsAsStr.push_back(pseudoBaseMap[i]);

            nc_misc::trim_integer_leading_zeros(baseDigitsAsStr);
        };

        auto isBaseWithinRange = [&]
        { return nc_misc::integer_gt_eq(baseDigitsAsStr, "2") and nc_misc::integer_lt_eq(baseDigitsAsStr, "36"); };

        auto areBaseDigitsValid = [&]
        { return nc_misc::areBaseDigitsValid(ascii_integer_literal, baseDigitsAsInt); };

        auto integerLiteralToBase10 = [&]
        {
            if (baseDigitsAsStr != "10")
            return u8_misc::convertAsciiStrToU8Str(nc_misc::convertIntegerBaseNToBase10(ascii_integer_literal, baseDigitsAsInt));
            
            return m_tokenString;
        };

        auto integerLiteralToBaseN = [&](const U8string& str)
        {
            if (baseDigitsAsStr == "10")
            return m_tokenString;

            return
            u8_misc::convertAsciiStrToU8Str(nc_misc::convertIntegerBase10ToBaseN(u8_misc::convertU8StrToAsciiStr(str), baseDigitsAsInt));
        };

        auto is_lessThanEqualTo = [&](const U8string& literal, const U8string& integerMax)
        { return nc_misc::integer_lt_eq(u8_misc::convertU8StrToAsciiStr(literal), u8_misc::convertU8StrToAsciiStr(integerMax)); };

        // [&literalType]{ for (auto &&i : literalType) i = u8_misc::basicLatinToLowerCase(i); }();

        if (isUnicodeBaseValid())
        {
            setBaseDigitsAsStr();

            if (!isBaseWithinRange())
            {
                m_log.write("Integer number literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m is tagged with a base that is not supported in nc");
                
                auto relativeColumn = m_relativeColumn + integer_literal_size;
                start_log(relativeColumn, relativeColumn + base.size(), m_line), spaceLog();
                m_log.write("   Only bases (2 - 36) are supported in nc. Unicode base makers[\033[1m₀ ₁ ₂ ₃ ₄ ₅ ₆ ₇ ₈ ₉\033[0m]"), additionalLog();
                return TokenType::_miscellany;
            }
            else
            baseDigitsAsInt = std::uint8_t(std::stoi(baseDigitsAsStr));

            if (!areBaseDigitsValid())
            {
                m_log.write("Integer number literal contains invalid base(\033[48;2;10;6;26;33;1m", baseDigitsAsStr, "\033[0m) digit(s)");
                start_log(m_relativeColumn, m_relativeColumn + integer_literal_size, m_line), spaceLog();

                m_log.write("   Only digits \033[48;2;10;6;26;32;1m");
                
                for (auto i = 0, j = 0; i < baseDigitsAsInt; ++i)
                {
                    if (i >= 10)
                    m_log.write(char('A' + j), '(', char('a' + j), ')'), ++j;
                    else
                    m_log.write(i);

                    if (i != baseDigitsAsInt - 1)
                    m_log.write(',', ' ');
                }
                m_log.write("\033[0m are allowed in base(\033[48;2;10;6;26;33;1m", baseDigitsAsStr, "\033[0m)"), additionalLog();
                return TokenType::_miscellany;
            }
        }
        else
        {
            m_log.write("Integer number literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m is tagged with a junk base");
            
            auto relativeColumn = m_relativeColumn + integer_literal_size;
            start_log(relativeColumn, relativeColumn + base.size(), m_line), spaceLog();
            m_log.write("   Try any valid combination of these base markers[\033[1m₀ ₁ ₂ ₃ ₄ ₅ ₆ ₇ ₈ ₉\033[0m] instead. The valid combinations being from \033[1m₂\033[0m to \033[1m₃₆\033[0m");
            additionalLog();
            return TokenType::_miscellany;
        }
        
        auto literalToBase10 = integerLiteralToBase10();

        if (!is_lessThanEqualTo(literalToBase10, uint1024Max))
        {
            m_log.write("Integer number literal is greater than the maximum integer value in nc");
            start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

            if (baseDigitsAsStr == "10")
            m_log.write("   Nc maximum integer value: \033[48;2;10;6;26;32;1m(2¹⁰²⁴ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint1024Max, "₁₀\033[0m"), additionalLog();
            else
            {
                m_log.write("   Nc maximum integer value: \033[48;2;10;6;26;32;1m(2¹⁰²⁴ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint1024Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(uint1024Max), base, "\033[0m");
                additionalLog();
                m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                additionalLog();
            }

            return TokenType::_miscellany;
        }

        if (literalType.empty())
        {
            if (is_lessThanEqualTo(literalToBase10, int32Max))
            literalType = lt_i32;
            else if (is_lessThanEqualTo(literalToBase10, uint32Max))
            literalType = lt_ui32;
            else if (is_lessThanEqualTo(literalToBase10, int64Max))
            literalType = lt_i64;
            else if (is_lessThanEqualTo(literalToBase10, uint64Max))
            literalType = lt_ui64;
            else if (is_lessThanEqualTo(literalToBase10, int128Max))
            literalType = lt_i128;
            else if (is_lessThanEqualTo(literalToBase10, uint128Max))
            literalType = lt_ui128;
            else if (is_lessThanEqualTo(literalToBase10, int256Max))
            literalType = lt_i256;
            else if (is_lessThanEqualTo(literalToBase10, uint256Max))
            literalType = lt_ui256;
            else if (is_lessThanEqualTo(literalToBase10, int512Max))
            literalType = lt_i512;
            else if (is_lessThanEqualTo(literalToBase10, uint512Max))
            literalType = lt_ui512;
            else if (is_lessThanEqualTo(literalToBase10, int1024Max))
            literalType = lt_i1024;
            else if (is_lessThanEqualTo(literalToBase10, uint1024Max))
            literalType = lt_ui1024;
        }
        else
        {
            if (literalType == lt_i8)
            {
                if (!is_lessThanEqualTo(literalToBase10, int8Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 8-bit signed integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   8-bit signed integer maximum positive value: \033[48;2;10;6;26;32;1m(2⁸⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", int8Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   8-bit signed integer maximum positive value: \003[40;32;1m(2⁸⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", int8Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(int8Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_i16)
            {
                if (!is_lessThanEqualTo(literalToBase10, int16Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 16-bit signed integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   16-bit signed integer maximum positive value: \033[48;2;10;6;26;32;1m(2¹⁶⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", int16Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   16-bit signed integer maximum positive value: \003[40;32;1m(2¹⁶⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", int16Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(int16Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_i32)
            {
                if (!is_lessThanEqualTo(literalToBase10, int32Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 32-bit signed integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   32-bit signed integer maximum positive value: \033[48;2;10;6;26;32;1m(2³²⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", int32Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   32-bit signed integer maximum positive value: \003[40;32;1m(2³²⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", int32Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(int32Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_i64)
            {
                if (!is_lessThanEqualTo(literalToBase10, int64Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 64-bit signed integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   64-bit signed integer maximum positive value: \033[48;2;10;6;26;32;1m(2⁶⁴⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", int64Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   64-bit signed integer maximum positive value: \033[48;2;10;6;26;32;1m(2⁶⁴⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", int64Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(int64Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_i128)
            {
                if (!is_lessThanEqualTo(literalToBase10, int128Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 128-bit signed integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   128-bit signed integer maximum positive value: \033[48;2;10;6;26;32;1m(2¹²⁸⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", int128Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   128-bit signed integer maximum positive value: \033[48;2;10;6;26;32;1m(2¹²⁸⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", int128Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(int128Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_i256)
            {
                if (!is_lessThanEqualTo(literalToBase10, int256Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 256-bit signed integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   256-bit signed integer maximum positive value: \033[48;2;10;6;26;32;1m(2²⁵⁶⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", int256Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   256-bit signed integer maximum positive value: \033[48;2;10;6;26;32;1m(2²⁵⁶⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", int256Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(int256Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_i512)
            {
                if (!is_lessThanEqualTo(literalToBase10, int512Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 512-bit signed integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   512-bit signed integer maximum positive value: \033[48;2;10;6;26;32;1m(2⁵¹²⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", int512Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   512-bit signed integer maximum positive value: \033[48;2;10;6;26;32;1m(2⁵¹²⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", int512Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(int512Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_i1024)
            {
                if (!is_lessThanEqualTo(literalToBase10, int1024Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 1024-bit signed integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   1024-bit signed integer maximum positive value: \033[48;2;10;6;26;;32;1m(2¹⁰²⁴⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;;32;1m", int1024Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   1024-bit signed integer maximum positive value: \033[48;2;10;6;26;;32;1m(2¹⁰²⁴⁻¹ - 1)₁₀\033[0m or \033[48;2;10;6;26;;32;1m", int1024Max, "₁₀\033[0m or \033[48;2;10;6;26;;32;1m", integerLiteralToBaseN(int1024Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_ui8)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint8Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 8-bit unsigned integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   8-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2⁸ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint8Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   8-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2⁸ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint8Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(uint8Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_ui16)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint16Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 16-bit unsigned integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   16-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2¹⁶ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint16Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   16-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2¹⁶ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint16Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(uint16Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_ui32)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint32Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 32-bit unsigned integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   32-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2³² - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint32Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   32-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2³² - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint32Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(uint32Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_ui64)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint64Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 64-bit unsigned integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   64-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2⁶⁴ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint64Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   64-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2⁶⁴ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint64Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(uint64Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_ui128)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint128Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 128-bit unsigned integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   128-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2¹²⁸ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint128Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   128-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2¹²⁸ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint128Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(uint128Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_ui256)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint256Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 256-bit unsigned integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   256-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2²⁵⁶ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint256Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   256-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2²⁵⁶ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint256Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(uint256Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_ui512)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint512Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 512-bit unsigned integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   512-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2⁵¹² - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint512Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   512-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2⁵¹² - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint512Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(uint512Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_ui1024)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint1024Max))
                {
                    m_log.write("Integer number literal is greater than the maximum positive value of an 1024-bit unsigned integer");
                    start_log(m_relativeColumn, m_relativeColumn + integer_literal_size + base.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    m_log.write("   1024-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2¹⁰²⁴ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint1024Max, "₁₀\033[0m"), additionalLog();
                    else
                    {
                        m_log.write("   1024-bit unsigned integer maximum positive value: \033[48;2;10;6;26;32;1m(2¹⁰²⁴ - 1)₁₀\033[0m or \033[48;2;10;6;26;32;1m", uint1024Max, "₁₀\033[0m or \033[48;2;10;6;26;32;1m", integerLiteralToBaseN(uint1024Max), base, "\033[0m");
                        additionalLog();
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the integer literal by the way: \033[48;2;10;6;26;31;1m", literalToBase10, "₁₀\033[0m");
                        additionalLog();
                    }
                }
            }
            else
            {
                if (literalType == lt_b16 or literalType == lt_b32 or literalType == lt_b128 or literalType == lt_d32 or literalType == lt_d64 or literalType == lt_d128 or literalType == lt_d256 or literalType == lt_d512)
                {
                    m_log.write("Integer number literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m is tagged with a real number literal-type instead of an integer literal-type");
                    
                    auto relativeColumn = m_relativeColumn + integer_literal_size + base.size() + 1/*plsu[`]*/;
                    start_log(relativeColumn, relativeColumn + literalType.size(), m_line), spaceLog();

                    m_log.write("   Try the below instead:"), additionalLog();
                    m_log.write("   ● \033[48;2;10;6;26;33;1mi8, i16, i32, i64, i128, i256, i512 or i1024\033[0m for signed integers"), additionalLog();
                    m_log.write("   ● \033[48;2;10;6;26;33;1mui8, ui16, ui32, ui64, ui128, ui256, ui512 or ui1024\033[0m for unsigned integers"), additionalLog();
                    m_log.write("   Note: they are all case sensitive"), additionalLog();
                }
                else
                {
                    bool isLiteralTypeNameValidUnicodeCharacter{};

                    for (size_t i = 0, n = literalType.size(); i < n; i++)
                    {
                        i == 0?
                        isLiteralTypeNameValidUnicodeCharacter = acceptableFirstUnicodeCharacter(literalType[i])
                        :
                        isLiteralTypeNameValidUnicodeCharacter = acceptableRestOfUnicodeCharacter(literalType[i]);
                        
                        if (!isLiteralTypeNameValidUnicodeCharacter) break;
                    }

                    if (!isLiteralTypeNameValidUnicodeCharacter)
                    {
                        m_log.write("Integer number literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m is tagged with a literalType that contains invalid nc unicode characters");
                        
                        auto relativeColumn = m_relativeColumn + integer_literal_size + base.size() + 1/*plsu[`]*/;
                        start_log(relativeColumn, relativeColumn + literalType.size(), m_line);
                    }
                }
            }
        }

        return TokenType::literal;
    }
    
    Lexer::TokenType Lexer::validateRealNumberLiteral(const U8string& base_exp, U8string& literalType)
    {
        std::uint32_t real_literal_size = m_tokenString.size();

        if (m_tokenString.contains('\''_u8))
        {
            if (m_tokenString.ends_with('\''_u8))
            {
                auto absoluteColumn = m_relativeColumn + real_literal_size;
                m_log.write("The digit seperator \033[48;2;10;6;26;33;1m'\033[0m should not end a number literal"), start_log(absoluteColumn - 1, absoluteColumn, m_line);
            }
            
            // remove the number seperator
            auto newEnd = std::remove(m_tokenString.begin(), m_tokenString.end(), '\'');
            m_tokenString.erase(newEnd, m_tokenString.end());
        }

        if (m_tokenString.contains('.'_u8))
        {
            if (std::ranges::count(m_tokenString, '.'_u8) != 1)
            {
                m_log.writews("A real-number literal should not contain multiple radix seperators"), start_log();
                spaceLog();
                m_log.write("   Remove them!"), additionalLog();
                return TokenType::_miscellany;
            }
        }

        U8string base{}, exponent{};
        bool isExponentNegative{};

        if (auto exponent_indicator_it = std::ranges::find(base_exp, '@'_u8); exponent_indicator_it != base_exp.end())
        {
            exponent = {exponent_indicator_it + 1, base_exp.end()};

            //check if the exponent has a sign
            if (exponent.starts_with('+'_u8))
            {
                exponent.erase(0, 1);

                if(exponent.empty()) //log an error when the exponent indicator is specified but no exponent is found
                {
                    m_log.write("In real number literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m, the exponent indicator(@) was found with no exponent");
                    
                    auto absoluteColumn = m_relativeColumn + real_literal_size + base_exp.size();
                    start_log(absoluteColumn - 2, absoluteColumn, m_line), spaceLog();
                    m_log.write("   At least one exponent digit should be after symbol \033[48;2;10;6;26;33;1m+\033[0m"), additionalLog();
                }
            }
            else if (exponent.starts_with('-'_u8))
            {
                exponent.erase(0, 1), isExponentNegative = true;

                if(exponent.empty()) //log an error when the exponent indicator is specified but no exponent is found
                {
                    m_log.write("In real number literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m, the exponent indicator(@) was found with no exponent");
                    
                    auto absoluteColumn = m_relativeColumn + real_literal_size + base_exp.size();
                    start_log(absoluteColumn - 2, absoluteColumn, m_line), spaceLog();
                    m_log.write("   At least one exponent digit should be after symbol \033[48;2;10;6;26;33;1m-\033[0m"), additionalLog();
                }
            }
            else
            {
                if(exponent.empty()) //log an error when the exponent indicator is specified but no exponent is found
                {
                    m_log.write("In real number literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m, the exponent indicator(@) was found with no exponent");
                    
                    auto absoluteColumn = m_relativeColumn + real_literal_size + base_exp.size();
                    start_log(absoluteColumn - 2, absoluteColumn, m_line);
                }
            }
            
            if (exponent_indicator_it != base_exp.begin())//if the exponent_indicator is not the first element in base_exp, also the same as [base_exp.starts_with('@'_u8)]
            base = U8string{base_exp.begin(), exponent_indicator_it};
        }
        else
        base = base_exp;
        
        //the code above invloves extracting the exponent from the base, that's why the first parameter is called [base_exp]

        std::string baseDigitsAsStr{};
        std::uint8_t baseDigitsAsInt{};
        auto ascii_real_literal = u8_misc::convertU8StrToAsciiStr(m_tokenString);
        auto ascii_exponent = u8_misc::convertU8StrToAsciiStr(exponent);

        auto isUnicodeBaseValid = [&base]
        {
            return std::ranges::all_of(base, [](U8char& x){
                auto unicode_cp = x.convertToUnicodeCodePoint();
                return unicode_cp >= 0x2080 and unicode_cp <= 0x2089;
            }
            );
        };
        
        auto setBaseDigitsAsStr = [&]
        {
            if (base.empty())
            baseDigitsAsStr = "10";

            std::map<U8char, char> pseudoBaseMap
            {
                {"₀"_u8, '0'}, {"₁"_u8, '1'}, {"₂"_u8, '2'}, {"₃"_u8, '3'}, {"₄"_u8, '4'}, {"₅"_u8, '5'}, {"₆"_u8, '6'}, {"₇"_u8, '7'}, {"₈"_u8, '8'}, {"₉"_u8, '9'}
            };

            for (auto &&i : base)
            baseDigitsAsStr.push_back(pseudoBaseMap[i]);

            nc_misc::trim_integer_leading_zeros(baseDigitsAsStr);
        };

        auto isBaseWithinRange = [&baseDigitsAsStr]
        { return nc_misc::integer_gt_eq(baseDigitsAsStr, "2") and nc_misc::integer_lt_eq(baseDigitsAsStr, "36"); };

        auto areRealBaseDigitsValid = [&]
        {
            if (auto found = std::ranges::find(m_tokenString, '.'_u8); found != m_tokenString.end())
            {
                auto real_part = U8string_view{found + 1, m_tokenString.end()};
                return
                nc_misc::areBaseDigitsValid(u8_misc::convertU8StrToAsciiStr(U8string_view{m_tokenString.begin(), found}), baseDigitsAsInt)
                and
                real_part.empty()? true : nc_misc::areBaseDigitsValid(u8_misc::convertU8StrToAsciiStr(real_part), baseDigitsAsInt);
            }
            //else
            return nc_misc::areBaseDigitsValid(ascii_real_literal, baseDigitsAsInt);
        };

        auto is_within_range = [&](const std::string& literal, U8string fp_min, U8string fp_max)
        {
            return nc_misc::real_eq(literal, "0@0") or (nc_misc::real_gt_eq(literal, u8_misc::convertU8StrToAsciiStr(fp_min)) and nc_misc::real_lt_eq(literal, u8_misc::convertU8StrToAsciiStr(fp_max)));
        };

        auto convertFpToBaseN = [&](const U8string& str)
        { return nc_misc::convertRealBase10ToBaseN(u8_misc::convertU8StrToAsciiStr(str), baseDigitsAsInt); };

        auto approximate_if_decfp = [&](std::string& decfp)
        {
            auto exponent_it = std::ranges::find(decfp, '@');
            std::string temp{};

            if (literalType == lt_d32)
            {
                temp = nc_misc::approximate_base10_real(std::string{decfp.begin(), exponent_it}, 6);
                decfp = nc_misc::normalize_realNum(temp.append(exponent_it, decfp.end()), immutable_lref);
            }
            else if (literalType == lt_d64)
            {
                temp = nc_misc::approximate_base10_real(std::string{decfp.begin(), exponent_it}, 15);
                decfp = nc_misc::normalize_realNum(temp.append(exponent_it, decfp.end()), immutable_lref);
            }
            else if (literalType == lt_d128)
            {
                temp = nc_misc::approximate_base10_real(std::string{decfp.begin(), exponent_it}, 34);
                decfp = nc_misc::normalize_realNum(temp.append(exponent_it, decfp.end()), immutable_lref);
            }
            else if (literalType == lt_d256)
            {
                temp = nc_misc::approximate_base10_real(std::string{decfp.begin(), exponent_it}, 72);
                decfp = nc_misc::normalize_realNum(temp.append(exponent_it, decfp.end()), immutable_lref);
            }
            else if (literalType == lt_d512)
            {
                temp = nc_misc::approximate_base10_real(std::string{decfp.begin(), exponent_it}, 148);
                decfp = nc_misc::normalize_realNum(temp.append(exponent_it, decfp.end()), immutable_lref);
            }
        };

        auto approximate_real_to_max_real = [&](std::string& realNum)
        {
            auto exponent_it = std::ranges::find(realNum, '@');
            std::string temp = nc_misc::approximate_base10_real(std::string{realNum.begin(), exponent_it}, 148);
            return nc_misc::normalize_realNum(temp.append(exponent_it, realNum.end()), immutable_lref);
        };

        auto printRealNumberCorrectly_u8 = [&](U8string_view realNumber, U8string_view base)
        {
            auto exponent_indicator_it = std::ranges::find(realNumber, '@'_u8);
            m_log.write("\033[48;2;10;6;26;32;1m", U8string_view{realNumber.begin(), exponent_indicator_it}, base, U8string_view{exponent_indicator_it, realNumber.end()}, "\033[0m");
        };

        auto printRealNumberCorrectly_ascii = [&](std::string_view realNumber, U8string_view base)
        {
            auto exponent_indicator_it = std::ranges::find(realNumber, '@'_u8);
            m_log.write("\033[48;2;10;6;26;32;1m", std::string_view{realNumber.begin(), exponent_indicator_it}, base, std::string_view{exponent_indicator_it, realNumber.end()}, "\033[0m");
        };

        // [&literalType]{ for (auto &&i : literalType) i = u8_misc::basicLatinToLowerCase(i); }();
        
        bool shouldReturn{};
        if (isUnicodeBaseValid())
        {
            setBaseDigitsAsStr();

            if (!isBaseWithinRange())
            {
                m_log.write("Real number literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m is tagged with a base that is not supported in nc");
                
                auto relativeColumn = m_relativeColumn + real_literal_size;
                start_log(relativeColumn, relativeColumn + base.size(), m_line), spaceLog();
                m_log.write("   Only bases (2 - 36) are supported in nc. Unicode base makers[\033[1m₀ ₁ ₂ ₃ ₄ ₅ ₆ ₇ ₈ ₉\033[0m]"), additionalLog();
                
                shouldReturn = true;
            }
            else
            {
                baseDigitsAsInt = std::uint8_t(std::stoi(baseDigitsAsStr));

                if (!areRealBaseDigitsValid())
                {
                    m_log.write("Real number literal contains invalid base(\033[48;2;10;6;26;33;1m", baseDigitsAsStr, "\033[0m) digit(s)");
                    start_log(m_relativeColumn, m_relativeColumn + real_literal_size, m_line), spaceLog();

                    m_log.write("   Only digits \033[48;2;10;6;26;32;1m");
                    
                    for (auto i = 0, j = 0; i < baseDigitsAsInt; ++i)
                    {
                        if (i >= 10)
                        m_log.write(char('A' + j), '(', char('a' + j), ')'), ++j;
                        else
                        m_log.write(i);

                        if (i != baseDigitsAsInt - 1)
                        m_log.write(',', ' ');
                    }
                    m_log.write("\033[0m are allowed in base(\033[48;2;10;6;26;33;1m", baseDigitsAsStr, "\033[0m)"), additionalLog();
                    
                    shouldReturn = true;
                }
            }
        }
        else
        {
            m_log.write("Real number literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m is tagged with a junk base");
            
            auto relativeColumn = m_relativeColumn + real_literal_size;
            start_log(relativeColumn, relativeColumn + base.size(), m_line), spaceLog();
            m_log.write("   Try any valid combination of these base markers[\033[1m₀ ₁ ₂ ₃ ₄ ₅ ₆ ₇ ₈ ₉\033[0m] instead. The valid combinations being from \033[1m₂\033[0m to \033[1m₃₆\033[0m");
            additionalLog();
            
            shouldReturn = true;
        }

        if (!exponent.empty())
        {
            if (std::ranges::all_of(exponent, [](U8char& x){ return u8_misc::isBasicLatinAlphabetNumeral(std::uint32_t(x)); }))
            {
                if (!nc_misc::areBaseDigitsValid(ascii_exponent, std::uint8_t(10)))
                {
                    m_log.write("The exponent of real number literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m, contains invalid base(\033[48;2;10;6;26;33;1m10\033[0m) digit(s)");
                    
                    auto relativeColumn = m_relativeColumn + real_literal_size + base.size() + 1/*plus the exponent indicator*/;
                    start_log(relativeColumn, relativeColumn + exponent.size(), m_line), spaceLog();
                    m_log.write("   You're getting this error because the exponent of real-numbers must be base(\033[48;2;10;6;26;33;1m10\033[0m) regardless of the base it is tagged with"), additionalLog();

                    shouldReturn = true;
                }
                else if (nc_misc::integer_gt_eq(ascii_exponent, "20000"/*since the highest possible exponent is 16'384*/))
                {
                    if (isExponentNegative)
                    m_log.write("The exponent of real number literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m is way too small, increase it");
                    else
                    m_log.write("The exponent of real number literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m is comically large, reduce it");

                    auto relativeColumn = m_relativeColumn + real_literal_size + base.size() + 1/*plus the exponent indicator*/;
                    start_log(relativeColumn, relativeColumn + exponent.size(), m_line), spaceLog();

                    m_log.write("   This is because the highest/lowest possible exponent in nc is \033[48;2;10;6;26;32;1m±16384₁₀\033[0m"), additionalLog();

                    shouldReturn = true;
                }
            }
            else
            {
                m_log.write("Base(\033[48;2;10;6;26;33;1m10\033[0m) digits where expected as the exponent in real number literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m instead got this \033[48;2;10;6;26;31;1m", exponent, "\033[0m");
                
                auto relativeColumn = m_relativeColumn + real_literal_size + base.size() + 1/*plus the exponent indicator*/;
                start_log(relativeColumn, relativeColumn + exponent.size(), m_line);

                shouldReturn = true;
            }
        }

        if (shouldReturn)
        return TokenType::_miscellany;
        
        auto operable_base10Token = [&]
        {
            if (baseDigitsAsStr == "10")
            {
                if (exponent.empty())
                return nc_misc::normalize_realNum(ascii_real_literal, immutable_lref);
                else
                {
                    if (isExponentNegative)
                    return nc_misc::normalize_realNum(ascii_real_literal + std::string{'@', '-'} + ascii_exponent, immutable_lref);
                    else
                    return nc_misc::normalize_realNum(ascii_real_literal + std::string{'@', '+'} + ascii_exponent, immutable_lref);
                }
            }
            else
            {
                if (exponent.empty())
                return ascii_real_literal;
                else
                {
                    std::string complete_ascii_real_token{};

                    if (isExponentNegative)
                    complete_ascii_real_token = ascii_real_literal + std::string{'@', '-'} + ascii_exponent;
                    else
                    complete_ascii_real_token = ascii_real_literal + std::string{'@', '+'} + ascii_exponent;
                    
                    return nc_misc::convertRealBaseNToBase10(complete_ascii_real_token, baseDigitsAsInt);
                }
            }
            return std::string{};
        }();

        if (auto tempBase10Token = approximate_real_to_max_real(operable_base10Token); !is_within_range(tempBase10Token, dec512fpMin, dec512fpMax))
        {
            m_log.write("Real number literal is not within range of nc's maximum decimal floating point which offers 148-digits of precision and 512-bits for storage");
            start_log(m_relativeColumn, m_relativeColumn + real_literal_size + base_exp.size(), m_line), spaceLog();

            if (baseDigitsAsStr == "10")
            {
                m_log.write("   Nc's maximum decimal floating point range: min("), printRealNumberCorrectly_u8(dec512fpMin, "₁₀"_u8str),
                m_log.write(") - max("), printRealNumberCorrectly_u8(dec512fpMax, "₁₀"_u8str), m_log.write(')');
                additionalLog();

                m_log.write("   Here's the real number literal normalized and approximated the way: "), printRealNumberCorrectly_ascii(tempBase10Token, "₁₀"_u8str);
                additionalLog();
            }
            else
            {
                m_log.write("   Nc's maximum decimal floating point range: min("), printRealNumberCorrectly_u8(dec512fpMin, "₁₀"_u8str),
                m_log.write(" or "), printRealNumberCorrectly_ascii(convertFpToBaseN(dec512fpMin), base),
                m_log.write(") - max("), printRealNumberCorrectly_u8(dec512fpMax, "₁₀"_u8str), m_log.write(" or "),
                printRealNumberCorrectly_ascii(convertFpToBaseN(dec512fpMax), base), m_log.write(')');
                additionalLog();
                
                m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(tempBase10Token, "₁₀"_u8str);
                additionalLog();
            }
            return TokenType::_miscellany;
        }

        approximate_if_decfp(operable_base10Token); //because dec floating requires approximation
        
        if (literalType.empty())
        {
            if (is_within_range(operable_base10Token, dec32fpMin, dec32fpMax))
            literalType = lt_d32;
            else if (is_within_range(operable_base10Token, dec64fpMin, dec64fpMax))
            literalType = lt_d64;
            else if (is_within_range(operable_base10Token, dec128fpMin, dec128fpMax))
            literalType = lt_d128;
            else if (is_within_range(operable_base10Token, dec256fpMin, dec256fpMax))
            literalType = lt_d256;
            else if (is_within_range(operable_base10Token, dec512fpMin, dec512fpMax))
            literalType = lt_d512;
        }
        else
        {
            if (literalType == lt_b16)
            {
                if (!is_within_range(operable_base10Token, bin16fpMin, bin16fpMax))
                {
                    m_log.writews("Real number literal is not within range of the IEEE binary16 floating point format");
                    start_log(m_relativeColumn, m_relativeColumn + real_literal_size + base_exp.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    {
                        m_log.write("   IEEE binary16 floating point range: min("), printRealNumberCorrectly_u8(bin16fpMin, "₁₀"_u8str),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(bin16fpMax, "₁₀"_u8str), m_log.write(')');
                        additionalLog();
                    }
                    else
                    {
                        m_log.write("   IEEE binary16 floating point range: min("), printRealNumberCorrectly_u8(bin16fpMin, "₁₀"_u8str),
                        m_log.write(" or "), printRealNumberCorrectly_ascii(convertFpToBaseN(bin16fpMin), base),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(bin16fpMax, "₁₀"_u8str), m_log.write(" or "),
                        printRealNumberCorrectly_ascii(convertFpToBaseN(bin16fpMax), base), m_log.write(')');
                        additionalLog();
                        
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(operable_base10Token, "₁₀"_u8str);
                        additionalLog();
                    }

                    m_log.write("   Where \033[48;2;10;6;26;33;1mmax\033[0m is the largest normal number and \033[48;2;10;6;26;33;1mmin\033[0m is the smallest denormal number for that binary floating point format");
                    additionalLog();
                }
            }
            else if (literalType == lt_b32)
            {
                if (!is_within_range(operable_base10Token, bin32fpMin, bin32fpMax))
                {
                    m_log.writews("Real number literal is not within range of the IEEE binary32 floating point format");
                    start_log(m_relativeColumn, m_relativeColumn + real_literal_size + base_exp.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    {
                        m_log.write("   IEEE binary32 floating point range: min("), printRealNumberCorrectly_u8(bin32fpMin, "₁₀"_u8str),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(bin32fpMax, "₁₀"_u8str), m_log.write(')');
                        additionalLog();
                    }
                    else
                    {
                        m_log.write("   IEEE binary32 floating point range: min("), printRealNumberCorrectly_u8(bin32fpMin, "₁₀"_u8str),
                        m_log.write(" or "), printRealNumberCorrectly_ascii(convertFpToBaseN(bin32fpMin), base),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(bin32fpMax, "₁₀"_u8str), m_log.write(" or "),
                        printRealNumberCorrectly_ascii(convertFpToBaseN(bin32fpMax), base), m_log.write(')');
                        additionalLog();
                        
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(operable_base10Token, "₁₀"_u8str);
                        additionalLog();
                    }

                    m_log.write("   Where \033[48;2;10;6;26;33;1mmax\033[0m is the largest normal number and \033[48;2;10;6;26;33;1mmin\033[0m is the smallest denormal number for that binary floating point format");
                    additionalLog();
                }
            }
            else if (literalType == lt_b64)
            {
                if (!is_within_range(operable_base10Token, bin64fpMin, bin64fpMax))
                {
                    m_log.writews("Real number literal is not within range of the IEEE binary64 floating point format");
                    start_log(m_relativeColumn, m_relativeColumn + real_literal_size + base_exp.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    {
                        m_log.write("   IEEE binary64 floating point range: min("), printRealNumberCorrectly_u8(bin64fpMin, "₁₀"_u8str),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(bin64fpMax, "₁₀"_u8str), m_log.write(')');
                        additionalLog();
                    }
                    else
                    {
                        m_log.write("   IEEE binary64 floating point range: min("), printRealNumberCorrectly_u8(bin64fpMin, "₁₀"_u8str),
                        m_log.write(" or "), printRealNumberCorrectly_ascii(convertFpToBaseN(bin64fpMin), base),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(bin64fpMax, "₁₀"_u8str), m_log.write(" or "),
                        printRealNumberCorrectly_ascii(convertFpToBaseN(bin64fpMax), base), m_log.write(')');
                        additionalLog();
                        
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(operable_base10Token, "₁₀"_u8str);
                        additionalLog();
                    }

                    m_log.write("   Where \033[48;2;10;6;26;33;1mmax\033[0m is the largest normal number and \033[48;2;10;6;26;33;1mmin\033[0m is the smallest denormal number for that binary floating point format");
                    additionalLog();
                }
            }
            else if (literalType == lt_b128)
            {
                if (!is_within_range(operable_base10Token, bin128fpMin, bin128fpMax))
                {
                    m_log.writews("Real number literal is not within range of the IEEE binary128 floating point format");
                    start_log(m_relativeColumn, m_relativeColumn + real_literal_size + base_exp.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    {
                        m_log.write("   IEEE binary128 floating point range: min("), printRealNumberCorrectly_u8(bin128fpMin, "₁₀"_u8str),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(bin128fpMax, "₁₀"_u8str), m_log.write(')');
                        additionalLog();
                    }
                    else
                    {
                        m_log.write("   IEEE binary128 floating point range: min("), printRealNumberCorrectly_u8(bin128fpMin, "₁₀"_u8str),
                        m_log.write(" or "), printRealNumberCorrectly_ascii(convertFpToBaseN(bin128fpMin), base),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(bin128fpMax, "₁₀"_u8str), m_log.write(" or "),
                        printRealNumberCorrectly_ascii(convertFpToBaseN(bin128fpMax), base), m_log.write(')');
                        additionalLog();
                        
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(operable_base10Token, "₁₀"_u8str);
                        additionalLog();
                    }

                    m_log.write("   Where \033[48;2;10;6;26;33;1mmax\033[0m is the largest normal number and \033[48;2;10;6;26;33;1mmin\033[0m is the smallest denormal number for that binary floating point format");
                    additionalLog();
                }
            }
            else if (literalType == lt_d32)
            {
                if (!is_within_range(operable_base10Token, dec32fpMin, dec32fpMax))
                {
                    m_log.writews("Real number literal is not within range of nc's implementation of decimal floating point that offers 6-digits of precision and uses 32-bits for storage");
                    start_log(m_relativeColumn, m_relativeColumn + real_literal_size + base_exp.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    {
                        m_log.write("   Nc's decimal floating point range: min("), printRealNumberCorrectly_u8(dec32fpMin, "₁₀"_u8str),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(dec32fpMax, "₁₀"_u8str), m_log.write(')');
                        additionalLog();

                        m_log.write("   Here's the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(operable_base10Token, "₁₀"_u8str);
                        additionalLog();
                    }
                    else
                    {
                        m_log.write("   Nc's decimal floating point range: min("), printRealNumberCorrectly_u8(dec32fpMin, "₁₀"_u8str),
                        m_log.write(" or "), printRealNumberCorrectly_ascii(convertFpToBaseN(dec32fpMin), base),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(dec32fpMax, "₁₀"_u8str), m_log.write(" or "),
                        printRealNumberCorrectly_ascii(convertFpToBaseN(dec32fpMax), base), m_log.write(')');
                        additionalLog();
                        
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(operable_base10Token, "₁₀"_u8str);
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_d64)
            {
                if (!is_within_range(operable_base10Token, dec64fpMin, dec64fpMax))
                {
                    m_log.writews("Real number literal is not within range of nc's implementation of decimal floating point that offers 15-digits of precision and uses 64-bits for storage");
                    start_log(m_relativeColumn, m_relativeColumn + real_literal_size + base_exp.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    {
                        m_log.write("   Nc's decimal floating point range: min("), printRealNumberCorrectly_u8(dec64fpMin, "₁₀"_u8str),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(dec64fpMax, "₁₀"_u8str), m_log.write(')');
                        additionalLog();

                        m_log.write("   Here's the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(operable_base10Token, "₁₀"_u8str);
                        additionalLog();
                    }
                    else
                    {
                        m_log.write("   Nc's decimal floating point range: min("), printRealNumberCorrectly_u8(dec64fpMin, "₁₀"_u8str),
                        m_log.write(" or "), printRealNumberCorrectly_ascii(convertFpToBaseN(dec64fpMin), base),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(dec64fpMax, "₁₀"_u8str), m_log.write(" or "),
                        printRealNumberCorrectly_ascii(convertFpToBaseN(dec64fpMax), base), m_log.write(')');
                        additionalLog();
                        
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(operable_base10Token, "₁₀"_u8str);
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_d128)
            {
                if (!is_within_range(operable_base10Token, dec128fpMin, dec128fpMax))
                {
                    m_log.writews("Real number literal is not within range of nc's implementation of decimal floating point that offers 34-digits of precision and uses 128-bits for storage");
                    start_log(m_relativeColumn, m_relativeColumn + real_literal_size + base_exp.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    {
                        m_log.write("   Nc's decimal floating point range: min("), printRealNumberCorrectly_u8(dec128fpMin, "₁₀"_u8str),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(dec128fpMax, "₁₀"_u8str), m_log.write(')');
                        additionalLog();

                        m_log.write("   Here's the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(operable_base10Token, "₁₀"_u8str);
                        additionalLog();
                    }
                    else
                    {
                        m_log.write("   Nc's decimal floating point range: min("), printRealNumberCorrectly_u8(dec128fpMin, "₁₀"_u8str),
                        m_log.write(" or "), printRealNumberCorrectly_ascii(convertFpToBaseN(dec128fpMin), base),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(dec128fpMax, "₁₀"_u8str), m_log.write(" or "),
                        printRealNumberCorrectly_ascii(convertFpToBaseN(dec128fpMax), base), m_log.write(')');
                        additionalLog();
                        
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(operable_base10Token, "₁₀"_u8str);
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_d256)
            {
                if (!is_within_range(operable_base10Token, dec256fpMin, dec256fpMax))
                {
                    m_log.writews("Real number literal is not within range of nc's implementation of decimal floating point that offers 72-digits of precision and uses 256-bits for storage");
                    start_log(m_relativeColumn, m_relativeColumn + real_literal_size + base_exp.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    {
                        m_log.write("   Nc's decimal floating point range: min("), printRealNumberCorrectly_u8(dec256fpMin, "₁₀"_u8str),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(dec256fpMax, "₁₀"_u8str), m_log.write(')');
                        additionalLog();

                        m_log.write("   Here's the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(operable_base10Token, "₁₀"_u8str);
                        additionalLog();
                    }
                    else
                    {
                        m_log.write("   Nc's decimal floating point range: min("), printRealNumberCorrectly_u8(dec256fpMin, "₁₀"_u8str),
                        m_log.write(" or "), printRealNumberCorrectly_ascii(convertFpToBaseN(dec256fpMin), base),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(dec256fpMax, "₁₀"_u8str), m_log.write(" or "),
                        printRealNumberCorrectly_ascii(convertFpToBaseN(dec256fpMax), base), m_log.write(')');
                        additionalLog();
                        
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(operable_base10Token, "₁₀"_u8str);
                        additionalLog();
                    }
                }
            }
            else if (literalType == lt_d512)//I don' actually need this, will remove later i guess
            {
                if (!is_within_range(operable_base10Token, dec512fpMin, dec512fpMax))
                {
                    m_log.writews("Real number literal is not within range of nc's implementation of decimal floating point that offers 148-digits of precision and uses 512-bits for storage");
                    start_log(m_relativeColumn, m_relativeColumn + real_literal_size + base_exp.size(), m_line), spaceLog();

                    if (baseDigitsAsStr == "10")
                    {
                        m_log.write("   Nc's decimal floating point range: min("), printRealNumberCorrectly_u8(dec512fpMin, "₁₀"_u8str),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(dec512fpMax, "₁₀"_u8str), m_log.write(')');
                        additionalLog();

                        m_log.write("   Here's the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(operable_base10Token, "₁₀"_u8str);
                        additionalLog();
                    }
                    else
                    {
                        m_log.write("   Nc's decimal floating point range: min("), printRealNumberCorrectly_u8(dec512fpMin, "₁₀"_u8str),
                        m_log.write(" or "), printRealNumberCorrectly_ascii(convertFpToBaseN(dec512fpMin), base),
                        m_log.write(") - max("), printRealNumberCorrectly_u8(dec512fpMax, "₁₀"_u8str), m_log.write(" or "),
                        printRealNumberCorrectly_ascii(convertFpToBaseN(dec512fpMax), base), m_log.write(')');
                        additionalLog();
                        
                        m_log.write("   Here's the base(\033[48;2;10;6;26;33;1m10\033[0m) equivalent of the real number literal normalized and approximated by the way: "), printRealNumberCorrectly_ascii(operable_base10Token, "₁₀"_u8str);
                        additionalLog();
                    }
                }
            }
            else
            {
                if (literalType == lt_i8 or literalType == lt_i16 or literalType == lt_i32 or literalType == lt_i64 or literalType == lt_i128 or literalType == lt_i256 or literalType == lt_i512 or literalType == lt_i1024 or literalType == lt_ui8 or literalType == lt_ui16 or literalType == lt_ui32 or literalType == lt_ui64 or literalType == lt_ui128 or literalType == lt_ui256 or literalType == lt_ui512 or literalType == lt_ui1024)
                {
                    m_log.writews("Real number literal is tagged with an integer literal type instead of a real-number literal type");
                    auto relativeColumn = m_relativeColumn + real_literal_size + base_exp.size() + 1/*plsu[`]*/;
                    start_log(relativeColumn, relativeColumn + literalType.size(), m_line), spaceLog();
                
                    m_log.write("   Try the below instead:"), additionalLog();
                    m_log.write("   ● \033[48;2;10;6;26;33;1mb16, b32, b64 or b128\033[0m for IEEE binary floating point numbers"), additionalLog();
                    m_log.write("   ● \033[48;2;10;6;26;33;1md32, d64, d128, d256, d512\033[0m for NC implementation of decimal floating point numbers"), additionalLog();
                    m_log.write("   Note: they are all case sensitive"), additionalLog();
                }
                else
                {
                    bool isLiteralTypeNameValidUnicodeCharacter{};

                    for (size_t i = 0, n = literalType.size(); i < n; i++)
                    {
                        i == 0?
                        isLiteralTypeNameValidUnicodeCharacter = acceptableFirstUnicodeCharacter(literalType[i])
                        :
                        isLiteralTypeNameValidUnicodeCharacter = acceptableRestOfUnicodeCharacter(literalType[i]);
                        
                        if (!isLiteralTypeNameValidUnicodeCharacter) break;
                    }

                    if (!isLiteralTypeNameValidUnicodeCharacter)
                    {
                        m_log.write("Real number literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m is tagged with a literalType that contains invalid nc unicode characters");
                        
                        auto relativeColumn = m_relativeColumn + real_literal_size + base_exp.size() + 1/*plsu[`]*/;
                        start_log(relativeColumn, relativeColumn + literalType.size(), m_line);
                    }
                }
            }
        }
    
        return TokenType::literal;
    }

    Lexer::TokenType Lexer::tokenizeCharLiterals(const U8string* literalType_ptr)
    {
        m_tokenString.clear();//for removing the literalType if there is any

        auto formerFilePos{m_filePosition};
        std::vector<std::uint32_t> rawStr_escape_sequence_size_list{};
        bool wasLiteralTypePtrNull{};

        if (literalType_ptr == nullptr) wasLiteralTypePtrNull = true, recordRelativeColumn(); //do not move

        //set literalType_ptr to the default characterWise-literal which is [ p ] if it is null
        if (literalType_ptr == nullptr) literalType_ptr = &lt_characterWise_p;

        auto literalTypeName_plus_characterWiseType = [&]()
        {
            if (*literalType_ptr == lt_characterWise_a) return std::string{"ascii-character"};
            else if (*literalType_ptr == lt_characterWise_p) return std::string{"plain-character"};
            else if (*literalType_ptr == lt_characterWise_u8) return std::string{"utf8-character"};
            return std::string{"character"};
        }();

        while (true)
        {
            if (nextFilePosAndColumn(), m_isEndofFile or m_fileBuffer[m_filePosition] == '\n'_u8)
            {
                m_isEndofFile? m_log.write("") : m_log.write("");
                m_log.writews("Unterminated", literalTypeName_plus_characterWiseType, "literal, expected this \033[48;2;10;6;26;32;1m'\033[0m before ");
                m_isEndofFile? m_log.write("\033[48;2;10;6;26;31;1mEOF(end_of_file)\033[0m") : m_log.write("\n");

                start_log();
                return TokenType::_miscellany;
            }
            else if (m_fileBuffer[m_filePosition] == '\''_u8)
            {
                nextFilePosAndColumn(); //skip past the string terminating single-quote
                break;
            }
            else if (m_fileBuffer[m_filePosition] == '\\'_u8) //for skipping potential escape code squences
            {
                auto rawStr_escape_sequence_size = 0u;
                auto cummulative_next = [&]{ nextFilePosAndColumn(), ++rawStr_escape_sequence_size; };

                if (cummulative_next(), !m_isEndofFile and m_fileBuffer[m_filePosition] == '#'_u8)//for capturing raw-escape sequence
                {
                    auto hashTag_number = 1u;

                    while (cummulative_next(), !m_isEndofFile and m_fileBuffer[m_filePosition] == '#'_u8)
                    ++hashTag_number;
                    
                    if (m_isEndofFile)
                    {
                        m_log.write("Expected this \033[48;2;10;6;26;32;1m{\033[0m to begin the escape sequence for raw character entry, before \033[48;2;10;6;26;31;1mEOF(end_of_file)\033[0m");
                        start_log();
                    }
                    else if (m_fileBuffer[m_filePosition] == '{'_u8)
                    {
                        while (true)
                        {
                            if (cummulative_next(), m_isEndofFile or m_fileBuffer[m_filePosition] == '\n'_u8)
                            {
                                m_isEndofFile? m_log.write("") : m_log.write("");
                                m_log.write("Unterminated raw character escape sequence, expected this \033[48;2;10;6;26;32;1m}");

                                for (auto i = 0u; i < hashTag_number; i++) m_log.write('#');

                                m_isEndofFile?
                                m_log.write("\033[0m before \033[48;2;10;6;26;31;1mEOF(end_of_file)\033[0m") : m_log.write("\033[0m before \n");
                                
                                start_log();
                                break;
                            }
                            else if (m_fileBuffer[m_filePosition] == '}'_u8 and (cummulative_next(), /*!m_isEndofFile and*/ m_fileBuffer[m_filePosition] == '#'_u8))
                            {
                                decltype(hashTag_number) count{};

                                while (!m_isEndofFile and m_fileBuffer[m_filePosition] == '#'_u8)
                                {
                                    if (++count == hashTag_number)
                                    {
                                        rawStr_escape_sequence_size_list.push_back(rawStr_escape_sequence_size);
                                        
                                        goto outside_main_while_loop;
                                    }
                                    
                                    cummulative_next();
                                }
                            }
                        }
                        /*label*/ outside_main_while_loop:
                    }
                    else
                    {
                        m_log.write("Expected this \033[48;2;10;6;26;32;1m{\033[0m to begin the escape sequence for raw character entry, instead of this \033[48;2;10;6;26;31;1m", m_fileBuffer[m_filePosition], "\033[0m");
                        
                        bool shouldNewLineBeConsidered = m_fileBuffer[m_filePosition] == '\n'_u8;
                        start_log(/*relativeColumn*/ m_absoluteColumn, /*absoluteColumn*/ m_absoluteColumn + 1, /*line*/ m_line, shouldNewLineBeConsidered);
                        rawStr_escape_sequence_size_list.push_back(0);

                        prevFilePosAndColumn();//for reversing incase the character is the characterWise terminator
                    }
                }
                else if (m_fileBuffer[m_filePosition] == '['_u8)
                {
                    while (nextFilePosAndColumn(), u8_misc::isBasicLatinAlphabetNumeral(std::uint32_t(m_fileBuffer[m_filePosition])) or m_fileBuffer[m_filePosition] == '\''_u8)
                    {}
                }
            }
        }
        
        //extract the token to m_tokenString
        {
            std::uint32_t count{};
            for (size_t i = formerFilePos; m_isEndofFile? i <= m_filePosition : i < m_filePosition; ++i)
            ++count;
            
            m_tokenString.assign(m_fileBuffer, formerFilePos, count);
        }

        if (validateNormalCharWiseLiterals(/*literalTypeName_plus_characterWiseType*/literalTypeName_plus_characterWiseType, /*literalType*/*literalType_ptr, /*rawStr_escape_sequence_size_list*/ rawStr_escape_sequence_size_list) == TokenType::_miscellany)
        return TokenType::_miscellany;

        auto isCharacterSize_gt_1 = [&]
        {
            if (rawStr_escape_sequence_size_list.size() > 1) return false;
            
            auto count = 0uz;
            for (auto i = 1uz, n = m_tokenString.size() - 1; i < n; i++)
            {
                if (m_tokenString[i] == '\\'_u8)
                {
                    if (m_tokenString[++i] == '#'_u8)
                    {
                        //reason is because the raw-escape is erroneous and already has a log about it, so need to report another log
                        if (rawStr_escape_sequence_size_list[0] == 0)
                        return false;

                        for (size_t j = i; ; j++)
                        {
                            if (m_tokenString[j] == '{'_u8)
                            {
                                const auto delimeter_size = (j - i + 1) * 2;
                                if (rawStr_escape_sequence_size_list[0] - delimeter_size > 1) return true;
                                else
                                break;
                            }
                        }
                        --i += rawStr_escape_sequence_size_list[0];
                    }
                    else if (m_tokenString[i] == '['_u8)
                    {
                        while (m_tokenString[++i] != ']'_u8) {}
                    }
                    else
                    ++i;
                }
                ++count;
            }
            io::cout.writews_nl("count:", count);
            return count > 1;
        }();

        //size check
        if (isCharacterSize_gt_1)
        m_log.write("Size of ", literalTypeName_plus_characterWiseType ," literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m cannot be greater than 1"), start_log();
        
        auto beginText = [&]()
        {
            std::string copy = literalTypeName_plus_characterWiseType;
            return copy.front() = char(std::toupper(copy.front())), copy;
        };

        if ((*literalType_ptr == lt_characterWise_a or *literalType_ptr == lt_characterWise_p) and std::ranges::any_of(m_tokenString, [](U8char& x){ return x.getByteEncoding() != U8char::ByteEncoding::one; }))
        {
            m_log.write(beginText(), " literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m cannot have characters greater than 1-byte of encoding");
            start_log();
        }
        
        //append the literalType back
        if (!wasLiteralTypePtrNull) m_tokenString = *literalType_ptr + m_tokenString;

        addToTokenDataList(std::optional<std::uint32_t>{}, m_line, false, false, *literalType_ptr);

        return TokenType::literal;
    }
    
    Lexer::TokenType Lexer::tokenizeStrLiterals(const U8string* literalType_ptr)
    {
        m_tokenString.clear();//for removing the literalType if there is any

        auto formerFilePos{m_filePosition};
        std::optional<std::uint32_t> optLine{};
        std::uint32_t start_line{};
        std::vector<std::uint32_t> rawStr_escape_sequence_size_list{};
        bool wasLiteralTypePtrNull{};

        if (literalType_ptr == nullptr) wasLiteralTypePtrNull = true, recordRelativeColumn(); //do not move

        //set literalType_ptr to the default characterWise-literal which is [ p ] if it is null
        if (literalType_ptr == nullptr) literalType_ptr = &lt_characterWise_p;

        auto literalTypeName_plus_characterWiseType = [&]()
        {
            if (*literalType_ptr == lt_characterWise_a) return "ascii-string";
            else if (*literalType_ptr == lt_characterWise_p) return "plain-string";
            else if (*literalType_ptr == lt_characterWise_u8) return "utf8-string";
            return "string";
        }();

        start_line = m_line;

        while (true)
        {
            if (nextFilePosAndColumn(), m_isEndofFile)
            {
                m_log.writews("Unterminated", literalTypeName_plus_characterWiseType, "literal, expected this \033[48;2;10;6;26;32;1m\"\033[0m before \033[48;2;10;6;26;31;1mEOF(end_of_file)\033[0m");
                start_line == m_line? start_log() : start_multiLineLog(start_line, m_relativeColumn);

                return TokenType::_miscellany;
            }
            else if (m_fileBuffer[m_filePosition] == '"'_u8)
            {
                nextFilePosAndColumn(); //skip past the string terminating dobule-quote
                break;
            }
            else if (m_fileBuffer[m_filePosition] == '\\'_u8) //for skipping potential escape code squences
            {
                auto rawStr_escape_sequence_size = 0u;
                auto cummulative_next = [&]{ nextFilePosAndColumn(), ++rawStr_escape_sequence_size; };

                if (cummulative_next(), !m_isEndofFile and m_fileBuffer[m_filePosition] == '#'_u8)//for capturing raw-escape sequence
                {
                    auto hashTag_number = 1u;

                    while (cummulative_next(), !m_isEndofFile and m_fileBuffer[m_filePosition] == '#'_u8)
                    ++hashTag_number;
                    
                    if (m_isEndofFile)
                    {
                        m_log.write("Expected this \033[48;2;10;6;26;32;1m{\033[0m to begin the escape sequence for raw string entry, before \033[48;2;10;6;26;31;1mEOF(end_of_file)\033[0m");
                        start_line == m_line? start_log() : start_multiLineLog(start_line, m_relativeColumn);
                    }
                    else if (m_fileBuffer[m_filePosition] == '{'_u8)
                    {
                        while (true)
                        {
                            if (cummulative_next(), m_isEndofFile)
                            {
                                m_log.write("Unterminated raw string escape sequence, expected this \033[48;2;10;6;26;32;1m}");

                                for (auto i = 0u; i < hashTag_number; i++) m_log.write('#');

                                m_log.write("\033[0m before \033[48;2;10;6;26;31;1mEOF(end_of_file)\033[0m");
                                
                                start_line == m_line? start_log() : start_multiLineLog(start_line, m_relativeColumn);
                                break;
                            }
                            else if (m_fileBuffer[m_filePosition] == '}'_u8 and (cummulative_next(), /*!m_isEndofFile and*/ m_fileBuffer[m_filePosition] == '#'_u8))
                            {
                                decltype(hashTag_number) count{};

                                while (!m_isEndofFile and m_fileBuffer[m_filePosition] == '#'_u8)
                                {
                                    if (++count == hashTag_number)
                                    {
                                        rawStr_escape_sequence_size_list.push_back(rawStr_escape_sequence_size);
                                        
                                        goto outside_main_while_loop;
                                    }
                                    
                                    cummulative_next();
                                }
                            }
                            
                            if (!m_isEndofFile and m_fileBuffer[m_filePosition] == '\n'_u8)
                            newLineFound();
                        }
                        /*label*/ outside_main_while_loop:
                    }
                    else
                    {
                        m_log.write("Expected this \033[48;2;10;6;26;32;1m{\033[0m to begin the escape sequence for raw string entry, instead of this \033[48;2;10;6;26;31;1m", m_fileBuffer[m_filePosition], "\033[0m");
                        
                        bool shouldNewLineBeConsidered = m_fileBuffer[m_filePosition] == '\n'_u8;
                        start_log(/*relativeColumn*/ m_absoluteColumn, /*absoluteColumn*/ m_absoluteColumn + 1, /*line*/ m_line, shouldNewLineBeConsidered);
                        rawStr_escape_sequence_size_list.push_back(0);

                        prevFilePosAndColumn();
                    }
                }
            }
            
            if (!m_isEndofFile and m_fileBuffer[m_filePosition] == '\n'_u8)
            newLineFound();
        }
        //if the raw string is multiline, set the optional-line
        if (start_line != m_line) optLine = start_line;
        
        //extract the token to m_tokenString
        {
            std::uint32_t count{};
            for (size_t i = formerFilePos; m_isEndofFile? i <= m_filePosition : i < m_filePosition; ++i)
            ++count;
            
            m_tokenString.assign(m_fileBuffer, formerFilePos, count);
        }

        validateNormalCharWiseLiterals(/*literalTypeName_plus_characterWiseType*/literalTypeName_plus_characterWiseType, /*literalType*/*literalType_ptr, /*rawStr_escape_sequence_size_list*/ rawStr_escape_sequence_size_list);

        //append the literalType back
        if (!wasLiteralTypePtrNull) m_tokenString = *literalType_ptr + m_tokenString;

        addToTokenDataList(optLine, m_line, false, false, *literalType_ptr);
        
        return TokenType::literal;
    }

    Lexer::TokenType Lexer::validateNormalCharWiseLiterals(std::string literalTypeName_plus_characterWiseType, const U8string& literalType, const std::vector<std::uint32_t>& rawStr_escape_sequence_size_list)
    {
        TokenType result = TokenType::literal;
        std::size_t rawStr_escape_sequence_size_list_index{};
        std::vector<std::uint32_t> newLineStore{};
        bool isNewLineStoreInitialized{};
        static std::pair</*is_escapeSequence_list_already_printed*/ bool, /*log-number*/std::size_t> print_escapeSequence_list_once_pair{};

        auto beginText = [&]()
        {
            std::string copy = literalTypeName_plus_characterWiseType;
            return copy.front() = char(std::toupper(copy.front())), copy;
        };

        auto init_newLineStore = [&]()
        {
            if (!isNewLineStoreInitialized)
            isNewLineStoreInitialized = true;
            else
            return;

            for (size_t i = 0, n = m_tokenString.size(); i < n; i++)
            {
                if (m_tokenString[i] == '\n'_u8)
                newLineStore.push_back(i);
            }
        };

        auto calc_line_and_column = [&](std::size_t index) -> std::pair<std::uint32_t, std::uint32_t>
        {
            std::pair</*line:*/ std::uint32_t, /*column:*/ std::uint32_t> line_column{};
            //for getting the column
            {
                if (newLineStore.empty() or index <= newLineStore.front())
                {
                    auto offset = m_relativeColumn - 1;
                    line_column.second = offset + (index + 1/*to get the position*/);
                }
                else
                {   
                    for (auto i = index, count = 0uz; ;)
                    {
                        ++count, --i;

                        if (m_tokenString[i] == '\n'_u8)
                        {
                            line_column.second = count;
                            break;
                        }
                    }
                }
            }
            //for getting the line
            {
                if (newLineStore.empty())
                line_column.first = m_line;
                else
                {
                    auto start_line = m_line - newLineStore.size();
                    for (size_t i = 0, n = newLineStore.size(); i < n; i++)
                    {
                        if (index <= newLineStore[i])//checking for the first new-line index that is <= the index
                        {
                            /*if the index is the position, minus one*/
                            line_column.first = start_line + i;
                            break;
                        }
                    }
                }
            }
            return line_column;
        };

        auto setBaseDigitsAsStr = [&](U8string_view base)
        {
            if (base.empty())
            return std::string{"16"};

            std::map<U8char, char> pseudoBaseMap
            {
                {"₀"_u8, '0'}, {"₁"_u8, '1'}, {"₂"_u8, '2'}, {"₃"_u8, '3'}, {"₄"_u8, '4'}, {"₅"_u8, '5'}, {"₆"_u8, '6'}, {"₇"_u8, '7'}, {"₈"_u8, '8'}, {"₉"_u8, '9'}
            };
            std::string digits{};

            for (auto &&i : base)
            digits.push_back(pseudoBaseMap[i]);

            return nc_misc::trim_integer_leading_zeros(digits), digits;
        };

        for (auto i = 0uz, size = m_tokenString.size(); i < size; ++i)
        {
            if (m_tokenString[i] == '\\'_u8)
            {
                switch (char(std::uint32_t(m_tokenString[++i])))
                {
                case '0': case 'e': case 'b': case 'f': case 'n': case 'r': case 't': case 'v': case '\'': case '\"': case '\\':  break;
                case '#':
                {
                    --i += rawStr_escape_sequence_size_list[rawStr_escape_sequence_size_list_index++];
                    break;
                }
                case '[':
                {
                    U8string integer_literal{}, base{};
                    
                    auto integer_start_i = ++i;
                    while (u8_misc::isBasicLatinAlphabetNumeral(std::uint32_t(m_tokenString[i])) or m_tokenString[i] == '\''_u8)
                    ++i;

                    integer_literal.assign(m_tokenString, integer_start_i, i - integer_start_i);

                    auto base_start_i = i;
                    std::uint32_t unicode_cp{};
                    while (unicode_cp = std::uint32_t(m_tokenString[i]), (unicode_cp >= 0x2080 and unicode_cp <= 0x2089))
                    ++i;

                    base.assign(m_tokenString, base_start_i, i - base_start_i);

                    /*_______________________________________________checking-starts-from-here_________________________________________________*/
                    
                    if (!base.empty() and integer_literal.empty())
                    {
                        m_log.write("In ", literalTypeName_plus_characterWiseType, " literal, escaped unicode code-point entry only contains a base and no code-point");

                        if (!isNewLineStoreInitialized)
                        init_newLineStore(), isNewLineStoreInitialized = true;
                        
                        auto line_column = calc_line_and_column(base_start_i);
                        start_log(/*relativeColumn:*/ line_column.second, /*absoluteColumn*/ line_column.second + base.size(), line_column.first), spaceLog();
                        m_log.write("   Note: the unicode code-point is base agnostic"), additionalLog();

                        result = TokenType::_miscellany;
                    }
                    else
                    {
                        auto baseDigitsAsStr = setBaseDigitsAsStr(base);
                        auto baseDigitsAsInteger = static_cast<std::uint8_t>(std::stoi(baseDigitsAsStr));
                        auto original_integer_size = integer_literal.size();

                        if (integer_literal.starts_with('\''_u8))
                        {
                            m_log.write("In ", literalTypeName_plus_characterWiseType, " literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m, the digit seperator \033[48;2;10;6;26;33;1m'\033[0m should not start an escaped unicode code-point entry");

                            init_newLineStore();
                            
                            auto line_column = calc_line_and_column(integer_start_i);
                            start_log(/*relativeColumn:*/ line_column.second, /*absoluteColumn*/ line_column.second + original_integer_size, line_column.first);
                        }
                        else if (integer_literal.ends_with('\''_u8))
                        {
                            m_log.write("In ", literalTypeName_plus_characterWiseType, " literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m, the digit seperator \033[48;2;10;6;26;33;1m'\033[0m should not end an escaped unicode code-point entry");

                            init_newLineStore();
                            
                            auto line_column = calc_line_and_column(integer_start_i);
                            start_log(/*relativeColumn:*/ line_column.second, /*absoluteColumn*/ line_column.second + original_integer_size, line_column.first);
                        }

                        //remove any digit seperator
                        {
                            auto newEnd = std::remove(integer_literal.begin(), integer_literal.end(), '\'');
                            integer_literal.erase(newEnd, integer_literal.end());
                        }

                        auto ascii_integer_literal = u8_misc::convertU8StrToAsciiStr(integer_literal);//move this below in my language

                        if (integer_literal.empty())
                        goto closing_square_brace_check;

                        //isBaseWithinRange
                        if (!(nc_misc::integer_gt_eq(baseDigitsAsStr, "2") and nc_misc::integer_lt_eq(baseDigitsAsStr, "36")))
                        {
                            m_log.write("In ", literalTypeName_plus_characterWiseType, " literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m, escaped unicode code-point entry \033[48;2;10;6;26;33;1m", integer_literal, "\033[0m is tagged with a base that is not supported in nc");

                            init_newLineStore();
                            
                            auto line_column = calc_line_and_column(base_start_i);
                            start_log(/*relativeColumn:*/ line_column.second, /*absoluteColumn*/ line_column.second + base.size(), line_column.first), spaceLog();
                            m_log.write("   Only bases (2 - 36) are supported in nc. Unicode base makers[\033[1m₀ ₁ ₂ ₃ ₄ ₅ ₆ ₇ ₈ ₉\033[0m]"), additionalLog();

                            result = TokenType::_miscellany;
                        }
                        //areBaseDigitsValid
                        else if (!nc_misc::areBaseDigitsValid(ascii_integer_literal, baseDigitsAsInteger))
                        {
                            m_log.write("In ", literalTypeName_plus_characterWiseType, " literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m, escaped unicode code-point entry contains invalid base(\033[48;2;10;6;26;33;1m", baseDigitsAsStr, "\033[0m) digit(s)");
                            
                            init_newLineStore();
                            
                            auto line_column = calc_line_and_column(integer_start_i);
                            start_log(/*relativeColumn:*/ line_column.second, /*absoluteColumn*/ line_column.second + original_integer_size, line_column.first), spaceLog();

                            m_log.write("   Only digits \033[48;2;10;6;26;32;1m");
                            
                            for (auto i = 0, j = 0; i < baseDigitsAsInteger; ++i)
                            {
                                if (i >= 10)
                                m_log.write(char('A' + j), '(', char('a' + j), ')'), ++j;
                                else
                                m_log.write(i);

                                if (i != baseDigitsAsInteger - 1)
                                m_log.write(',', ' ');
                            }
                            m_log.write("\033[0m are allowed in base(\033[1m", baseDigitsAsStr, "\033[0m)"), additionalLog();

                            result = TokenType::_miscellany;
                        }
                        else
                        {
                            auto convertedMaxCodePoint = baseDigitsAsStr == "16"? "10FFFF" : nc_misc::convertIntegerBase10ToBaseN(0x10FFFF/*1'114'111u*/, baseDigitsAsInteger);
                            
                            if (nc_misc::integer_gt(ascii_integer_literal, convertedMaxCodePoint))
                            {
                                m_log.write("In ", literalTypeName_plus_characterWiseType, " literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m, escaped unicode code-point entry is greater than the maximum unicode code-point");
                                
                                init_newLineStore();
                                
                                auto line_column = calc_line_and_column(integer_start_i);
                                start_log(/*relativeColumn:*/ line_column.second, /*absoluteColumn*/ line_column.second + original_integer_size, line_column.first), spaceLog();

                                if (baseDigitsAsStr == "16")
                                m_log.write("   Maximum unicode code-point: \033[48;2;10;6;26;32;1m10FFFF₁₆\033[0m"), additionalLog();
                                else
                                m_log.write("   Maximum unicode code-point: \033[48;2;10;6;26;32;1m10FFFF₁₆\033[0m or \033[48;2;10;6;26;32;1m", convertedMaxCodePoint, base, "\033[0m"), additionalLog();
                                
                                result = TokenType::_miscellany;
                            }
                            else if (literalTypeName_plus_characterWiseType.contains("character") and (literalType == lt_characterWise_a or literalType == lt_characterWise_p))
                            {
                                if (convertedMaxCodePoint = baseDigitsAsStr == "16"? "7F" : nc_misc::convertIntegerBase10ToBaseN(0x7F/*127u*/, baseDigitsAsInteger), nc_misc::integer_gt(ascii_integer_literal, convertedMaxCodePoint))
                                {
                                    m_log.write("In ", literalTypeName_plus_characterWiseType, " literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m, escaped unicode code point entry is greater than the maximum basic-latin(ascii) code-point");
                                    
                                    init_newLineStore();
                                    
                                    auto line_column = calc_line_and_column(integer_start_i);
                                    start_log(/*relativeColumn:*/ line_column.second, /*absoluteColumn*/ line_column.second + original_integer_size, line_column.first);

                                    if (baseDigitsAsStr == "16")
                                    m_log.write("   Maximum basic-latin(ascii) code-point: \033[48;2;10;6;26;32;1m10FFFF₁₆\033[0m"), additionalLog();
                                    else
                                    m_log.write("   Maximum basic-latin(ascii) code-point: \033[48;2;10;6;26;32;1m10FFFF₁₆\033[0m or \033[48;2;10;6;26;32;1m", convertedMaxCodePoint, base, "\033[0m"), additionalLog();

                                    result = TokenType::_miscellany;
                                }
                            }
                        }

                        /*error if the closing square brace is not found*/
                        closing_square_brace_check:
                        if (m_tokenString[i] != ']'_u8)
                        {
                            m_log.write("In ", literalTypeName_plus_characterWiseType, " literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m, escaped unicode code-point entry is unterminated, expected this \033[48;2;10;6;26;32;1m]\033[0m instead of this \033[48;2;10;6;26;31;1m", m_tokenString[i], "\033[0m");

                            init_newLineStore();
                            
                            auto line_column = calc_line_and_column(i);
                            bool shouldNewLineBeConsidered = m_tokenString[i] == '\n'_u8;

                            start_log(/*relativeColumn:*/ line_column.second, /*absoluteColumn*/ line_column.second + 1, line_column.first, shouldNewLineBeConsidered);
                        }
                        else
                        ++i;
                    }
                    break;
                }
                default:
                {
                    result = TokenType::_miscellany;
                    m_log.write(beginText(), " literal \033[48;2;10;6;26;33;1m", m_tokenString, "\033[0m contains invalid escape squence: \033[48;2;10;6;26;31;1m", m_tokenString[i], "\033[0m");
                    
                    init_newLineStore();
                    
                    auto line_column = calc_line_and_column(i);
                    bool shouldNewLineBeConsidered = m_tokenString[i] == '\n'_u8;
                    
                    start_log(/*relativeColumn:*/ line_column.second, /*absoluteColumn*/ line_column.second + 1, line_column.first, shouldNewLineBeConsidered);
                    spaceLog();

                    if (!print_escapeSequence_list_once_pair.first)
                    {
                        print_escapeSequence_list_once_pair.first = true, print_escapeSequence_list_once_pair.second = m_log.getLogCounter() - 1uz;

                        m_log.write("   Try the below instead:"), additionalLog();
                        m_log.write(R"(   ● [48;2;10;6;26;32;1mb[0m for backspace equivalent to \[8])"), additionalLog();
                        m_log.write(R"(   ● [48;2;10;6;26;32;1me[0m for escape equivalent to \[1B])"), additionalLog();
                        m_log.write(R"(   ● [48;2;10;6;26;32;1mf[0m for formfeed equivalent to \[C])"), additionalLog();
                        m_log.write(R"(   ● [48;2;10;6;26;32;1mn[0m for new-line equivalent to \[A])"), additionalLog();
                        m_log.write(R"(   ● [48;2;10;6;26;32;1mr[0m for carraige-return equivalent to \[D])"), additionalLog();
                        m_log.write(R"(   ● [48;2;10;6;26;32;1mt[0m for horizontal-tabulation equivalent to \[9])"), additionalLog();
                        m_log.write(R"(   ● [48;2;10;6;26;32;1mv[0m for vertical tabulation equivalent to \[B])"), additionalLog();
                        m_log.write(R"(   ● [48;2;10;6;26;32;1m"[0m for quotation-mark(dobule-quote) equivalent to \[22])"), additionalLog();
                        m_log.write(R"(   ● [48;2;10;6;26;32;1m'[0m for apostrophe(single-quote) equivalent to \[27])"), additionalLog();
                        m_log.write(R"(   ● [48;2;10;6;26;32;1m0[0m for null equivalent to \[0])"), additionalLog();
                        m_log.write(R"(   ● [48;2;10;6;26;32;1m[[0m for unicode code-point entry, with its terminator being [48;2;10;6;26;32;1m][0m. The unicode code-point entry follows the same rules as valid nc integer literal, meaning that they are base agnostic and support digit separators with the exception that the default base is [48;2;10;6;26;32;1m16[0m not [48;2;10;6;26;32;1m10[0m, example: [48;2;10;6;26;32;1m\[2a][0m, [48;2;10;6;26;32;1m\[42₁₀][0m, [48;2;10;6;26;32;1m\[1F₂₇][0m, [48;2;10;6;26;32;1m\[10'10'10₂][0m, etc)"), additionalLog();
                        m_log.write(R"(   ● Variable length [48;2;10;6;26;32;1m#[0m followed by [48;2;10;6;26;32;1m{[0m for raw string/character entry(for disabling escape-sequences), with is terminator being [48;2;10;6;26;32;1m}[0m followed by the same [48;2;10;6;26;32;1m#[0m length, example: [48;2;10;6;26;32;1m\#{\p '}#[0m, [48;2;10;6;26;32;1m\##{ }# }##[0m, [48;2;10;6;26;32;1m\#####{ }### }#####[0m, etc. This method of raw string entry was inspired by the rust programming language - i.e this [48;2;10;6;26;32;1mr#"\k""""\p"#[0m - but instead of being standalone, it's embedded)"), additionalLog();
                        m_log.write("   Obviously the usage is this: \\<escape-squence>"), additionalLog();
                    }
                    else
                    m_log.write("   The list of valid nc escape-sequences are already listed in log(\033[48;2;10;6;26;33;1m", print_escapeSequence_list_once_pair.second, "\033[0m) starting from the 5th line"), additionalLog();
                    
                    break;
                }
                }
            }
        }
        
        return result;
    }

    Lexer::TokenType Lexer::tokenizeSymbols()
    {
        recordRelativeColumn(); //do not move

        if (m_fileBuffer[m_filePosition] == sMinus.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sGreaterthan.front())
            m_tokenString.assign(sArrow), nextFilePosAndColumn() /*skip past the sGreaterthan token*/;
            else if (m_fileBuffer[m_filePosition] == sEqual.front())
            m_tokenString.assign(sMinusAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
            else if (m_fileBuffer[m_filePosition] == sMinus.front())
            m_tokenString.assign(sDecrement), nextFilePosAndColumn() /*skip past the second sMinus token*/;
            else
            m_tokenString.assign(sMinus);
        }
        else if (m_fileBuffer[m_filePosition] == sLessthan.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
            m_tokenString.assign(sLessThan_equalTo), nextFilePosAndColumn() /*skip past the sEqual token*/;
            else if (m_fileBuffer[m_filePosition] == sLessthan.front())
            {
                if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sShiftLeftAssign), nextFilePosAndColumn();
                else if (m_fileBuffer[m_filePosition] == sLessthan.front())
                {
                    if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
                    m_tokenString.assign(sRotateLeftAssign), nextFilePosAndColumn();
                    else
                    m_tokenString.assign(sRotateLeft);
                }
                else
                m_tokenString.assign(sShiftLeft);
            }
            else
            m_tokenString.assign(sLessthan);
        }
        else if (m_fileBuffer[m_filePosition] == sGreaterthan.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
            m_tokenString.assign(sGreaterThan_equalTo), nextFilePosAndColumn() /*skip past the sEqual token*/;
            else if (m_fileBuffer[m_filePosition] == sGreaterthan.front())
            {
                if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sShiftRightAssign), nextFilePosAndColumn();
                else if (m_fileBuffer[m_filePosition] == sGreaterthan.front())
                {
                    if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
                    m_tokenString.assign(sRotateRightAssign), nextFilePosAndColumn();
                    else
                    m_tokenString.assign(sRotateRight);
                }
                else
                m_tokenString.assign(sShiftRight);
            }
            else
            m_tokenString.assign(sGreaterthan);
        }
        else if (m_fileBuffer[m_filePosition] == sColon.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
            m_tokenString.assign(sAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
            else if (m_fileBuffer[m_filePosition] == sColon.front())
            {
                if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sInitAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
                else
                m_tokenString.assign(sScopeResolution);
            }
            else
            m_tokenString.assign(sColon);
        }
        else if (m_fileBuffer[m_filePosition] == sNot.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
            m_tokenString.assign(sNotEqual), nextFilePosAndColumn() /*skip past the sEqual token*/;
            else
            m_tokenString.assign(sNot);
        }
        else if (m_fileBuffer[m_filePosition] == sPlus.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
            m_tokenString.assign(sPlusAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
            else if (m_fileBuffer[m_filePosition] == sPlus.front())
            m_tokenString.assign(sIncrement), nextFilePosAndColumn() /*skip past the second sPlus token*/;
            else
            m_tokenString.assign(sPlus);
        }
        else if (m_fileBuffer[m_filePosition] == sAlternateDivide.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
            m_tokenString.assign(sAlternateDivideAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
            else
            m_tokenString.assign(sAlternateDivide);
        }
        else if (m_fileBuffer[m_filePosition] == sMultiply.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
            m_tokenString.assign(sMultiplyAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
            else
            m_tokenString.assign(sMultiply);
        }
        else if (m_fileBuffer[m_filePosition] == sDivide.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
            m_tokenString.assign(sDivideAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
            else if (m_fileBuffer[m_filePosition] == sRemainder.front())
            m_tokenString.assign(sDivideRemainder), nextFilePosAndColumn() /*skip past the sRemainder token*/;
            else
            m_tokenString.assign(sDivide);
        }
        else if (m_fileBuffer[m_filePosition] == sRemainder.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
            m_tokenString.assign(sRemainderAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
            else
            m_tokenString.assign(sRemainder);
        }
        else if (m_fileBuffer[m_filePosition] == sExponention.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sEqual.front())
            m_tokenString.assign(sExponentionAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
            else
            m_tokenString.assign(sExponention);
        }
        else if (m_fileBuffer[m_filePosition] == sEqual.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sMinus.front())
            m_tokenString.assign(sReverseMinusAssign), nextFilePosAndColumn() /*skip past the sMinus token*/;
            else if (m_fileBuffer[m_filePosition] == sDivide.front())
            m_tokenString.assign(sReverseDivideAssign), nextFilePosAndColumn() /*skip past the sDivide token*/;
            else if (m_fileBuffer[m_filePosition] == sRemainder.front())
            m_tokenString.assign(sReverseRemainderAssign), nextFilePosAndColumn() /*skip past the sRemainder token*/;
            else if (m_fileBuffer[m_filePosition] == sAlternateDivide.front())
            m_tokenString.assign(sReverseAlternateDivideAssign), nextFilePosAndColumn() /*skip past the sAlternateDivide token*/;
            else
            m_tokenString.assign(sEqual);
        }
        else if (m_fileBuffer[m_filePosition] == '#')
        {
            if (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == sLparen.front())
            m_tokenString.assign(sHashedLparen), nextFilePosAndColumn();
            else
            return prevFilePosAndColumn(), TokenType::_miscellany;
        }
        else if (
        m_fileBuffer[m_filePosition] == sLcurly.front() or m_fileBuffer[m_filePosition] == sRcurly.front() or
        m_fileBuffer[m_filePosition] == sLparen.front() or m_fileBuffer[m_filePosition] == sRparen.front() or
        m_fileBuffer[m_filePosition] == sSemicolon.front() or m_fileBuffer[m_filePosition] == sComma.front() or
        m_fileBuffer[m_filePosition] == sLsquare.front() or m_fileBuffer[m_filePosition] == sRsquare.front() or
        m_fileBuffer[m_filePosition] == sQuestionMark.front() or
        m_fileBuffer[m_filePosition] == sMemberOf.front() or m_fileBuffer[m_filePosition] == sCommercialAt.front() or
        m_fileBuffer[m_filePosition] == sAlternateLessThan_equalTo.front() or m_fileBuffer[m_filePosition] == sAlternateNotEqual.front() or
        m_fileBuffer[m_filePosition] == sAlternateGreaterThan_equalTo.front() or m_fileBuffer[m_filePosition] == sAmperSand.front() or
        m_fileBuffer[m_filePosition] == sPipe.front() or m_fileBuffer[m_filePosition] == sDollarSign.front()
        )
        m_tokenString.push_back(m_fileBuffer[m_filePosition]), nextFilePosAndColumn() /*move the file position past the symbol*/;
        else
        return TokenType::_miscellany;
        
        addToTokenDataList(std::optional<std::uint32_t>{}, m_line, false, false, {}, true);

        return TokenType::symbol;
    }


    void Lexer::start_log()
    {
        m_log.log(true);
        
        m_log.write("   \033[1mLine\033[0m: \033[48;2;10;6;26;36;1m", m_line, "\033[0m from \033[1mcolumn\033[0m: [\033[48;2;10;6;26;36;1m", m_relativeColumn, "\033[0m - \033[48;2;10;6;26;36;1m", m_absoluteColumn, "\033[0m]"), m_log.log();

        U8string lineContents{};
        std::uint32_t count{};

        for (auto i = m_lineNumToFilePos[m_line], size = (std::uint32_t)m_fileBuffer.size(); i < size; ++i)
        {
            if (m_fileBuffer[i] == '\n'_u8)
            break;

            ++count;
        }
        lineContents.assign(m_fileBuffer, m_lineNumToFilePos[m_line], count);

        m_log.write("   \033[4;1mLine contents\033[0m│\033[48;2;10;6;26;37m" , lineContents, "\033[0m"), m_log.log();

        auto n_of_arrows = misc::safe_unsigned_sub(m_absoluteColumn, m_relativeColumn);
        auto arrowPointers = U8string{lineContents.size(), ' '_u8}.replace(m_relativeColumn - 1uz, n_of_arrows, n_of_arrows, "⇧"_u8);

        m_log.write("                │\033[1m", arrowPointers.erase(arrowPointers.find_last_of("⇧"_u8) + 1), "\033[0m");

        m_log.log();
        //*†↥⇑↑

        m_isDeadZone = true;
    }

    void Lexer::start_log(std::uint32_t relativeColumn, std::uint32_t absoluteColumn, std::uint32_t line, bool consideredNewLine_flag)
    {
        m_log.log(true);
        
        consideredNewLine_flag?
        m_log.write("   \033[1mLine\033[0m: \033[48;2;10;6;26;36;1m", line, "\033[0m \033[1mcolumn\033[0m: \033[48;2;10;6;26;36;1m", relativeColumn, "\033[0m")
        :
        m_log.write("   \033[1mLine\033[0m: \033[48;2;10;6;26;36;1m", line, "\033[0m from \033[1mcolumn\033[0m: [\033[48;2;10;6;26;36;1m", relativeColumn, "\033[0m - \033[48;2;10;6;26;36;1m", absoluteColumn, "\033[0m]");

        m_log.log();

        U8string lineContents{};
        std::uint32_t count{};

        for (auto i = m_lineNumToFilePos[line], size = (std::uint32_t)m_fileBuffer.size(); i < size; ++i)
        {
            if (m_fileBuffer[i] == '\n'_u8)
            {
                if (consideredNewLine_flag)
                ++count;

                break;
            }
            
            ++count;
        }
        lineContents.assign(m_fileBuffer, m_lineNumToFilePos[line], count);

        m_log.write("   \033[4;1mLine contents\033[0m│\033[48;2;10;6;26;37m" , lineContents, "\033[0m"), m_log.log();

        auto n_of_arrows = misc::safe_unsigned_sub(absoluteColumn, relativeColumn);
        auto arrowPointers = U8string{lineContents.size(), ' '_u8}.replace(relativeColumn - 1uz, n_of_arrows, n_of_arrows, "⇧"_u8);

        m_log.write("                │\033[1m", arrowPointers.erase(arrowPointers.find_last_of("⇧"_u8) + 1), "\033[0m");

        m_log.log();
        //*†↥⇑↑

        m_isDeadZone = true;
    }

    void Lexer::start_multiLineLog(std::uint32_t start_line, std::uint32_t relativeColumn)
    {
        m_log.log(true);

        m_log.write("   From [\033[1mline\033[0m: \033[48;2;10;6;26;36;1m", start_line, "\033[0m \033[1mcolumn\033[0m: \033[48;2;10;6;26;36;1m", relativeColumn, "\033[0m] - [\033[1mline\033[0m: \033[48;2;10;6;26;36;1m", m_line, "\033[0m \033[1mcolumn\033[0m: \033[48;2;10;6;26;36;1m", m_absoluteColumn, "\033[0m]");
        m_log.log();

        U8string lineContents{};
        
        for (auto current_line = start_line; current_line <= m_line; ++current_line)
        {
            std::uint32_t count{};
            for (auto i = m_lineNumToFilePos[current_line], size = (std::uint32_t)m_fileBuffer.size(); i < size; ++i)
            {
                if (m_fileBuffer[i] == '\n'_u8)
                break;

                ++count;
            }
            lineContents.assign(m_fileBuffer, m_lineNumToFilePos[current_line], count);

            m_log.write("   \033[4;1mLine contents\033[0m│\033[48;2;10;6;26;37m" , lineContents, "\033[0m"), m_log.log();
            m_log.write("                │\033[1m");

            if (current_line == start_line)//when it is the begining
            {
                auto temp = lineContents.size() - (std::size_t)(relativeColumn - 1);
                m_log.write(U8string{lineContents.size(), ' '_u8}.replace(relativeColumn - 1, temp, temp, "⇧"_u8));
            }
            //no need to check for when it is end because i only call the function for unterminated multiLine entities like multi-comments and strings
            else
            {
                auto size = lineContents.size();
                m_log.write(U8string{size, ' '_u8}.replace(0uz, size, size, "⇧"_u8));
            }

            m_log.write("\033[0m"), m_log.log();
        }

        m_isDeadZone = true;
    }

    void Lexer::spaceLog()
    { m_log.log(); }

    void Lexer::additionalLog()
    { m_log.log(); }

    void Lexer::errorOutput()
    { m_log.outputLogEntries("\002(10)\033[5;1mLexer Log\033[0m", io::cerr), panic(); }

    std::ostream& operator<<(std::ostream& out, Lexer::TokenType temp)
    {
        switch (temp)
        {
        case Lexer::TokenType::reserved:
            return out << "reserved";
        case Lexer::TokenType::identifier:
            return out << "identifier";
        case Lexer::TokenType::literal:
            return out << "literal";
        case Lexer::TokenType::symbol:
            return out << "symbol";
        case Lexer::TokenType::_miscellany:
            return out << "NULL";
        case Lexer::TokenType::_eot:
            return out << "<eot>";
        default:
            throw std::invalid_argument{"parameter temp contains an invalid Lexer::TokenType enumerator"};
        }
    }
    
}// namespace Nc