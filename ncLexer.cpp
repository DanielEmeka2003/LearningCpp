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
#include "myfunctions1.h"
#include "stream/fileoutputstream.h"
#include "stream/fileinputstream.h"
#include "boost/multiprecision/cpp_dec_float.hpp"

namespace Nc
{
    Lexer::Lexer(Myfcn::U8string& fileBuffer): m_fileBuffer{ std::move(fileBuffer) }{}

    Lexer::Lexer(Myfcn::U8string&& fileBuffer): m_fileBuffer{ std::move(fileBuffer) }{}

    void Lexer::lexFileBuffer()
    {
        while (lexify() != TokenType::_eot){}

        if (!m_log.isLogEmpty()) errorOutput();
    }
    
    std::vector<Lexer::TokenData>& Lexer::getTokenList()
    { return m_tokenDataList; }

    const std::vector<Lexer::TokenData>& Lexer::getTokenList() const
    { return m_tokenDataList; }
    
    Lexer::LineNumerToFilePositionMap& Lexer::getLineNumToFilePos()
    { return m_lineNumToFilePos; }
    const Lexer::LineNumerToFilePositionMap& Lexer::getLineNumToFilePos() const
    { return m_lineNumToFilePos; }
    Myfcn::U8string& Lexer::getFileBuffer()
    { return m_fileBuffer; }
    const Myfcn::U8string& Lexer::getFileBuffer() const
    { return m_fileBuffer; }

    void Lexer::logBoxStyle(Log::BoxLineStyle s)
    { m_log.changeBoxLineStyle(s); }


    Lexer::TokenType Lexer::lexify()
    {
        if (m_isEndofFile)
        return TokenType::_eot;

        auto acceptableUnicode = [](const Myfcn::U8char& x)
        {
            if (
            x.isSpacingModifierLetters() or x.isCombiningDiacriticalMarks() or x.isCombiningDiacriticalMarksExtended() or x.isCombiningDiacriticalMarksSupplement() or
            x.isGeneralPunctuation() or x.isSuperscriptsAndSubscripts() or x.isCurrencySymbols() or x.isCombiningDiacriticalMarksForSymbols() or x.isArrows() or
            x.isMathematicalOperators() or x.isMiscellaneousTechnical() or x.isControlPictures() or x.isOpticalCharacterRecognition() or x.isBoxDrawing() or 
            x.isBlockElements() or x.isGeometricShapes() or x.isMiscellaneousMathematicalSymbolsA() or x.isSupplementalArrowsA() or x.isBraillePatterns() or
            x.isSupplementalArrowsB() or x.isMiscellaneousMathematicalSymbolsB() or x.isSupplementalMathematicalOperators() or x.isMiscellaneousSymbolsAndArrows() or
            x.isCombiningHalfMarks() or x.isIdeographicSymbolsAndPunctuation() or x.isSupplementalArrowsC()
            )
            return false;
            else
            {
                if (x.isBasicLatin())
                {
                    if (x.isBasicLatinAlphabet() or x == '_'_u8)
                    return true;
                    else
                    return false;
                }
                return true;
            }
            
            //just here so the compiler won't complain
            return true;
        };

        //clear the token string before a lexing operation begins
        m_tokenString.clear();

        while (m_fileBuffer[m_filePosition].isBasicLatinWhiteSpace())
        {
            if (m_fileBuffer[m_filePosition] == '\n'_u8)
            newLineFound();

            nextFilePosAndColumn();

            if (m_isEndofFile)
            return TokenType::_eot;
        }
        
        if (acceptableUnicode(m_fileBuffer[m_filePosition]))
        return tokenizeWords();
        else if (m_fileBuffer[m_filePosition] == '\''_u8)
        return tokenizeCharLiterals();
        else if (m_fileBuffer[m_filePosition] == '"'_u8)
        return tokenizeStrLiterals();
        else if (m_fileBuffer[m_filePosition].isBasicLatinDigit())
        return tokenizeNumbers();
        else
        {
            if (tokenizeSymbols() == TokenType::_miscellany)
            {
                //check for comments
                auto tokenType = singleLineComment();

                if (tokenType == TokenType::_miscellany)
                tokenType = multiLineComment();

                if (tokenType == TokenType::_comment)
                return TokenType::_comment;
                else if (tokenType == TokenType::_eot)
                return TokenType::_eot;
                
                //keep the unrecongizned token for error purposes
                m_tokenString.push_back(m_fileBuffer[m_filePosition]);
                nextFilePosAndColumn(); //move the file position past the symbol

                m_log.writews("unrecognized token [", m_tokenString, "]"), start_Log();
                spaceLog();
                m_log.writews("  unicode & utf8 code point⟦", m_tokenString.front().convertToUnicodeCodePoint(), '&', m_tokenString.front().convertToUtf8CodePoint(), "⟧");
                additionalLog();

                return TokenType::_miscellany;
            }
            else return TokenType::symbol;
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
    {
        m_relativeColumn = m_absoluteColumn;
    }

    void Lexer::addToTokenDataList(Myfcn::U8string& tokenString, std::optional<std::uint32_t> optLine, std::uint32_t line, std::uint32_t absoluteColumn, std::uint32_t relativeColumn, bool isReserved, bool isIdentifier, Myfcn::U8string literalType, bool isSymbol)
    {
        if (m_isDeadZone)
        return;
        
        //add the token to the list
        m_tokenDataList.push_back(TokenData{
            .token{ std::move(tokenString) },
            .optLine{ optLine },
            .line{ line },
            .absoluteColumn{ absoluteColumn },
            .relativeColumn{ relativeColumn },
            .isReserved{ isReserved }, 
            .isIdentifier{ isIdentifier },
            .literalType{ std::move(literalType) },
            .isSymbol{ isSymbol }
        });
    }

    Lexer::TokenType Lexer::singleLineComment()
    {
        if (m_fileBuffer[m_filePosition] == '`'_u8)
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == '`'_u8)
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
        }
        return TokenType::_miscellany;
    }

    Lexer::TokenType Lexer::multiLineComment()
    {
        if (m_fileBuffer[m_filePosition] == '`'_u8)
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == '{'_u8)
                {   
                    auto savedAbsoluteColumn = m_absoluteColumn, savedLine = m_line; //for error purposes
                    while (true)
                    {
                        nextFilePosAndColumn(), multiLineComment()/*to support of multi-line nesting*/;

                        if (m_isEndofFile)
                        {
                            m_log.write("unterminated mutiline comment, expected this [ }` ] before EOF(end-of-file)"), start_MultiLineCommentLog();
                            spaceLog();
                            m_log.writews("  [NOTE] the multiline comment started on line:", savedLine, "column:", savedAbsoluteColumn-1, '-', savedAbsoluteColumn);
                            additionalLog();

                            return TokenType::_eot;
                        }
                        else if (m_fileBuffer[m_filePosition] == '}'_u8 and (nextFilePosAndColumn(), m_fileBuffer[m_filePosition] == '`'_u8 and !m_isEndofFile))
                        {
                            nextFilePosAndColumn(); //for moving past symbol[ ` ]
                            return TokenType::_comment;
                        }
                        else if (!m_isEndofFile and m_fileBuffer[m_filePosition] == '\n'_u8)//it is directly under the second conditionally for a reason
                        newLineFound();
                    }
                }
                else prevFilePosAndColumn();
            }
        }
        return TokenType::_miscellany;
    }


    Lexer::TokenType Lexer::tokenizeWords()
    {
        auto acceptableUnicode = [](const Myfcn::U8char& x)
        {
            if (
            x.isSpacingModifierLetters() or x.isCombiningDiacriticalMarks() or x.isCombiningDiacriticalMarksExtended() or x.isCombiningDiacriticalMarksSupplement() or
            x.isGeneralPunctuation() or x.isSuperscriptsAndSubscripts() or x.isCurrencySymbols() or x.isCombiningDiacriticalMarksForSymbols() or x.isArrows() or
            x.isMathematicalOperators() or x.isMiscellaneousTechnical() or x.isControlPictures() or x.isOpticalCharacterRecognition() or x.isBoxDrawing() or 
            x.isBlockElements() or x.isGeometricShapes() or x.isMiscellaneousMathematicalSymbolsA() or x.isSupplementalArrowsA() or x.isBraillePatterns() or
            x.isSupplementalArrowsB() or x.isMiscellaneousMathematicalSymbolsB() or x.isSupplementalMathematicalOperators() or x.isMiscellaneousSymbolsAndArrows() or
            x.isCombiningHalfMarks() or x.isIdeographicSymbolsAndPunctuation() or x.isSupplementalArrowsC()
            )
            return false;
            else
            {
                if (x.isBasicLatin())
                {
                    if (x.isBasicLatinAlphabetNumeral() or x == '_'_u8)
                    return true;
                    else
                    return false;
                }
                return true;
            }
            
            //just here so the compiler won't complain
            return true;
        };

        auto formerFilePos{m_filePosition};
        recordRelativeColumn(); //do not move from here
        
        while(!m_isEndofFile and acceptableUnicode(m_fileBuffer[m_filePosition]))
        nextFilePosAndColumn();
        
        //extract the token to m_tokenString
        for (size_t i = formerFilePos; m_isEndofFile? i <= m_filePosition : i < m_filePosition; ++i)
        m_tokenString.push_back(m_fileBuffer[i]);

        if (!m_isEndofFile)
        {
            auto literalType = m_tokenString;

            for (auto &&i : literalType)//lowercase the pontential character literal type
            i.basicLatinToLowerCase();

            if (m_fileBuffer[m_filePosition] == '\''_u8)
            {
                return tokenizeCharLiterals(&literalType);
            }
            else if (m_fileBuffer[m_filePosition] == '"'_u8)
            {
                return tokenizeStrLiterals(&literalType);
            }
        }
        
        if (TokenType result{reservedWords()}; result != TokenType::_miscellany)
        return result;

        addToTokenDataList(m_tokenString, std::optional<std::uint32_t>{}, m_line, m_absoluteColumn, m_relativeColumn, false, true);

        return TokenType::identifier;
    }
    
    Lexer::TokenType Lexer::reservedWords()
    {
        if
        (
            m_tokenString == rInt8 or m_tokenString == rInt16 or m_tokenString == rInt32 or m_tokenString == rInt64 or m_tokenString == rInt128 or
            m_tokenString == rInt256 or m_tokenString ==  rInt512 or m_tokenString == rInt1024 or m_tokenString == rUint8 or m_tokenString == rUint16 or
            m_tokenString == rUint32 or m_tokenString == rUint64 or m_tokenString == rUint128 or m_tokenString == rUint256 or m_tokenString ==  rUint512 or
            m_tokenString == rUint1024 or m_tokenString == rBool or m_tokenString == rAchar or m_tokenString == rPchar or m_tokenString == rU8char or
            m_tokenString == rAstring or m_tokenString == rU8string or m_tokenString == rPstring or m_tokenString == rBin16fp or m_tokenString == rBin32fp or
            m_tokenString == rBin64fp or m_tokenString == rBin128fp or m_tokenString == rDec32fp or m_tokenString == rDec64fp or m_tokenString == rDec128fp or
            m_tokenString == rDec256fp or m_tokenString == rDec512fp or m_tokenString == rVoid or m_tokenString == rNullptrType or m_tokenString == rNoneType or
            m_tokenString == rAnyTypeList or m_tokenString == rNcType or m_tokenString == rAnyType or m_tokenString == rTypeinfo or m_tokenString == rGive or
            m_tokenString == rBreak or m_tokenString == rContinue or m_tokenString == rIf or m_tokenString == rElse or m_tokenString == rDo or
            m_tokenString == rWhile or m_tokenString == rFor or m_tokenString == rStatic or m_tokenString == rAnd or m_tokenString == rOr or
            m_tokenString == rNot or m_tokenString == rXor or m_tokenString == rBitwiseAnd or m_tokenString == rBitwiseOr or m_tokenString == rBitwiseNot or
            m_tokenString == rBitwiseXor or m_tokenString == rOdd or m_tokenString == rOutput or m_tokenString == rInput or m_tokenString == rSizeof or
            m_tokenString == rMem or m_tokenString == rGuess or m_tokenString == rTypecast or m_tokenString == rBitcast or m_tokenString == rDefault or
            m_tokenString == rSumType or m_tokenString == rProductType or m_tokenString == rEnum or m_tokenString == rNamespace or m_tokenString == rType_alias or
            m_tokenString == rNamespace_alias
        )
        {
            addToTokenDataList(m_tokenString, std::optional<std::uint32_t>{}, m_line, m_absoluteColumn, m_relativeColumn, true);
            return TokenType::reserved;
        }
        else if (m_tokenString == rTrue or m_tokenString == rFalse)
        {
            addToTokenDataList(m_tokenString, std::optional<std::uint32_t>{}, m_line, m_absoluteColumn, m_relativeColumn, false, false, lbool, false);
            return TokenType::literal;
        }
        else if (m_tokenString == rNullptr)
        {
            addToTokenDataList(m_tokenString, std::optional<std::uint32_t>{}, m_line, m_absoluteColumn, m_relativeColumn, false, false, lnullptr, false);
            return TokenType::literal;
        }
        else if (m_tokenString == rNone)
        {
            addToTokenDataList(m_tokenString, std::optional<std::uint32_t>{}, m_line, m_absoluteColumn, m_relativeColumn, false, false, lnone, false);
            return TokenType::literal;
        }

        return TokenType::_miscellany;
    }

    
    Lexer::TokenType Lexer::tokenizeNumbers()
    {
        auto formerFilePos{m_filePosition}; //maintain intial index
        recordRelativeColumn(); //do not move

        while (!m_isEndofFile and (m_fileBuffer[m_filePosition].isBasicLatinAlphabetNumeral() or m_fileBuffer[m_filePosition] == '\''_u8 or m_fileBuffer[m_filePosition] == '.'_u8))
        nextFilePosAndColumn();
        
        //extract the token to m_tokenString
        for (size_t i = formerFilePos; m_isEndofFile? i <= m_filePosition : i < m_filePosition; ++i)
        m_tokenString.push_back(m_fileBuffer[i]);

        if (m_tokenString.contains('\''_u8))
        {
            if (m_tokenString.ends_with('\''_u8))
            m_log.write("digit seperator [ ' ] should not end a number literal"), start_Log();
            
            // remove the number seperator
            auto newEnd = std::remove(m_tokenString.begin(), m_tokenString.end(), '\'');
            m_tokenString.erase(newEnd, m_tokenString.end());
        }
        
        auto savedTokenString = m_tokenString;
        m_tokenString.clear();
        
        Myfcn::U8string baseAndLiteralType{};
        std::optional<TokenData> optionalTokenData{};
        std::uint32_t savedAbsoluteColumn{ m_absoluteColumn };
        std::uint32_t savedRelativeColumn{ m_relativeColumn };
        bool extract_exponent_once = false;

        while (true)
        {
            if (!extract_exponent_once and m_fileBuffer[m_filePosition] == "⋿"_u8)
            {
                if (nextFilePosAndColumn(), m_fileBuffer[m_filePosition] == '+'_u8)
                baseAndLiteralType.append("⋿+"_u8str), nextFilePosAndColumn(), savedAbsoluteColumn = m_absoluteColumn;
                else if (m_fileBuffer[m_filePosition] == '-'_u8)
                baseAndLiteralType.append("⋿-"_u8str), nextFilePosAndColumn(), savedAbsoluteColumn = m_absoluteColumn;
                else
                baseAndLiteralType.push_back("⋿"_u8), savedAbsoluteColumn = m_absoluteColumn;

                extract_exponent_once = true;
            }

            if (m_isEndofFile or m_fileBuffer[m_filePosition].isBasicLatinWhiteSpace()) //the position of this loop terminating code is strategic, do not move
            break;

            if (auto result = tokenizeSymbols(); result == TokenType::symbol)
            {
                if (!m_isDeadZone) //if the deadZone flag is not set, remove the parsed symbol from the list, because we haven't added the possible number token to the list
                optionalTokenData.emplace(m_tokenDataList.back()), m_tokenDataList.pop_back();

                break;
            }

            baseAndLiteralType.push_back(m_fileBuffer[m_filePosition]), nextFilePosAndColumn(), savedAbsoluteColumn = m_absoluteColumn;
        }
        
        m_tokenString = savedTokenString, m_relativeColumn = savedRelativeColumn;
        
        auto split = [&baseAndLiteralType]
        {
            if (auto result = std::ranges::find(baseAndLiteralType, '`'_u8); result != baseAndLiteralType.end())
            return std::pair{Myfcn::U8string{baseAndLiteralType.begin(), result}, Myfcn::U8string{result+1, baseAndLiteralType.end()}};
            else
            return std::pair{baseAndLiteralType, Myfcn::U8string{}};

            return std::pair{Myfcn::U8string{}, Myfcn::U8string{}};
        };
        
        auto [base, literalType] = split();

        std::swap(savedAbsoluteColumn, m_absoluteColumn); //perform a swap, which is dum

        if (m_tokenString.contains('.'_u8) or base.contains("⋿"_u8))
        validateRealNumberLiteral(base, literalType);
        else
        validateIntLiteral(base, literalType);
        
        std::swap(savedAbsoluteColumn, m_absoluteColumn); //perform a reswap, which is very dum

        addToTokenDataList(m_tokenString, std::optional<std::uint32_t>{}, m_line, savedAbsoluteColumn, savedRelativeColumn, false, false, literalType, false);

        if (optionalTokenData.has_value())
        m_tokenDataList.push_back(optionalTokenData.value());

        return TokenType::literal;
    }

    void Lexer::validateIntLiteral(Myfcn::U8string& base, Myfcn::U8string& literalType)
    {
        if (base.empty())
        base = "₁₀"_u8str;

        auto isUnicodeBaseValid = [&base]
        {
            return std::ranges::all_of(base, [](Myfcn::U8char& x){

                std::string unicode{ x.convertToUnicodeCodePoint() };
                return Myfcn::ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2080", 16) and Myfcn::ManipulateStrAsBaseN::lessThanOrEqual(unicode, "2089", 16);
            }
            );
        };
        
        auto alteredBase = [&base]
        {
            std::map<Myfcn::U8char, char> pseudoBaseMap{
                {"₀"_u8, '0'}, {"₁"_u8, '1'}, {"₂"_u8, '2'}, {"₃"_u8, '3'}, {"₄"_u8, '4'}, {"₅"_u8, '5'}, {"₆"_u8, '6'}, {"₇"_u8, '7'}, {"₈"_u8, '8'}, {"₉"_u8, '9'}
            };
            std::string real_numbers{};
            for (auto &&i : base)
            real_numbers.push_back(pseudoBaseMap[i]);

            std::size_t count_of_leading_zeros{};
            if (real_numbers.starts_with('0'))
            {
                for (size_t i = 0; i < real_numbers.size(); i++)
                {
                    if (real_numbers[i] == '0' and i != real_numbers.size() - 1)
                    ++count_of_leading_zeros;
                }
            }
            
            if (count_of_leading_zeros != 0)
            real_numbers.erase(real_numbers.begin(), real_numbers.begin() + count_of_leading_zeros);

            return real_numbers;
        }();

        auto isBaseValid = [&alteredBase]
        {
            return !(Myfcn::ManipulateStrAsBaseN::equal(alteredBase, "0", 10) or Myfcn::ManipulateStrAsBaseN::greaterThan(alteredBase, "36", 10));
        };

        auto areBaseDigitsValid = [&]
        {
            return Myfcn::ManipulateStrAsBaseN::isValidBaseDigits(convertU8StrToAsciiStr(m_tokenString), static_cast<std::uint8_t>(std::stoi(alteredBase)));
        };

        auto integerLiteralToBase10 = [&]
        {
            if (alteredBase != "10")
            {
                return
                Myfcn::convertAsciiStrToU8Str(Myfcn::ManipulateStrAsBaseN::convertBaseNToBase10(convertU8StrToAsciiStr(m_tokenString), std::uint8_t(std::stoi(alteredBase))));
            }
            return m_tokenString;
        };

        auto integerLiteralToBaseN = [&](const Myfcn::U8string& str)
        {
            if (alteredBase == "10")
            return m_tokenString;

            return
            Myfcn::convertAsciiStrToU8Str(Myfcn::ManipulateStrAsBaseN::convertBase10ToBaseN(convertU8StrToAsciiStr(str), std::uint8_t(std::stoi(alteredBase))));
        };

        auto is_lessThanEqualTo = [&](const Myfcn::U8string& literal, const Myfcn::U8string& integerMax)
        {
            return Myfcn::ManipulateStrAsBaseN::lessThanOrEqual(Myfcn::convertU8StrToAsciiStr(literal), Myfcn::convertU8StrToAsciiStr(integerMax), 10);
        };

        [&literalType]{ for (auto &&i : literalType) i.basicLatinToLowerCase(); }();

        if (isUnicodeBaseValid())
        {
            if (!isBaseValid())
            {
                m_log.write("integer-number literal [ ", m_tokenString, " ] is tagged with a base that is not supported in NC; the base in question (", alteredBase, ')');
                start_Log();
                spaceLog();
                m_log.write("  only bases 1 - 36 are supported in NC; base options[₁₂₃₄₅₆₇₈₉]"), additionalLog();
                return;
            }
            else if (!areBaseDigitsValid())
            {
                m_log.write("integer-number literal [ ", m_tokenString, " ] contains invalid base(", alteredBase, ") digit(s)"), start_Log();
                spaceLog();
                m_log.write("  only digits [");

                auto alteredBaseAsInt = std::stoi(alteredBase);
                for (auto i = 0, j = 0; i < alteredBaseAsInt; ++i)
                {
                    if (i >= 10)
                    m_log.write(char('A' + j), '(', char('a' + j), ')'), ++j;
                    else
                    m_log.write(i);

                    if (i != alteredBaseAsInt - 1)
                    m_log.write(',', ' ');
                }
                m_log.write("] are allowed in base(", alteredBase, ')'), additionalLog();
                return;
            }
        }
        else
        {
            m_log.write("integer-number literal [ ", m_tokenString, " ] is tagged with an invalid base; the invalid base in question (", base, ")"), start_Log();
            spaceLog();
            m_log.write("  try any valid combination of these[₁₂₃₄₅₆₇₈₉] instead; [NOTE] the valid combinations being from ₁ to ₃₆"), additionalLog();
            return;
        }
        
        auto literalToBase10 = integerLiteralToBase10();
        m_tokenString += base; //prepares the token string by appending it's base to it. It is important not to move this, because function integerLiteralToBase10() depends on the token string being without it's base

        if (literalType.empty())
        {
            if (is_lessThanEqualTo(literalToBase10, int32Max))
            literalType = li32;
            else if (is_lessThanEqualTo(literalToBase10, uint32Max))
            literalType = lui32;
            else if (is_lessThanEqualTo(literalToBase10, int64Max))
            literalType = li64;
            else if (is_lessThanEqualTo(literalToBase10, uint64Max))
            literalType = lui64;
            else if (is_lessThanEqualTo(literalToBase10, int128Max))
            literalType = li128;
            else if (is_lessThanEqualTo(literalToBase10, uint128Max))
            literalType = lui128;
            else if (is_lessThanEqualTo(literalToBase10, int256Max))
            literalType = li256;
            else if (is_lessThanEqualTo(literalToBase10, uint256Max))
            literalType = lui256;
            else if (is_lessThanEqualTo(literalToBase10, int512Max))
            literalType = li512;
            else if (is_lessThanEqualTo(literalToBase10, uint512Max))
            literalType = lui512;
            else if (is_lessThanEqualTo(literalToBase10, int1024Max))
            literalType = li1024;
            else if (is_lessThanEqualTo(literalToBase10, uint1024Max))
            literalType = lui1024;
            else
            {
                m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum integer value of NC which is uint1024"), start_Log();
                spaceLog();
                if (alteredBase == "10")
                m_log.writews("  1024bit unsigned integer maximum value: (2¹⁰²⁴ - 1) or", uint1024Max), additionalLog();
                else
                {
                    m_log.write("  1024bit unsigned integer maximum value: (2¹⁰²⁴ - 1)₁₀ i.e ", uint1024Max, "₁₀ or ", integerLiteralToBaseN(uint1024Max), base);
                    additionalLog();
                    m_log.write("  here's the base(10) equivalent of the literal by the way: ", literalToBase10), additionalLog();
                }
            }
        }
        else
        {
            if (literalType == li8)
            {
                if (!is_lessThanEqualTo(literalToBase10, int8Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum positive value of a 8bit signed integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  8bit signed integer maximum positive value: (2⁸⁻¹ - 1) or", int8Max), additionalLog();
                    else
                    {
                        m_log.write("  8bit signed integer maximum positive value: (2⁸⁻¹ - 1)₁₀ or ", int8Max, "₁₀ or ", integerLiteralToBaseN(int8Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the integer literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == li16)
            {
                if (!is_lessThanEqualTo(literalToBase10, int16Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum positive value of a 16bit signed integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  16bit signed integer maximum positive value: (2¹⁶⁻¹ - 1) or", int16Max), additionalLog();
                    else
                    {
                        m_log.write("  16bit signed integer maximum positive value: (2¹⁶⁻¹ - 1)₁₀ or ", int16Max, "₁₀ or ", integerLiteralToBaseN(int16Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the integer literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == li32)
            {
                if (!is_lessThanEqualTo(literalToBase10, int32Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum positive value of a 32bit signed integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  32bit signed integer maximum positive value: (2³²⁻¹ - 1) or", int32Max), additionalLog();
                    else
                    {
                        m_log.write("  32bit signed integer maximum positive value: (2³²⁻¹ - 1)₁₀ or ", int32Max, "₁₀ or ", integerLiteralToBaseN(int32Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the integer literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == li64)
            {
                if (!is_lessThanEqualTo(literalToBase10, int64Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum positive value of a 64bit signed integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  64bit signed integer maximum positive value: (2⁶⁴⁻¹ - 1) or", int64Max), additionalLog();
                    else
                    {
                        m_log.write("  64bit signed integer maximum positive value: (2⁶⁴⁻¹ - 1)₁₀ or ", int64Max, "₁₀ or ", integerLiteralToBaseN(int64Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the integer literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == li128)
            {
                if (!is_lessThanEqualTo(literalToBase10, int128Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum positive value of a 128bit signed integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  128bit signed integer maximum positive value: (2¹²⁸⁻¹ - 1) or", int128Max), additionalLog();
                    else
                    {
                        m_log.write("  128bit signed integer maximum positive value: (2¹²⁸⁻¹ - 1)₁₀ or ", int128Max, "₁₀ or ", integerLiteralToBaseN(int128Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the integer literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == li256)
            {
                if (!is_lessThanEqualTo(literalToBase10, int256Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum positive value of a 256bit signed integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  256bit signed integer maximum positive value: (2²⁵⁶⁻¹ - 1) or", int256Max), additionalLog();
                    else
                    {
                        m_log.write("  256bit signed integer maximum positive value: (2²⁵⁶⁻¹ - 1)₁₀ or ", int256Max, "₁₀ or ", integerLiteralToBaseN(int256Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the integer literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == li512)
            {
                if (!is_lessThanEqualTo(literalToBase10, int512Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum positive value of a 512bit signed integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  512bit signed integer maximum positive value: (2⁵¹²⁻¹ - 1) or", int512Max), additionalLog();
                    else
                    {
                        m_log.write("  512bit signed integer maximum positive value: (2⁵¹²⁻¹ - 1)₁₀ or ", int512Max, "₁₀ or ", integerLiteralToBaseN(int512Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == li1024)
            {
                if (!is_lessThanEqualTo(literalToBase10, int1024Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum positive value of a 1024bit signed integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  1024bit signed integer maximum positive value: (2¹⁰²⁴⁻¹ - 1) or", int1024Max), additionalLog();
                    else
                    {
                        m_log.write("  1024bit signed integer maximum positive value: (2¹⁰²⁴⁻¹ - 1)₁₀ or ", int1024Max, "₁₀ or ", integerLiteralToBaseN(int1024Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == lui8)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint8Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum value of an 8bit unsigned integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  8bit unsigned integer maximum value: (2⁸ - 1) or", uint8Max), additionalLog();
                    else
                    {
                        m_log.write("  8bit unsigned integer maximum value: (2⁸ - 1)₁₀ or ", uint8Max, "₁₀ or ", integerLiteralToBaseN(uint8Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == lui16)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint16Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum value of a 16bit unsigned integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  16bit unsigned integer maximum value: (2¹⁶ - 1) or", uint16Max), additionalLog();
                    else
                    {
                        m_log.write("  16bit unsigned integer maximum value: (2¹⁶ - 1)₁₀ or ", uint16Max, "₁₀ or ", integerLiteralToBaseN(uint16Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == lui32)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint32Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum value of a 32bit unsigned integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  32bit unsigned integer maximum value: (2³² - 1) or", uint32Max), additionalLog();
                    else
                    {
                        m_log.write("  32bit unsigned integer maximum value: (2³² - 1)₁₀ or ", uint32Max, "₁₀ or ", integerLiteralToBaseN(uint32Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == lui64)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint64Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum value of a 64bit unsigned integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  64bit unsigned integer maximum value: (2⁶⁴ - 1) or", uint64Max), additionalLog();
                    else
                    {
                        m_log.write("  64bit unsigned integer maximum value: (2⁶⁴ - 1)₁₀ or ", uint64Max, "₁₀ or ", integerLiteralToBaseN(uint64Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == lui128)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint128Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum value of a 128bit unsigned integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  128bit unsigned integer maximum value: (2¹²⁸ - 1) or", uint128Max), additionalLog();
                    else
                    {
                        m_log.write("  128bit unsigned integer maximum value: (2¹²⁸ - 1)₁₀ or ", uint128Max, "₁₀ or ", integerLiteralToBaseN(uint128Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == lui256)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint256Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum value of a 256bit unsigned integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  256bit unsigned integer maximum value: (2²⁵⁶ - 1) or", uint256Max), additionalLog();
                    else
                    {
                        m_log.write("  256bit unsigned integer maximum value: (2²⁵⁶ - 1)₁₀ or ", uint256Max, "₁₀ or ", integerLiteralToBaseN(uint256Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == lui512)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint512Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum value of a 512bit unsigned integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  512bit unsigned integer maximum value: (2⁵¹² - 1) or", uint512Max), additionalLog();
                    else
                    {
                        m_log.write("  512bit unsigned integer maximum value: (2⁵¹² - 1)₁₀ or ", uint512Max, "₁₀ or ", integerLiteralToBaseN(uint512Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else if (literalType == lui1024)
            {
                if (!is_lessThanEqualTo(literalToBase10, uint1024Max))
                {
                    m_log.writews("integer-number literal [", m_tokenString, "] is greater than the maximum value of a 1024bit unsigned integer"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.writews("  1024bit unsigned integer maximum value: (2¹⁰²⁴ - 1) or", uint1024Max), additionalLog();
                    else
                    {
                        m_log.write("integer-number literal [ ", m_tokenString, " ] is not within range of (2¹⁰²⁴ - 1)₁₀ i.e ", uint1024Max, "₁₀ or ", integerLiteralToBaseN(uint1024Max), base);
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the literal by the way: ", literalToBase10), additionalLog();
                    }
                }
            }
            else
            {
                if (literalType == lb16 or literalType == lb32 or literalType == lb128 or literalType == ld32 or literalType == ld64 or literalType == ld128 or literalType == ld256 or literalType == ld512)
                m_log.writews("integer-number literal [", m_tokenString, "] is tagged with a real-number literal instead of an integer literal [", literalType, ']');
                else
                m_log.writews("integer-number literal [", m_tokenString, "] is tagged with an unrecognized integer literal type [", literalType, ']');

                start_Log(), spaceLog();

                m_log.write("  try the below instead:"), additionalLog();
                m_log.write("  ● [i8, i16, i32, i64, i128, i256, i512 or i1024] for signed integers"), additionalLog();
                m_log.write("  ● [ui8, ui16, ui32, ui64, ui128, ui256, ui512 or ui1024] for unsigned integers"), additionalLog();
                m_log.write("  they are all case insensitive of course"), additionalLog();
            }
        }
    }
    
    void Lexer::validateRealNumberLiteral(Myfcn::U8string& base_exp, Myfcn::U8string& literalType)
    {
        if (m_tokenString.contains('.'_u8))
        {
            if (std::ranges::count(m_tokenString, '.') != 1)
            {
                m_log.writews("a real-number literal should not contain multiple radix seperators"), start_Log();
                spaceLog();
                m_log.write("  remove them!"), additionalLog();
                return;
            }
            else if (m_tokenString.ends_with('.'_u8))
            {
                m_log.writews("radix seperator [ . ] should not end a real-number literal, one or more valid base digits are meant to follow"), start_Log();
                return;
            }
        }

        Myfcn::U8string base{}, exponent{};
        //continue from here
        [[maybe_unused]] bool isExponentNegative{};
        if (auto found = std::ranges::find(base_exp, "⋿"_u8); found != base_exp.end())
        {
            exponent = {found + 1, base_exp.end()};

            //check if the exponent has a sign
            if (exponent.starts_with('+'_u8))
            exponent.erase(exponent.begin());
            else if (exponent.starts_with('-'_u8))
            exponent.erase(exponent.begin()), isExponentNegative = true;

            //variable found points to the character [⋿] such that an increment by one points to the character after [⋿]
            if(exponent.empty()) //we should log an error when the exponent indicator is specified but no exponent is found
            m_log.write("the exponent indicator(⋿) was specified but no exponent was found"), start_Log();
            
            if (found == base_exp.begin())
            base = "₁₀"_u8str;
            else
            base = {base_exp.begin(), found};
        }
        else
        {
            if (base_exp.empty())
            base = "₁₀"_u8str;
            else
            base = base_exp;
        }
        //the code above invloves extracting the exponent from the base, that's why the firsr parameter is called [base_exp]

        auto isUnicodeBaseValid = [&base]
        {
            return std::ranges::all_of(base, [](Myfcn::U8char& x){

                std::string unicode{ x.convertToUnicodeCodePoint() };
                return Myfcn::ManipulateStrAsBaseN::greaterThanOrEqual(unicode, "2080", 16) and Myfcn::ManipulateStrAsBaseN::lessThanOrEqual(unicode, "2089", 16);
            }
            );
        };
        
        auto alteredBase = [&base]
        {
            std::map<Myfcn::U8char, char> pseudoBaseMap{
                {"₀"_u8, '0'}, {"₁"_u8, '1'}, {"₂"_u8, '2'}, {"₃"_u8, '3'}, {"₄"_u8, '4'}, {"₅"_u8, '5'}, {"₆"_u8, '6'}, {"₇"_u8, '7'}, {"₈"_u8, '8'}, {"₉"_u8, '9'}
            };
            std::string real_numbers{};
            for (auto &&i : base)
            real_numbers.push_back(pseudoBaseMap[i]);

            std::size_t count_of_leading_zeros{};
            if (real_numbers.starts_with('0'))
            {
                for (size_t i = 0; i < real_numbers.size(); i++)
                {
                    if (real_numbers[i] == '0' and i != real_numbers.size() - 1)
                    ++count_of_leading_zeros;
                }
            }
            
            if (count_of_leading_zeros != 0)
            real_numbers.erase(real_numbers.begin(), real_numbers.begin() + count_of_leading_zeros);

            return real_numbers;
        }();

        auto isBaseValid = [&alteredBase]
        {
            // if (alteredBase == 0 or alteredBase > 36)
            return !(Myfcn::ManipulateStrAsBaseN::equal(alteredBase, "0", 10) or Myfcn::ManipulateStrAsBaseN::greaterThan(alteredBase, "36", 10));
        };

        auto areBaseDigitsValid = [&]
        {
            if (auto found = std::ranges::find(m_tokenString, '.'_u8); found != m_tokenString.end())
            {
                return
                Myfcn::ManipulateStrAsBaseN::isValidBaseDigits(Myfcn::convertU8StrToAsciiStr({m_tokenString.begin(), found}), static_cast<std::uint8_t>(std::stoi(alteredBase)))
                and
                Myfcn::ManipulateStrAsBaseN::isValidBaseDigits(Myfcn::convertU8StrToAsciiStr({found + 1, m_tokenString.end()}), static_cast<std::uint8_t>(std::stoi(alteredBase)));
            }
            //else
            return Myfcn::ManipulateStrAsBaseN::isValidBaseDigits(convertU8StrToAsciiStr(m_tokenString), static_cast<std::uint8_t>(std::stoi(alteredBase)));
        };

        auto is_within_range = [&](const std::string& literal, Myfcn::U8string fp_min, Myfcn::U8string fp_max)
        {
            std::ranges::replace(fp_min, "⋿"_u8, '@'_u8), std::ranges::replace(fp_max, "⋿"_u8, '@'_u8);
            return Myfcn::ManipulateStrAsBaseN::isFPNumberGreaterThanEqualTo(literal, Myfcn::convertU8StrToAsciiStr(fp_min)) and Myfcn::ManipulateStrAsBaseN::isFPNumberLessThanEqualTo(literal, Myfcn::convertU8StrToAsciiStr(fp_max));
        };

        auto convertFpToBaseN = [&](Myfcn::U8string str)
        {
            std::ranges::replace(str, "⋿"_u8, '@'_u8);
            auto return_value = Myfcn::ManipulateStrAsBaseN::convertFractionalBase10ToBaseN(Myfcn::convertU8StrToAsciiStr(str), uint8_t(std::stoi(alteredBase)));
            Myfcn::U8string u8_return_value = Myfcn::convertAsciiStrToU8Str(return_value);
            std::ranges::replace(u8_return_value, '@'_u8, "⋿"_u8);
            return u8_return_value;
        };

        auto approximate_if_decfp = [&](std::string& decfp)
        {
            if (literalType == ld32)
            decfp = Myfcn::ManipulateStrAsBaseN::approximation(decfp, 6);
            else if (literalType == ld64)
            decfp = Myfcn::ManipulateStrAsBaseN::approximation(decfp, 15);
            else if (literalType == ld128)
            decfp = Myfcn::ManipulateStrAsBaseN::approximation(decfp, 34);
            else if (literalType == ld256)
            decfp = Myfcn::ManipulateStrAsBaseN::approximation(decfp, 72);
            else if (literalType == ld512)
            decfp = Myfcn::ManipulateStrAsBaseN::approximation(decfp, 149);

            return decfp;
        };

        auto preparedTokenStr = [&]
        {
            m_tokenString += exponent.empty()? base : (isExponentNegative? base + "⋿-"_u8str + exponent : base + "⋿"_u8str + exponent);
            // return exponent.empty()? m_tokenString + base : (isExponentNegative? m_tokenString + base + "⋿-"_u8str + exponent : m_tokenString + base + "⋿"_u8str + exponent);
        };

        [&literalType]{ for (auto &&i : literalType) i.basicLatinToLowerCase(); }();
        
        bool shouldReturn{};
        if (isUnicodeBaseValid())
        {
            if (!isBaseValid())
            {
                m_log.write("real-number literal [ ", m_tokenString, " ] is tagged with a base that is not supported in NC; the base in question (", alteredBase, ')');
                start_Log();
                spaceLog();
                m_log.write("  only bases 1 - 36 are supported in NC; base options[₁₂₃₄₅₆₇₈₉]"), additionalLog();

                shouldReturn = true;
            }
            else if (!areBaseDigitsValid())
            {
                m_log.write("real-number literal [ ", m_tokenString, " ] contains invalid base(", alteredBase, ") digit(s)");
                start_Log();
                spaceLog();
                m_log.write("  only digits [");

                auto alteredBaseAsInt = std::stoi(alteredBase);
                for (auto i = 0, j = 0; i < alteredBaseAsInt; ++i)
                {
                    if (i >= 10)
                    m_log.write(char('A' + j), '(', char('a' + j), ')'), ++j;
                    else
                    m_log.write(i);

                    if (i != alteredBaseAsInt - 1)
                    m_log.write(',', ' ');
                }
                m_log.write("] are allowed in base(", alteredBase, ')');
                additionalLog();

                shouldReturn = true;
            }
        }
        else
        {
            m_log.write("real-number literal [ ", m_tokenString, " ] is tagged with an invalid base; the invalid base in question:(", base, ")"), start_Log();
            spaceLog();
            m_log.write("  try any valid combination of these[₁₂₃₄₅₆₇₈₉] instead; [NOTE] the valid combinations being from ₁ to ₃₆"), additionalLog();

            shouldReturn = true;
        }

        if (!exponent.empty())
        {
            if (std::ranges::all_of(exponent, [](Myfcn::U8char& x){ return x.isBasicLatinAlphabetNumeral(); }))
            {
                if (!Myfcn::ManipulateStrAsBaseN::isValidBaseDigits(convertU8StrToAsciiStr(exponent), std::uint8_t(10)))
                {
                    m_log.writews("the exponent [", exponent, "] of real-number literal [", m_tokenString, "] contains invalid base(10) digit(s)"), start_Log();
                    spaceLog();
                    m_log.write("  you're getting this error because the exponent of real-numbers must be base(10) regardless of the base it is tagged with"), additionalLog();
                    m_log.write("  oh, and in case you forgot, valid base(10) digits are from [0 - 9]"), additionalLog();

                    shouldReturn = true;
                }
                else if (Myfcn::ManipulateStrAsBaseN::greaterThanOrEqual(convertU8StrToAsciiStr(exponent), "2147483647", std::uint8_t(10)))
                {
                    if (isExponentNegative)
                    m_log.write("the exponent [ ", exponent, " ] of real-number literal [ -", m_tokenString, " ] is way too small, increase it"), start_Log();
                    else
                    m_log.writews("the exponent [", exponent, "] of real-number literal [", m_tokenString, "] is comically large, reduce it"), start_Log();

                    shouldReturn = true;
                }
            }
            else
            {
                m_log.write("base(10) digits where expected as the exponent in real-number literal [", m_tokenString, "] instead got this [", exponent, "]");
                start_Log();

                shouldReturn = true;
            }
        }

        if (shouldReturn)
        return;
        
        auto ascii_base10_realNumber = [&]
        {
            if (alteredBase == "10")
            {
                if (exponent.empty())
                return Myfcn::ManipulateStrAsBaseN::normalizeToScientificN(Myfcn::convertU8StrToAsciiStr(m_tokenString));
                else
                {
                    if (isExponentNegative)
                    return Myfcn::ManipulateStrAsBaseN::normalizeToScientificN(Myfcn::convertU8StrToAsciiStr(m_tokenString) + std::string{'@', '-'} + Myfcn::convertU8StrToAsciiStr(exponent));
                    else
                    return Myfcn::ManipulateStrAsBaseN::normalizeToScientificN(Myfcn::convertU8StrToAsciiStr(m_tokenString) + std::string{'@'} + Myfcn::convertU8StrToAsciiStr(exponent));
                }
            }
            else
            {
                std::string
                realConvertedToBase10 = Myfcn::ManipulateStrAsBaseN::convertFractionalBaseNToBase10(Myfcn::convertU8StrToAsciiStr(m_tokenString), std::uint8_t(std::stoi(alteredBase)));
                
                using boost_dec50 = boost::multiprecision::cpp_dec_float_50;

                if (exponent.empty())
                return Myfcn::ManipulateStrAsBaseN::normalizeToScientificN(realConvertedToBase10);
                else
                {
                    if (isExponentNegative)
                    {
                        std::string temp = (boost_dec50{realConvertedToBase10} * boost::multiprecision::pow(boost_dec50{alteredBase}, -1 * boost_dec50{Myfcn::convertU8StrToAsciiStr(exponent)})).str();

                        if (temp.contains('e')) 
                        std::ranges::replace(temp, 'e', '@');

                        return Myfcn::ManipulateStrAsBaseN::normalizeToScientificN(temp);
                    }
                    else
                    return Myfcn::ManipulateStrAsBaseN::normalizeToScientificN((boost_dec50{realConvertedToBase10} * boost::multiprecision::pow(boost_dec50{alteredBase}, boost_dec50{Myfcn::convertU8StrToAsciiStr(exponent)})).str());
                }
            }
            return std::string{};
        }();

        approximate_if_decfp(ascii_base10_realNumber); //because dec floating requires approximation

        Myfcn::U8string utf8_base10_realNumber = Myfcn::convertAsciiStrToU8Str(ascii_base10_realNumber);
        std::ranges::replace(utf8_base10_realNumber, '@'_u8, "⋿"_u8);
        preparedTokenStr(); //prepares the token string by appending the base and
        
        if (literalType.empty())
        {
            if (is_within_range(ascii_base10_realNumber, dec32fpMin, dec32fpMax))
            literalType = ld32;
            else if (is_within_range(ascii_base10_realNumber, dec64fpMin, dec64fpMax))
            literalType = ld64;
            else if (is_within_range(ascii_base10_realNumber, dec128fpMin, dec128fpMax))
            literalType = ld128;
            else if (is_within_range(ascii_base10_realNumber, dec256fpMin, dec256fpMax))
            literalType = ld256;
            else if (is_within_range(ascii_base10_realNumber, dec512fpMin, dec512fpMax))
            literalType = ld512;
            else
            {
                m_log.writews("real-number literal [", m_tokenString, "] is not within range of NC's implementation of decimal floating point that uses 512bits for storage");
                start_Log();
                spaceLog();
                if (alteredBase == "10")
                m_log.write("  NC decimal floating point range: min(", dec512fpMin, ") - max(", dec512fpMax, ')'), additionalLog();
                else
                {
                    m_log.write("  NC decimal floating point range: min(", dec512fpMin, " or ", convertFpToBaseN(dec512fpMin), ") - max(", dec512fpMax, " or ", convertFpToBaseN(dec512fpMax), ')');
                    additionalLog();
                    m_log.write("  here's the base(10) equivalent of the real-number literal by the way: ", utf8_base10_realNumber), additionalLog();
                }
            }
        }
        else
        {
            if (literalType == lb16)
            {
                if (!is_within_range(ascii_base10_realNumber, bin16fpMin, bin16fpMax))
                {
                    m_log.writews("real-number literal [", m_tokenString, "] is not within range of the IEEE binary16 floating point format"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.write("  IEEE binary16 floating point range: min(", bin16fpMin, ") - max(", bin16fpMax, ')'), additionalLog();
                    else
                    {
                        m_log.write("  IEEE binary16 floating point range: min(", bin16fpMin, " or ", convertFpToBaseN(bin16fpMin), ") - max(", bin16fpMax, " or ", convertFpToBaseN(bin16fpMax), ')');
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the real-number literal by the way: ", utf8_base10_realNumber), additionalLog();
                    }

                    m_log.write("  where (max) is the largest normal number and (min) is the smallest denormal number for that binary floating point format");
                    additionalLog();
                }
            }
            else if (literalType == lb32)
            {
                if (!is_within_range(ascii_base10_realNumber, bin32fpMin, bin32fpMax))
                {
                    m_log.writews("real-number literal [", m_tokenString, "] is not within range of the IEEE binary32 floating point format"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.write("  IEEE binary32 floating point range: min(", bin32fpMin, ") - max(", bin32fpMax, ')'), additionalLog();
                    else
                    {
                        m_log.write("  IEEE binary32 floating point range: min(", bin32fpMin, " or ", convertFpToBaseN(bin32fpMin), ") - max(", bin32fpMax, " or ", convertFpToBaseN(bin32fpMax), ')');
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the real-number literal by the way: ", utf8_base10_realNumber), additionalLog();
                    }

                    m_log.write("  where (max) is the largest normal number and (min) is the smallest denormal number for that binary floating point format");
                    additionalLog();
                }
            }
            else if (literalType == lb64)
            {
                if (!is_within_range(ascii_base10_realNumber, bin64fpMin, bin64fpMax))
                {
                    m_log.writews("real-number literal [", m_tokenString, "] is not within range of the IEEE binary64 floating point format"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.write("  IEEE binary64 floating point range: min(", bin64fpMin, ") - max(", bin64fpMax, ')'), additionalLog();
                    else
                    {
                        m_log.write("  IEEE binary64 floating point range: min(", bin64fpMin, " or ", convertFpToBaseN(bin64fpMin), ") - max(", bin64fpMax, " or ", convertFpToBaseN(bin64fpMax), ')');
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the real-number literal by the way: ", utf8_base10_realNumber), additionalLog();
                    }

                    m_log.write("  where (max) is the largest normal number and (min) is the smallest denormal number for that binary floating point format");
                    additionalLog();
                }
            }
            else if (literalType == lb128)
            {
                if (!is_within_range(ascii_base10_realNumber, bin128fpMin, bin128fpMax))
                {
                    m_log.writews("real-number literal [", m_tokenString, "] is not within range of the IEEE binary128 floating point format"), start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.write("  IEEE binary128 floating point range: min(", bin128fpMin, ") - max(", bin128fpMax, ')'), additionalLog();
                    else
                    {
                        m_log.write("  IEEE binary128 floating point range: min(", bin128fpMin, " or ", convertFpToBaseN(bin128fpMin), ") - max(", bin128fpMax, " or ", convertFpToBaseN(bin128fpMax), ')');
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the real-number literal by the way: ", utf8_base10_realNumber), additionalLog();
                    }

                    m_log.write("  where (max) is the largest normal number and (min) is the smallest denormal number for that binary floating point format");
                    additionalLog();
                }
            }
            else if (literalType == ld32)
            {
                if (!is_within_range(ascii_base10_realNumber, dec32fpMin, dec32fpMax))
                {
                    m_log.writews("real-number literal [", m_tokenString, "] is not within range of NC's implementation of decimal floating point that uses 32bits for storage");
                    start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.write("  NC decimal floating point range: min(", dec32fpMin, ") - max(", dec32fpMax, ')'), additionalLog();
                    else
                    {
                        m_log.write("  NC decimal floating point range: min(", dec32fpMin, " or ", convertFpToBaseN(dec32fpMin), ") - max(", dec32fpMax, " or ", convertFpToBaseN(dec32fpMax), ')');
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the real-number literal by the way: ", utf8_base10_realNumber), additionalLog();
                    }
                }
            }
            else if (literalType == ld64)
            {
                if (!is_within_range(ascii_base10_realNumber, dec64fpMin, dec64fpMax))
                {
                    m_log.writews("real-number literal [", m_tokenString, "] is not within range of NC's implementation of decimal floating point that uses 64bits for storage");
                    start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.write("  NC decimal floating point range: min(", dec64fpMin, ") - max(", dec64fpMax, ')'), additionalLog();
                    else
                    {
                        m_log.write("  NC decimal floating point range: min(", dec64fpMin, " or ", convertFpToBaseN(dec64fpMin), ") - max(", dec64fpMax, " or ", convertFpToBaseN(dec64fpMax), ')');
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the real-number literal by the way: ", utf8_base10_realNumber), additionalLog();
                    }
                }
            }
            else if (literalType == ld128)
            {
                if (!is_within_range(ascii_base10_realNumber, dec128fpMin, dec128fpMax))
                {
                    m_log.writews("real-number literal [", m_tokenString, "] is not within range of NC's implementation of decimal floating point that uses 128bits for storage");
                    start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.write("  NC decimal floating point range: min(", dec128fpMin, ") - max(", dec128fpMax, ')'), additionalLog();
                    else
                    {
                        m_log.write("  NC decimal floating point range: min(", dec128fpMin, " or ", convertFpToBaseN(dec128fpMin), ") - max(", dec128fpMax, " or ", convertFpToBaseN(dec128fpMax), ')');
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the real-number literal by the way: ", utf8_base10_realNumber), additionalLog();
                    }
                }
            }
            else if (literalType == ld256)
            {
                if (!is_within_range(ascii_base10_realNumber, dec256fpMin, dec256fpMax))
                {
                    m_log.writews("real-number literal [", m_tokenString, "] is not within range of NC's implementation of decimal floating point that uses 256bits for storage");
                    start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.write("  NC decimal floating point range: min(", dec256fpMin, ") - max(", dec256fpMax, ')'), additionalLog();
                    else
                    {
                        m_log.write("  NC decimal floating point range: min(", dec256fpMin, " or ", convertFpToBaseN(dec256fpMin), ") - max(", dec256fpMax, " or ", convertFpToBaseN(dec256fpMax), ')');
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the real-number literal by the way: ", utf8_base10_realNumber), additionalLog();
                    }
                }
            }
            else if (literalType == ld512)
            {
                if (!is_within_range(ascii_base10_realNumber, dec512fpMin, dec512fpMax))
                {
                    m_log.writews("real-number literal [", m_tokenString, "] is not within range of NC's implementation of decimal floating point that uses 512bits for storage");
                    start_Log();
                    spaceLog();
                    if (alteredBase == "10")
                    m_log.write("  NC decimal floating point range: min(", dec512fpMin, ") - max(", dec512fpMax, ')'), additionalLog();
                    else
                    {
                        m_log.write("  NC decimal floating point range: min(", dec512fpMin, " or ", convertFpToBaseN(dec512fpMin), ") - max(", dec512fpMax, " or ", convertFpToBaseN(dec512fpMax), ')');
                        additionalLog();
                        m_log.write("  here's the base(10) equivalent of the real-number literal by the way: ", utf8_base10_realNumber), additionalLog();
                    }
                }
            }
            else
            {
                if (literalType == li8 or literalType == li16 or literalType == li32 or literalType == li64 or literalType == li128 or literalType == li256 or literalType == li512 or literalType == li1024 or literalType == lui8 or literalType == lui16 or literalType == lui32 or literalType == lui64 or literalType == lui128 or literalType == lui256 or literalType == lui512 or literalType == lui1024)
                m_log.writews("real-number literal [", m_tokenString, "] is tagged with an integer literal type instead of a real-number literal type [", literalType, ']');
                else
                m_log.writews("real-number literal [", m_tokenString, "] is tagged with an unrecognized real-number literal type [", literalType, ']');

                start_Log(), spaceLog();

                m_log.write("  try the below instead:"), additionalLog();
                m_log.write("  ● [b16, b32, b64 or b128] for IEEE binary floating point numbers"), additionalLog();
                m_log.write("  ● [d32, d64, d128, d256, d512] for NC implementation of decimal floating point numbers"), additionalLog();
                m_log.write("  they are all case insensitive of course"), additionalLog();
            }
        }
    }

    Lexer::TokenType Lexer::tokenizeCharLiterals(const Myfcn::U8string* literalType_ptr)
    {
        auto formerFilePos{m_filePosition};
        if (literalType_ptr == nullptr) recordRelativeColumn(); //do not move

        nextFilePosAndColumn(); //skip past the single quote that was found

        auto isCharacterWiseLiteralValid = [&]()
        {
            return literalType_ptr == nullptr or (*literalType_ptr == lcharacterWise_a or *literalType_ptr == lcharacterWise_u8 or *literalType_ptr == lcharacterWise_p);
        };

        if (!isCharacterWiseLiteralValid())
        {
            m_log.writews('[', m_tokenString, "] is not a recognized character wise literal"), start_Log(), spaceLog();
            m_log.write("  try the below instead:"), additionalLog();
            m_log.write("  ● [ a ] for ascii string literal"), additionalLog();
            m_log.write("  ● [ p ] for plain string literal"), additionalLog();
            m_log.write("  ● [ u8 ] for utf8 string literal"), additionalLog();
            m_log.write("  [NOTE] default is [ p ] - plain string literal"), additionalLog();

            while (true)
            {
                if (m_isEndofFile or m_fileBuffer[m_filePosition] == '\n'_u8)
                {
                    m_log.writews("unterminated junk character literal, expected this [ \' ] before");

                    if (m_isEndofFile) m_log.write("EOF(end_of_file)");
                    else m_log.write("newline");
                        
                    start_Log(), spaceLog();

                    m_log.write("  it is junk because of log(", m_log.getLogCounter(), ") above"), additionalLog();
                    return TokenType::_miscellany;
                }
                else if (m_fileBuffer[m_filePosition] == '\''_u8)
                {
                    nextFilePosAndColumn(); //for extracting the loop terminating single quote too
                    break;
                }
                        
                if (m_fileBuffer[m_filePosition] == '\\'_u8) //for skipping potential escape code squences
                nextFilePosAndColumn();
                        
                nextFilePosAndColumn();
            }  
                
            //extract the token to m_tokenString
            for (size_t i = formerFilePos; m_isEndofFile? i <= m_filePosition : i < m_filePosition; ++i)
            m_tokenString.push_back(m_fileBuffer[i]);

            return TokenType::_miscellany;
        }

        //set literalType_ptr to the default characterWise-literal which is [ p ] if it is null
        if (literalType_ptr == nullptr) literalType_ptr = &lcharacterWise_p;

        auto stringedCharLiteral = [&]()
        {
            if (*literalType_ptr == lcharacterWise_a) return "ascii";
            else if (*literalType_ptr == lcharacterWise_p) return "plain";
            else if (*literalType_ptr == lcharacterWise_u8) return "utf8";
            return "[undefined]";
        }();

        while (true)
        {
            if (m_isEndofFile or m_fileBuffer[m_filePosition] == '\n'_u8)
            {
                if (m_isEndofFile)
                m_log.writews("unterminated", stringedCharLiteral, "character literal, expected this [ \' ] before EOF(end_of_file)");
                else
                m_log.writews("unterminated", stringedCharLiteral, "character literal, expected this [ \' ] before newline");

                start_Log();
                return TokenType::_miscellany;
            }
            else if (m_fileBuffer[m_filePosition] == '\''_u8)
            {
                nextFilePosAndColumn(); //for extracting the loop terminating single quote too
                break;
            }
                    
            if (m_fileBuffer[m_filePosition] == '\\'_u8) //for skipping potential escape code squences
            nextFilePosAndColumn();
                    
            nextFilePosAndColumn();
        }
            
        //extract the token to m_tokenString
        for (size_t i = formerFilePos; m_isEndofFile? i <= m_filePosition : i < m_filePosition; ++i)
        m_tokenString.push_back(m_fileBuffer[i]);

        if (validateNormalCharWiseLiterals(/*charType*/"character", /*stringedCharLiteral*/stringedCharLiteral, /*literalType*/*literalType_ptr) == TokenType::_miscellany)
        return TokenType::_miscellany;

        auto character_size = [&]
        {   
            int size{};
            for (auto i = m_tokenString.begin(); i != m_tokenString.end(); i++)
            {
                if (*i == '\\'_u8)
                {
                    ++i;
                    if (*i == 'U'_u8)
                    {
                        while (true)
                        {
                            if (*i == ']'_u8)
                            break;

                            ++i;
                        }
                    }
                }
                ++size;
            }
            
            return size - 2;
        }();

        //size check
        if (*literalType_ptr == lcharacterWise_u8)
        {
            if (character_size > 1)
            m_log.writews("size of", stringedCharLiteral ,"character literal [", m_tokenString, "] cannot be greater than 1"), start_Log();
        }
        else
        {
            if (std::ranges::any_of(m_tokenString, [](Myfcn::U8char& x){ return x.getByteEncoding() != Myfcn::U8char::ByteEncoding::one; }))
            m_log.writews(stringedCharLiteral, "character literal [", m_tokenString, "] cannot have characters(s) greater than 1[byte] of encoding"), start_Log();
            else if (character_size > 1)
            m_log.writews("size of", stringedCharLiteral, "character literal [", m_tokenString, "] cannot be greater than 1"), start_Log();
        }
        
        addToTokenDataList(m_tokenString, std::optional<std::uint32_t>{}, m_line, m_absoluteColumn, m_relativeColumn, false, false, *literalType_ptr);

        return TokenType::literal;
    }
    
    Lexer::TokenType Lexer::tokenizeStrLiterals(const Myfcn::U8string* literalType_ptr)
    {
        auto formerFilePos{m_filePosition};
        bool isRaw = false;
        std::optional<std::uint32_t> optLine{};
        if (literalType_ptr == nullptr) recordRelativeColumn(); //do not move

        nextFilePosAndColumn(); //skip past the double quote that was found

        //raw string literal string check
        if ((!m_isEndofFile and m_fileBuffer[m_filePosition] == '"'_u8) and (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == '"'_u8))
        isRaw = true;
        else prevFilePosAndColumn(m_filePosition - formerFilePos);/*if failed tack the filePosition back to the character past the initial double quote*/

        auto isCharacterWiseLiteralValid = [&]()
        {
            return literalType_ptr == nullptr or (*literalType_ptr == lcharacterWise_a or *literalType_ptr == lcharacterWise_u8 or *literalType_ptr == lcharacterWise_p);
        };

        if (!isCharacterWiseLiteralValid())
        {
            m_log.writews('[', m_tokenString, "] is not a recognized character wise literal"), start_Log(), spaceLog();
            m_log.write("  try the below instead:"), additionalLog();
            m_log.write("  ● [ a ] for ascii string literal"), additionalLog();
            m_log.write("  ● [ p ] for plain string literal"), additionalLog();
            m_log.write("  ● [ u8 ] for utf8 string literal"), additionalLog();
            m_log.write("  [NOTE] default is [ p ] - plain string literal"), additionalLog();

            if (isRaw)
            {
                while (true)
                {
                    if (m_isEndofFile)
                    {
                        m_log.writews("unterminated junk raw string literal, expected this [ \"\"\" ] before EOF(end_of_file)"), start_Log(), spaceLog();
                        m_log.write("  it is junk because of log(", m_log.getLogCounter(), ") above"), additionalLog();
                        return TokenType::_miscellany;
                    }
                    else if
                    (
                        m_fileBuffer[m_filePosition] == '"'_u8
                        and
                        (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == '"'_u8)
                        and
                        (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == '"'_u8)
                    )
                    {
                        nextFilePosAndColumn(); //for extracting the loop terminating double quote
                        break;
                    }
                    else if (!m_isEndofFile and m_fileBuffer[m_filePosition] == '\n'_u8)//it is directly under the second conditionally for a reason
                    newLineFound();

                    nextFilePosAndColumn();
                }
                
                //extract the token to m_tokenString
                for (size_t i = formerFilePos; m_isEndofFile? i <= m_filePosition : i < m_filePosition; ++i)
                {
                    if (m_fileBuffer[i] == '\n')
                    m_tokenString += std::string{'\\', 'n'};
                    else
                    m_tokenString.push_back(m_fileBuffer[i]);
                }
            }
            else
            {
                while (true)
                {
                    if (m_isEndofFile or m_fileBuffer[m_filePosition] == '\n'_u8)
                    {
                        m_log.writews("unterminated junk string literal, expected this [ \" ] before");

                        if (m_isEndofFile) m_log.write("EOF(end_of_file)");
                        else m_log.write("newline");
                        
                        start_Log(), spaceLog();

                        m_log.write("  it is junk because of log(", m_log.getLogCounter(), ") above"), additionalLog();
                        return TokenType::_miscellany;
                    }
                    else if (m_fileBuffer[m_filePosition] == '"'_u8)
                    {
                        nextFilePosAndColumn(); //for extracting the loop terminating single quote too
                        break;
                    }
                        
                    if (m_fileBuffer[m_filePosition] == '\\'_u8) //for skipping potential escape code squences
                    nextFilePosAndColumn();
                        
                    nextFilePosAndColumn();
                }  
                
                //extract the token to m_tokenString
                for (size_t i = formerFilePos; m_isEndofFile? i <= m_filePosition : i < m_filePosition; ++i)
                m_tokenString.push_back(m_fileBuffer[i]);
            }

            return TokenType::_miscellany;
        }

        //set literalType_ptr to the default characterWise-literal which is [ p ] if it is null
        if (literalType_ptr == nullptr) literalType_ptr = &lcharacterWise_p;

        auto stringedCharLiteral = [&]()
        {
            if (*literalType_ptr == lcharacterWise_a) return "ascii";
            else if (*literalType_ptr == lcharacterWise_p) return "plain";
            else if (*literalType_ptr == lcharacterWise_u8) return "utf8";
            return "[undefined]";
        }();

        if (isRaw)
        {   
            auto startLine = m_line;
            while (true)
            {
                if (m_isEndofFile)
                {
                    m_log.writews("unterminated", stringedCharLiteral, "string literal, expected this [ \"\"\" ] before EOF(end_of_file)"), start_Log();
                    return TokenType::_miscellany;
                }
                else if
                (
                    m_fileBuffer[m_filePosition] == '"'_u8
                    and
                    (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == '"'_u8)
                    and
                    (nextFilePosAndColumn(), !m_isEndofFile and m_fileBuffer[m_filePosition] == '"'_u8)
                )
                {
                    nextFilePosAndColumn(); //for extracting the loop terminating double quote
                    break;
                }
                else if (!m_isEndofFile and m_fileBuffer[m_filePosition] == '\n'_u8)//it is directly under the second conditionally for a reason
                newLineFound();

                nextFilePosAndColumn();
            }
            //if the raw string is multiline, set the optional-line
            if (startLine != m_line) optLine = startLine;
            
            //extract the token to m_tokenString
            for (size_t i = formerFilePos; m_isEndofFile? i <= m_filePosition : i < m_filePosition; ++i)
            {
                if (m_fileBuffer[i] == '\n')
                m_tokenString += std::string{'\\', 'n'};
                else
                m_tokenString.push_back(m_fileBuffer[i]);
            }
        }
        else
        {
            while (true)
            {
                if (m_isEndofFile or m_fileBuffer[m_filePosition] == '\n'_u8)
                {
                    if (m_isEndofFile)
                    m_log.writews("unterminated", stringedCharLiteral, "string literal, expected this [ \" ] before eof(end-of-file)");
                    else
                    m_log.writews("unterminated", stringedCharLiteral, "string literal, expected this [ \" ] before newline");

                    start_Log();
                    return TokenType::_miscellany;
                }
                else if (m_fileBuffer[m_filePosition] == '"'_u8)
                {
                    nextFilePosAndColumn(); //for extracting the loop terminating single quote too
                    break;
                }
                    
                if (m_fileBuffer[m_filePosition] == '\\'_u8) //for skipping potential escape code squences
                nextFilePosAndColumn();
                    
                nextFilePosAndColumn();
            }
            
            //extract the token to m_tokenString
            for (size_t i = formerFilePos; m_isEndofFile? i <= m_filePosition : i < m_filePosition; ++i)
            m_tokenString.push_back(m_fileBuffer[i]);

            validateNormalCharWiseLiterals(/*charType*/"string", /*stringedCharLiteral*/stringedCharLiteral, /*literalType*/*literalType_ptr);
        }

        addToTokenDataList(m_tokenString, optLine, m_line, m_absoluteColumn, m_relativeColumn, false, false, *literalType_ptr);
        
        return TokenType::literal;
    }

    Lexer::TokenType Lexer::validateNormalCharWiseLiterals(const char* charType, const char* stringedCharLiteral, const Myfcn::U8string& literalType)
    {
        TokenType result = TokenType::literal;

        for (auto i = m_tokenString.begin(); i != m_tokenString.end(); ++i)
        {
            if (*i == '\\'_u8)
            {
                switch (std::uint32_t(*++i))
                {
                case 'z': case 'e': case 'b': case 'f': case 'n': case 'r': case 't': case 'v': case '\'': case '\"': case '\\': break;

                case 'U': case 'u':
                    if (*++i != '[')
                    {
                        result = TokenType::_miscellany;

                        m_log.writews("the left square brace [ [ ] denoting start of escaped unicode code point entry is missing in", stringedCharLiteral, charType, "literal");
                        start_Log(), spaceLog();
                        m_log.write("  try this: U|u[<code-point>]; remmeber the base of the number is expected to be base(16)"), additionalLog();
                    }
                    else
                    {
                        Myfcn::U8string u8str{};
                        while (++i != m_tokenString.end())
                        {
                            if (*i == ']'_u8)
                            break;
                                
                            u8str.push_back(*i);
                        }

                        if (i == m_tokenString.end())
                        {
                            result = TokenType::_miscellany;

                            // m_log.write("missing right square brace [ ] ] denoting end of escaped unicode code point entry in", stringedCharLiteral, charType, "literal");
                            m_log.writews("the left square brace [ [ ] denoting end of escaped unicode code point entry is missing in", stringedCharLiteral, charType, "literal");
                            start_Log();

                            --i; //take i back to point before end so the for statement won't loop forever
                        }
                        else
                        {
                            //trim any digit separator(') or whitespace if found
                            if (std::ranges::any_of(u8str, [](Myfcn::U8char& x){ return x.isBasicLatinWhiteSpace() or x == '\''; }))
                            {
                                auto newEnd = std::remove_if(u8str.begin(), u8str.end(), [](Myfcn::U8char& x){ return x.isBasicLatinWhiteSpace() or x == '\''; });
                                u8str.erase(newEnd, u8str.end());
                            }

                            if (!u8str.empty())
                            {
                                if (std::ranges::all_of(u8str, [](Myfcn::U8char& i){ if (i.isBasicLatinAlphabetNumeral()) return true; return false; }))
                                {
                                    if (!Myfcn::ManipulateStrAsBaseN::isValidBaseDigits(convertU8StrToAsciiStr(u8str), 16))
                                    {
                                        result = TokenType::_miscellany;

                                        m_log.writews("in", stringedCharLiteral, charType, ", escaped unicode code point entry [", u8str, "] contains invalid base(16) digit(s)");
                                        start_Log(), spaceLog();
                                        m_log.write("  try these digits instead: 1, 2, 3, 4, 5, 6, 7, 8, 9, A(a), B(b), C(c), D(d), E(e), F(f)"), additionalLog();
                                    }
                                    else
                                    {
                                        if (literalType == lcharacterWise_u8 and Myfcn::ManipulateStrAsBaseN::greaterThan(convertU8StrToAsciiStr(u8str), "10ffff", 16))
                                        {
                                            result = TokenType::_miscellany;

                                            m_log.writews("in", stringedCharLiteral, charType, "literal, escaped unicode code point entry [", u8str, "] is greater than maximum unicode code-point 10FFFF₁₆");
                                            start_Log();
                                        }
                                        else if ((void*)charType == (void*)"character" and Myfcn::ManipulateStrAsBaseN::greaterThan(convertU8StrToAsciiStr(u8str), "7f", 16))
                                        {
                                            result = TokenType::_miscellany;

                                            m_log.writews("in", stringedCharLiteral, charType, "literal, escaped unicode code point entry [", u8str, "] is greater than maximum ascii code-point 7F₁₆");
                                            start_Log();
                                        }
                                    }
                                }
                                else
                                {
                                    result = TokenType::_miscellany;

                                    m_log.writews("in", stringedCharLiteral, charType, "literal, escaped unicode code point entry contains non-base(16) digits [", u8str, "]");
                                    start_Log();
                                }
                            }
                        }
                    }
                break;
                    
                default:
                    result = TokenType::_miscellany;

                    m_log.writews(stringedCharLiteral, charType, "literal [", m_tokenString, "], contains an invalid escape squence: [", *i, ']'), start_Log();
                    spaceLog();
                    m_log.write("  try the below instead:"), additionalLog();
                    m_log.write(R"(  ● [ b ] for backspace equivalent to \U[8])"), additionalLog();
                    m_log.write(R"(  ● [ e ] for escape equivalent to \U[1B])"), additionalLog();
                    m_log.write(R"(  ● [ f ] for formfeed equivalent to \U[C])"), additionalLog();
                    m_log.write(R"(  ● [ n ] for newline equivalent to \U[A])"), additionalLog();
                    m_log.write(R"(  ● [ r ] for carraige-return equivalent to \U[D])"), additionalLog();
                    m_log.write(R"(  ● [ t ] for horizontal-tab equivalent to \U[9])"), additionalLog();
                    m_log.write(R"(  ● [ v ] for vertical tab equivalent to \U[B])"), additionalLog();
                    m_log.write(R"(  ● [ " ] for double-quote equivalent to \U[22])"), additionalLog();
                    m_log.write(R"(  ● [ ' ] for single-quote equivalent to \U[27])"), additionalLog();
                    m_log.write(R"(  ● [ z ] for null-character equivalent to \U[0])"), additionalLog();
                    m_log.write(R"(  ● [ U ] or [ u ] for entry of unicode code-point entry => \U[<code-point>])"), additionalLog();
                    m_log.write("  obviously the usage is this: \\(EscapeSquence)"), additionalLog();
                    break;
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
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == sGreaterthan.front())
                m_tokenString.assign(sArrow), nextFilePosAndColumn() /*skip past the sGreaterthan token*/;
                else if (m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sSubtractionAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
                else if (m_fileBuffer[m_filePosition] == sMinus.front())
                m_tokenString.assign(sDecrement), nextFilePosAndColumn() /*skip past the second sMinus token*/;
                else
                m_tokenString.assign(sMinus);
            }
            else m_tokenString.assign(sMinus);
        }
        else if (m_fileBuffer[m_filePosition] == sLessthan.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sLessthan_equalto), nextFilePosAndColumn() /*skip past the sEqual token*/;
                else if (m_fileBuffer[m_filePosition] == sLessthan.front())
                {
                    if (nextFilePosAndColumn(), !m_isEndofFile)
                    {
                        if (m_fileBuffer[m_filePosition] == sEqual.front())
                        m_tokenString.assign(sBitwiseShiftLeftAssign), nextFilePosAndColumn();
                        else
                        m_tokenString.assign(sBitwiseShiftLeft);
                    }
                    else m_tokenString.assign(sBitwiseShiftLeft);
                }
                else
                m_tokenString.assign(sLessthan);
            }
            else m_tokenString.assign(sLessthan);
        }
        else if (m_fileBuffer[m_filePosition] == sGreaterthan.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sGreaterthan_equalto), nextFilePosAndColumn() /*skip past the sEqual token*/;
                else if (m_fileBuffer[m_filePosition] == sGreaterthan.front())
                {
                    if (nextFilePosAndColumn(), !m_isEndofFile)
                    {
                        if (m_fileBuffer[m_filePosition] == sEqual.front())
                        m_tokenString.assign(sBitwiseShiftRightAssign), nextFilePosAndColumn();
                        else
                        m_tokenString.assign(sBitwiseShiftRight);
                    }
                    else m_tokenString.assign(sBitwiseShiftRight);
                }
                else
                m_tokenString.assign(sGreaterthan);
            }
            else m_tokenString.assign(sGreaterthan);
        }
        else if (m_fileBuffer[m_filePosition] == sColon.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
                else if (m_fileBuffer[m_filePosition] == sColon.front())
                m_tokenString.assign(sScopeResolution), nextFilePosAndColumn() /*skip past the sColon token*/;
                else
                m_tokenString.assign(sColon);
            }
            else m_tokenString.assign(sColon);
        }
        else if (m_fileBuffer[m_filePosition] == sNot.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sNotEqual), nextFilePosAndColumn() /*skip past the sEqual token*/;
                else if (m_fileBuffer[m_filePosition] == sLcurly.front())
                m_tokenString.assign(sLinverseCurly), nextFilePosAndColumn() /*skip past the sLcurly token*/;
                else if (m_fileBuffer[m_filePosition] == sLparen.front())
                m_tokenString.assign(sLinverseParen), nextFilePosAndColumn() /*skip past the sLinverseParen token*/;
                else
                m_tokenString.assign(sNot);
            }
            else m_tokenString.assign(sNot);
        }
        else if (m_fileBuffer[m_filePosition] == sBitwiseAnd.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == sBitwiseAnd.front())
                m_tokenString.assign(sAnd), nextFilePosAndColumn() /*skip past the second sBitwiseAnd token*/;
                else if (m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sBitwiseAndAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
                else
                m_tokenString.assign(sBitwiseAnd);
            }
            else m_tokenString.assign(sBitwiseAnd);
        }
        else if (m_fileBuffer[m_filePosition] == sBitwiseOr.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == sBitwiseOr.front())
                m_tokenString.assign(sOr), nextFilePosAndColumn() /*skip past the second sBitwiseOr token*/;
                else if (m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sBitwiseOrAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
                else
                m_tokenString.assign(sBitwiseOr);
            }
            else m_tokenString.assign(sBitwiseOr);
        }
        else if (m_fileBuffer[m_filePosition] == sBitwiseXor.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == sBitwiseXor.front())
                m_tokenString.assign(sXor), nextFilePosAndColumn() /*skip past the second sBitwiseXor token*/;
                else if (m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sBitwiseXorAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
                else
                m_tokenString.assign(sBitwiseXor);
            }
            else m_tokenString.assign(sBitwiseXor);
        }
        else if (m_fileBuffer[m_filePosition] == sPlus.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sAdditionAssign), nextFilePosAndColumn() /*skip past the SEqual token*/;
                else if (m_fileBuffer[m_filePosition] == sPlus.front())
                m_tokenString.assign(sIncrement), nextFilePosAndColumn() /*skip past the second sPlus token*/;
                else
                m_tokenString.assign(sPlus);
            }
            else m_tokenString.assign(sPlus);
        }
        else if (m_fileBuffer[m_filePosition] == sMultiply.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sMultiplicationAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
                else
                m_tokenString.assign(sMultiply);
            }
            else m_tokenString.assign(sMultiply);
        }
        else if (m_fileBuffer[m_filePosition] == sDivide.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sDivisionAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
                else
                m_tokenString.assign(sDivide);
            }
            else m_tokenString.assign(sDivide);
        }
        else if (m_fileBuffer[m_filePosition] == sModuluo.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == sEqual.front())
                m_tokenString.assign(sModuluoAssign), nextFilePosAndColumn() /*skip past the sEqual token*/;
                else
                m_tokenString.assign(sModuluo);
            }
            else m_tokenString.assign(sModuluo);
        }
        else if (m_fileBuffer[m_filePosition] == sLhashedParen.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == sLparen.front())
                m_tokenString.assign(sLhashedParen), nextFilePosAndColumn() /*skip past the sLparen token*/;
                else
                return prevFilePosAndColumn(), TokenType::_miscellany; /*unextract the hash[#]*/
            }
            else return TokenType::_miscellany;
        }
        else if (m_fileBuffer[m_filePosition] == sRcurly.front())
        {
            if (nextFilePosAndColumn(), !m_isEndofFile)
            {
                if (m_fileBuffer[m_filePosition] == sSemicolon.front())
                m_tokenString.assign(sRsemicolonCurly), nextFilePosAndColumn() /*skip past the sSemicolon token*/;
                else
                m_tokenString.assign(sRcurly);
            }
            else m_tokenString.assign(sRcurly);
        }
        else if (
        m_fileBuffer[m_filePosition] == sLcurly.front() or m_fileBuffer[m_filePosition] == sLparen.front() or
        m_fileBuffer[m_filePosition] == sRparen.front() or m_fileBuffer[m_filePosition] == sSemicolon.front() or
        m_fileBuffer[m_filePosition] == sBitwiseNot.front() or m_fileBuffer[m_filePosition] == sComma.front() or
        m_fileBuffer[m_filePosition] == sEqual.front() or m_fileBuffer[m_filePosition] == sQuestionMark.front() or
        m_fileBuffer[m_filePosition] == sMemberOf.front() or m_fileBuffer[m_filePosition] == sExponention.front() or
        m_fileBuffer[m_filePosition] == sCommercialAt.front() or m_fileBuffer[m_filePosition] == sAlternateLessthan_equalto.front() or
        m_fileBuffer[m_filePosition] == sAlternateNotEqual.front() or m_fileBuffer[m_filePosition] == sAlternateGreaterthan_equalto.front()
        )
        m_tokenString.push_back(m_fileBuffer[m_filePosition]), nextFilePosAndColumn() /*move the file position past the symbol*/;
        else
        return TokenType::_miscellany;
        
        addToTokenDataList(m_tokenString, std::optional<std::uint32_t>{}, m_line, m_absoluteColumn, m_relativeColumn, false, false, {}, true);

        return TokenType::symbol;
    }

    void Lexer::start_Log()
    {
        m_log.log(true), m_isDeadZone = true;
        
        m_log.writews("  line:", m_line, "column:", m_relativeColumn, '-', m_absoluteColumn), m_log.log();

        Myfcn::U8string lineContents{};
        for (auto i = m_fileBuffer.begin() + m_lineNumToFilePos[m_line]; i != m_fileBuffer.end(); i.operator++())
        {
            if (*i == '\n'_u8)
            break;

            lineContents.push_back(*i);
        }

        m_log.writews("  line contents:" , lineContents), m_log.log();

        if (m_absoluteColumn - m_relativeColumn == 1)
        m_log.writews("               :", Myfcn::U8string{lineContents.size(), ' '_u8}.insert(m_relativeColumn - 1, m_absoluteColumn - m_relativeColumn, "⇑"_u8));
        else
        m_log.writews("               :", Myfcn::U8string{lineContents.size(), ' '_u8}.insert(m_relativeColumn - 1, m_absoluteColumn - m_relativeColumn, "↑"_u8));

        m_log.log();
        //*†↥⇑↑
    }

    void Lexer::start_MultiLineCommentLog()
    {
        m_log.log(true);

        m_log.writews("  line:", m_line, "column:", m_absoluteColumn), m_log.log();

        Myfcn::U8string lineContents{};
        for (auto i = m_fileBuffer.begin() + m_lineNumToFilePos[m_line]; i != m_fileBuffer.end(); i.operator++())
        {
            if (*i == '\n'_u8)
            break;

            lineContents.push_back(*i);
        }

        m_log.writews("  line contents:" , lineContents), m_log.log();

        // std::cout << "m_absoluteColum: " << m_absoluteColumn << ' ' << m_absoluteColumn - 2 << " lineContents.size(): " << lineContents.size() << '\n';
        // m_absoluteColum: 1 (4294967295):[m_absoluteColumn - 2] lineContents.size(): 1
        // m_log.writews("               :", Myfcn::U8string{lineContents.size(), ' '_u8}.insert(m_absoluteColumn - 2, 1, "⇑"_u8));

        m_log.writews("               :", Myfcn::U8string{lineContents.size(), ' '_u8}.insert(lineContents.size() - 1, 1, "⇑"_u8)), m_log.log();
        spaceLog();
        m_log.writews("  meaning it was expected after [", m_fileBuffer.at(m_fileBuffer.size() - 1), ']'), m_log.log();
    }

    void Lexer::spaceLog()
    { m_log.log(); }

    void Lexer::additionalLog()
    { m_log.log(); }

    [[noreturn]] void Lexer::errorOutput()
    {
        m_log.write("⟨compilation terminated⟩ (⌐■_■) ψ(._. )>");
        m_log.log();
        
        m_log.outputLogEntries("LEXER ERROR", System::coutput);

        m_line = {};
        m_lineNumToFilePos.~unordered_map();
        m_absoluteColumn = {};
        m_relativeColumn = {};
        m_fileBuffer.~basic_string();
        m_filePosition = {};
        m_tokenDataList.~vector();
        m_tokenString.~basic_string();
        m_log.~Log();
        m_isDeadZone = {};
        
        std::exit(EXIT_FAILURE);
    }

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