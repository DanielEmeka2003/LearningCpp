#ifndef NCLEXER_H
#define NCLEXER_H

#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <cstdint>
#include <unordered_map>

#include "stream/stringoutputstream.h"
#include "myfunctions1.h"
#include "ncLog.h"

namespace Nc
{
    using namespace Myfcn::u8literals;

    class Lexer
    {
    public:
    enum class TokenType{reserved, identifier, literal, symbol, _miscellany, _comment, _eot};

    /*Literal*/

    static inline const Myfcn::U8string li8{"i8"_u8str};
    static inline const Myfcn::U8string li16{"i16"_u8str};
    static inline const Myfcn::U8string li32{"i32"_u8str};
    static inline const Myfcn::U8string li64{"i64"_u8str};
    static inline const Myfcn::U8string li128{"i128"_u8str};
    static inline const Myfcn::U8string li256{"i256"_u8str};
    static inline const Myfcn::U8string li512{"i512"_u8str};
    static inline const Myfcn::U8string li1024{"i1024"_u8str};
    static inline const Myfcn::U8string lui8{"ui8"_u8str};
    static inline const Myfcn::U8string lui16{"ui16"_u8str};
    static inline const Myfcn::U8string lui32{"ui32"_u8str};
    static inline const Myfcn::U8string lui64{"ui64"_u8str};
    static inline const Myfcn::U8string lui128{"ui128"_u8str};
    static inline const Myfcn::U8string lui256{"ui256"_u8str};
    static inline const Myfcn::U8string lui512{"ui512"_u8str};
    static inline const Myfcn::U8string lui1024{"ui1024"_u8str};

    static inline const Myfcn::U8string lbool{"boolL"_u8str};
    static inline const Myfcn::U8string lnullptr{"nullptrL"_u8str};
    static inline const Myfcn::U8string lnone{"noneL"_u8str};

    static inline const Myfcn::U8string lb16{"b16"_u8str};
    static inline const Myfcn::U8string lb32{"b32"_u8str};
    static inline const Myfcn::U8string lb64{"b64"_u8str};
    static inline const Myfcn::U8string lb128{"b128"_u8str};
    static inline const Myfcn::U8string ld32{"d32"_u8str};
    static inline const Myfcn::U8string ld64{"d64"_u8str};
    static inline const Myfcn::U8string ld128{"d128"_u8str};
    static inline const Myfcn::U8string ld256{"d256"_u8str};
    static inline const Myfcn::U8string ld512{"d512"_u8str};
    
    static inline const Myfcn::U8string lcharacterWise_a{"a"_u8str};
    static inline const Myfcn::U8string lcharacterWise_u8{"u8"_u8str};
    static inline const Myfcn::U8string lcharacterWise_p{"p"_u8str};
    static inline const Myfcn::U8string lcharacterWise_ra{"ra"_u8str};
    static inline const Myfcn::U8string lcharacterWise_ru8{"ru8"_u8str};
    static inline const Myfcn::U8string lcharacterWise_rp{"rp"_u8str};

    /*Reserved*/

    static inline const Myfcn::U8string rInt8{"int8"_u8str};      
    static inline const Myfcn::U8string rInt16{"int16"_u8str};
    static inline const Myfcn::U8string rInt32{"int32"_u8str};          
    static inline const Myfcn::U8string rInt64{"int64"_u8str}; 
    static inline const Myfcn::U8string rInt128{"int128"_u8str};
    static inline const Myfcn::U8string rInt256{"int256"_u8str};
    static inline const Myfcn::U8string rInt512{"int512"_u8str};
    static inline const Myfcn::U8string rInt1024{"int1024"_u8str};
    static inline const Myfcn::U8string rUint8{"uint8"_u8str};
    static inline const Myfcn::U8string rUint16{"uint16"_u8str};
    static inline const Myfcn::U8string rUint32{"uint32"_u8str};
    static inline const Myfcn::U8string rUint64{"uint64"_u8str};
    static inline const Myfcn::U8string rUint128{"uint128"_u8str};
    static inline const Myfcn::U8string rUint256{"uint256"_u8str};
    static inline const Myfcn::U8string rUint512{"uint512"_u8str};
    static inline const Myfcn::U8string rUint1024{"uint1024"_u8str};
    static inline const Myfcn::U8string rBool{"bool"_u8str};
    static inline const Myfcn::U8string rAchar{"achar"_u8str};
    static inline const Myfcn::U8string rPchar{"pchar"_u8str};
    static inline const Myfcn::U8string rU8char{"u8char"_u8str};
    static inline const Myfcn::U8string rAstring{"astring"_u8str};
    static inline const Myfcn::U8string rPstring{"pstring"_u8str};
    static inline const Myfcn::U8string rU8string{"u8string"_u8str};
    static inline const Myfcn::U8string rBin16fp{"bin16fp"_u8str};
    static inline const Myfcn::U8string rBin32fp{"bin32fp"_u8str};
    static inline const Myfcn::U8string rBin64fp{"bin64fp"_u8str};
    static inline const Myfcn::U8string rBin128fp{"bin128fp"_u8str};
    static inline const Myfcn::U8string rDec32fp{"dec32fp"_u8str};
    static inline const Myfcn::U8string rDec64fp{"dec64fp"_u8str};
    static inline const Myfcn::U8string rDec128fp{"dec128fp"_u8str};
    static inline const Myfcn::U8string rDec256fp{"dec256fp"_u8str};
    static inline const Myfcn::U8string rDec512fp{"dec512fp"_u8str};
    static inline const Myfcn::U8string rVoid{"void"_u8str};
    static inline const Myfcn::U8string rNullptrType{"nullptrType"_u8str};
    static inline const Myfcn::U8string rNoneType{"noneType"_u8str};
    static inline const Myfcn::U8string rAnyTypeList{"anyTypeList"_u8str};
    static inline const Myfcn::U8string rAnyType{"anyType"_u8str};
    static inline const Myfcn::U8string rNcType{"ncType"_u8str};
    static inline const Myfcn::U8string rTypeinfo{"typeinfo"_u8str};
    static inline const Myfcn::U8string rGive{"give"_u8str};
    static inline const Myfcn::U8string rBreak{"break"_u8str};
    static inline const Myfcn::U8string rContinue{"continue"_u8str};
    static inline const Myfcn::U8string rTrue{"true"_u8str};
    static inline const Myfcn::U8string rFalse{"false"_u8str};
    static inline const Myfcn::U8string rIf{"if"_u8str};    
    static inline const Myfcn::U8string rElse{"else"_u8str};
    static inline const Myfcn::U8string rWhile{"while"_u8str};
    static inline const Myfcn::U8string rDo{"do"_u8str};
    static inline const Myfcn::U8string rFor{"for"_u8str};
    static inline const Myfcn::U8string rStatic{"static"_u8str};
    static inline const Myfcn::U8string rAnd{"and"_u8str};
    static inline const Myfcn::U8string rOr{"or"_u8str};
    static inline const Myfcn::U8string rNot{"not"_u8str};
    static inline const Myfcn::U8string rXor{"xor"_u8str};
    static inline const Myfcn::U8string rBitwiseAnd{"bitand"_u8str};
    static inline const Myfcn::U8string rBitwiseOr{"bitor"_u8str};
    static inline const Myfcn::U8string rBitwiseNot{"bitnot"_u8str};
    static inline const Myfcn::U8string rBitwiseXor{"bitxor"_u8str};
    static inline const Myfcn::U8string rOdd{"odd"_u8str};
    static inline const Myfcn::U8string rOutput{"output"_u8str};
    static inline const Myfcn::U8string rInput{"input"_u8str};
    static inline const Myfcn::U8string rSizeof{"sizeof"_u8str};
    static inline const Myfcn::U8string rMem{"mem"_u8str};
    static inline const Myfcn::U8string rNullptr{"nullptr"_u8str};
    static inline const Myfcn::U8string rNone{"none"_u8str};
    static inline const Myfcn::U8string rGuess{"guess"_u8str};
    static inline const Myfcn::U8string rTypecast{"typecast"_u8str};
    static inline const Myfcn::U8string rBitcast{"bitcast"_u8str};
    static inline const Myfcn::U8string rDefault{"default"_u8str};
    static inline const Myfcn::U8string rSumType{"sumType"_u8str};
    static inline const Myfcn::U8string rProductType{"productType"_u8str};
    static inline const Myfcn::U8string rEnum{"enum"_u8str};
    static inline const Myfcn::U8string rNamespace{"namespace"_u8str};
    static inline const Myfcn::U8string rGlobal{"global"_u8str};
    static inline const Myfcn::U8string rType_alias{"type_alias"_u8str};
    static inline const Myfcn::U8string rNamespace_alias{"namespace_alias"_u8str};

    /*Symbol*/
    
    static inline const Myfcn::U8string sArrow{"->"_u8str};
    static inline const Myfcn::U8string sAssign{":="_u8str};
    static inline const Myfcn::U8string sEqual{'='_u8};
    static inline const Myfcn::U8string sNotEqual{"!="_u8str};
    static inline const Myfcn::U8string sLcurly{'{'_u8};
    static inline const Myfcn::U8string sRcurly{'}'_u8};
    static inline const Myfcn::U8string sLparen{'('_u8};
    static inline const Myfcn::U8string sRparen{')'_u8};
    static inline const Myfcn::U8string sSemicolon{';'_u8};
    static inline const Myfcn::U8string sLessthan{'<'_u8};
    static inline const Myfcn::U8string sGreaterthan{'>'_u8};
    static inline const Myfcn::U8string sLessthan_equalto{"<="_u8str};
    static inline const Myfcn::U8string sGreaterthan_equalto{">="_u8str};
    static inline const Myfcn::U8string sPlus{'+'_u8};
    static inline const Myfcn::U8string sMinus{'-'_u8};
    static inline const Myfcn::U8string sMultiply{'*'_u8};
    static inline const Myfcn::U8string sDivide{'/'_u8};
    static inline const Myfcn::U8string sModuluo{'%'_u8};
    static inline const Myfcn::U8string sAnd{"&&"_u8str};
    static inline const Myfcn::U8string sOr{"||"_u8str};
    static inline const Myfcn::U8string sNot{'!'_u8};
    static inline const Myfcn::U8string sXor{"^^"_u8str};
    static inline const Myfcn::U8string sBitwiseAnd{'&'_u8};
    static inline const Myfcn::U8string sBitwiseOr{'|'_u8};
    static inline const Myfcn::U8string sBitwiseNot{'~'_u8};
    static inline const Myfcn::U8string sBitwiseXor{'^'_u8};
    static inline const Myfcn::U8string sBitwiseShiftLeft{"<<"_u8str};
    static inline const Myfcn::U8string sBitwiseShiftRight{">>"_u8str};
    static inline const Myfcn::U8string sComma{','_u8};
    static inline const Myfcn::U8string sAdditionAssign{"+="_u8str};
    static inline const Myfcn::U8string sSubtractionAssign{"-="_u8str};
    static inline const Myfcn::U8string sMultiplicationAssign{"*="_u8str};
    static inline const Myfcn::U8string sDivisionAssign{"/="_u8str};
    static inline const Myfcn::U8string sModuluoAssign{"%="_u8str};
    static inline const Myfcn::U8string sBitwiseShiftLeftAssign{"<<="_u8str};
    static inline const Myfcn::U8string sBitwiseShiftRightAssign{">>="_u8str};
    static inline const Myfcn::U8string sBitwiseAndAssign{"&="_u8str};
    static inline const Myfcn::U8string sBitwiseOrAssign{"|="_u8str};
    static inline const Myfcn::U8string sBitwiseXorAssign{"^="_u8str};
    static inline const Myfcn::U8string sIncrement{"++"_u8str};
    static inline const Myfcn::U8string sDecrement{"--"_u8str};
    static inline const Myfcn::U8string sColon{':'_u8};
    static inline const Myfcn::U8string sQuestionMark{'?'_u8};
    static inline const Myfcn::U8string sMemberOf{'.'_u8};
    static inline const Myfcn::U8string sExponention{"∘"_u8};
    static inline const Myfcn::U8string sCommercialAt{'@'_u8};
    static inline const Myfcn::U8string sLinverseCurly{"!{"_u8str};
    static inline const Myfcn::U8string sLinverseParen{"!("_u8str};
    static inline const Myfcn::U8string sLhashedParen{"#("_u8str};
    // static inline const Myfcn::U8string sLmathangle{"⟨"_u8};
    // static inline const Myfcn::U8string sRmathangle{"⟩"_u8};
    static inline const Myfcn::U8string sScopeResolution{"::"_u8str};
    static inline const Myfcn::U8string sAlternateLessthan_equalto{"≤"_u8str};
    static inline const Myfcn::U8string sAlternateGreaterthan_equalto{"≥"_u8str};
    static inline const Myfcn::U8string sAlternateNotEqual{"≠"_u8str};
    static inline const Myfcn::U8string sRsemicolonCurly{"};"_u8str};
    // static inline const Myfcn::U8string sLSquare{"["_u8str};
    // static inline const Myfcn::U8string sRSquare{"]"_u8str};
    
    static inline const Myfcn::U8string int8Max{"127"_u8str};
    static inline const Myfcn::U8string int16Max{"32767"_u8str};
    static inline const Myfcn::U8string int32Max{"2147483647"_u8str};
    static inline const Myfcn::U8string int64Max{"9223372036854775807"_u8str};
    static inline const Myfcn::U8string int128Max{"170141183460469231731687303715884105727"_u8str};
    static inline const Myfcn::U8string int256Max{"57896044618658097711785492504343953926634992332820282019728792003956564819967"_u8str};
    static inline const Myfcn::U8string int512Max{"6703903964971298549787012499102923063739682910296196688861780721860882015036773488400937149083451713845015929093243025426876941405973284973216824503042047"_u8str};
    static inline const Myfcn::U8string int1024Max{"89884656743115795386465259539451236680898848947115328636715040578866337902750481566354238661203768010560056939935696678829394884407208311246423715319737062188883946712432742638151109800623047059726541476042502884419075341171231440736956555270413618581675255342293149119973622969239858152417678164812112068607"_u8str};

    static inline const Myfcn::U8string uint8Max{"255"_u8str};
    static inline const Myfcn::U8string uint16Max{"65535"_u8str};
    static inline const Myfcn::U8string uint32Max{"4294967295"_u8str};
    static inline const Myfcn::U8string uint64Max{"18446744073709551615"_u8str};
    static inline const Myfcn::U8string uint128Max{"340282366920938463463374607431768211455"_u8str};
    static inline const Myfcn::U8string uint256Max{"115792089237316195423570985008687907853269984665640564039457584007913129639935"_u8str};
    static inline const Myfcn::U8string uint512Max{"13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084095"_u8str};
    static inline const Myfcn::U8string uint1024Max{"179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137215"_u8str};

    static inline const Myfcn::U8string bin16fpMin{"5.96⋿-8"_u8str};
    static inline const Myfcn::U8string bin16fpMax{"6.5504⋿4"_u8str};
    static inline const Myfcn::U8string bin32fpMin{"1.40129846432⋿-45"_u8str};
    static inline const Myfcn::U8string bin32fpMax{"3.402823466385⋿38"_u8str};
    static inline const Myfcn::U8string bin64fpMin{"4.940656458412⋿-324"_u8str};
    static inline const Myfcn::U8string bin64fpMax{"1.79769313486⋿308"_u8str};
    static inline const Myfcn::U8string bin128fpMin{"6.475175119438⋿-4966"_u8str};
    static inline const Myfcn::U8string bin128fpMax{"1.189731495357⋿4932"_u8str};

    static inline const Myfcn::U8string dec32fpMin{"1.0⋿-64"_u8str};
    static inline const Myfcn::U8string dec32fpMax{"9.999999⋿63"_u8str};
    static inline const Myfcn::U8string dec64fpMin{"1.0⋿-256"_u8str};
    static inline const Myfcn::U8string dec64fpMax{"9.999999999999999⋿255"_u8str};
    static inline const Myfcn::U8string dec128fpMin{"1.0⋿-512"_u8str};
    static inline const Myfcn::U8string dec128fpMax{"9.9999999999999999999999999999999999⋿511"_u8str};
    static inline const Myfcn::U8string dec256fpMin{"1.0⋿-1024"_u8str};
    static inline const Myfcn::U8string dec256fpMax{"9.999999999999999999999999999999999999999999999999999999999999999999999999⋿1023"_u8str};
    static inline const Myfcn::U8string dec512fpMin{"1.0⋿-2048"_u8str};
    static inline const Myfcn::U8string dec512fpMax{"9.99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999⋿2047"_u8str};

    struct TokenData
    {
        Myfcn::U8string token{};
        std::uint32_t line{};
        std::uint32_t absoluteColumn{};
        std::uint32_t relativeColumn{};
        bool isReserved{};
        bool isIdentifier{};
        Myfcn::U8string literalType{};
        bool isSymbol{};
    };
    
    using LineNumerToFilePositionMap = std::unordered_map<std::uint32_t, std::uint32_t>;

    private:
        std::uint32_t m_line{1};
        LineNumerToFilePositionMap m_lineNumToFilePos{ {1, 0} };
        std::uint32_t m_absoluteColumn{1};
        std::uint32_t m_relativeColumn{};
        Myfcn::U8string m_fileBuffer{};
        std::uint32_t m_filePosition{};
        std::vector<TokenData> m_tokenDataList{};
        Myfcn::U8string m_tokenString{};
        Log m_log{};
        bool m_isDeadZone{};
        bool m_isEndofFile{};
        bool m_AbsoluteColumnStaticBoolean{true};

    public:
        Lexer(Myfcn::U8string& fileBuffer);
        Lexer(Myfcn::U8string&& fileBuffer);
        ~Lexer() = default;

        void lexFileBuffer();

        const std::vector<TokenData>& getTokenList() const;
        std::vector<TokenData>& getTokenList();
        LineNumerToFilePositionMap& getLineNumToFilePos();
        const LineNumerToFilePositionMap& getLineNumToFilePos() const;
        Myfcn::U8string& getFileBuffer();
        const Myfcn::U8string& getFileBuffer() const;

        void logBoxStyle(Log::BoxLineStyle s);

    private:
        TokenType lexify();

        void newLineFound();
        void nextFilePosAndColumn(std::uint32_t number = 1);
        void prevFilePosAndColumn(std::uint32_t number = 1);
        void recordRelativeColumn();
        void addToTokenDataList(Myfcn::U8string& tokenString, std::uint32_t line, std::uint32_t absoluteColumn, std::uint32_t relativeColumn, bool isReserved = false, bool isIdentifier = false, Myfcn::U8string literalType = {}, bool isSymbol = false);

        TokenType singleLineComment();
        TokenType multiLineComment();

        TokenType tokenizeWords();
        TokenType reservedWords();
        
        TokenType tokenizeNumbers();
        void validateIntLiteral(Myfcn::U8string& base, Myfcn::U8string& literalType);
        void validateRealNumberLiteral(Myfcn::U8string& base_exp, Myfcn::U8string& literalType);

        TokenType tokenizeCharLiterals(const Myfcn::U8string const* literalType_ptr = nullptr);
        TokenType tokenizeStrLiterals(const Myfcn::U8string const* literalType_ptr = nullptr);
        TokenType validateNormalCharWiseLiterals(const char* charType, const char* stringedCharLiteral, const Myfcn::U8string& literalType){}

        TokenType tokenizeSymbols();

        void start_Log();
        void start_MultiLineCommentLog();
        void spaceLog();
        void additionalLog();

        [[noreturn]] void errorOutput();
    };

    //overload version of the output stream for type Lexer::TokenType
    std::ostream& operator<<(std::ostream& out, Lexer::TokenType temp);

} // namespace Nc

#endif