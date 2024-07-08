#ifndef NCLEXER_H
#define NCLEXER_H

#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <cstdint>
#include <unordered_map>

#include "stream/stringoutputstream.h"
#include "misc_utility.h"
#include "u8char.h"
#include "ncLog.h"

namespace Nc
{
    class Lexer
    {
    public:
    enum class TokenType{reserved, identifier, literal, symbol, _miscellany, _comment, _eot};

    /*Literal*/

    static inline const U8string lt_i8{"i8"_u8str}; //switching from U8string to std::string_view is beyond impossible in c++, i would switch when i move over to nc
    static inline const U8string lt_i16{"i16"_u8str};
    static inline const U8string lt_i32{"i32"_u8str};
    static inline const U8string lt_i64{"i64"_u8str};
    static inline const U8string lt_i128{"i128"_u8str};
    static inline const U8string lt_i256{"i256"_u8str};
    static inline const U8string lt_i512{"i512"_u8str};
    static inline const U8string lt_i1024{"i1024"_u8str};
    static inline const U8string lt_ui8{"ui8"_u8str};
    static inline const U8string lt_ui16{"ui16"_u8str};
    static inline const U8string lt_ui32{"ui32"_u8str};
    static inline const U8string lt_ui64{"ui64"_u8str};
    static inline const U8string lt_ui128{"ui128"_u8str};
    static inline const U8string lt_ui256{"ui256"_u8str};
    static inline const U8string lt_ui512{"ui512"_u8str};
    static inline const U8string lt_ui1024{"ui1024"_u8str};
    
    static inline const U8string lt_b16{"b16"_u8str};
    static inline const U8string lt_b32{"b32"_u8str};
    static inline const U8string lt_b64{"b64"_u8str};
    static inline const U8string lt_b128{"b128"_u8str};
    static inline const U8string lt_d32{"d32"_u8str};
    static inline const U8string lt_d64{"d64"_u8str};
    static inline const U8string lt_d128{"d128"_u8str};
    static inline const U8string lt_d256{"d256"_u8str};
    static inline const U8string lt_d512{"d512"_u8str};
    
    static inline const U8string lt_characterWise_a{"a"_u8str};
    static inline const U8string lt_characterWise_u8{"u8"_u8str};
    static inline const U8string lt_characterWise_p{"p"_u8str};

    static inline const U8string lInt{'i'_u8};
    static inline const U8string lBool{'b'_u8};
    static inline const U8string lReal{'r'_u8};
    static inline const U8string lStr{'s'_u8};
    static inline const U8string lChr{'c'_u8};

    /*Reserved*/

    /*----------------------------------------------------------------*/
    static inline const U8string rInt8{"int8"_u8str};      
    static inline const U8string rInt16{"int16"_u8str};
    static inline const U8string rInt32{"int32"_u8str};          
    static inline const U8string rInt64{"int64"_u8str}; 
    static inline const U8string rInt128{"int128"_u8str};
    static inline const U8string rInt256{"int256"_u8str};
    static inline const U8string rInt512{"int512"_u8str};
    static inline const U8string rInt1024{"int1024"_u8str};
    static inline const U8string rUint8{"uint8"_u8str};
    static inline const U8string rUint16{"uint16"_u8str};
    static inline const U8string rUint32{"uint32"_u8str};
    static inline const U8string rUint64{"uint64"_u8str};
    static inline const U8string rUint128{"uint128"_u8str};
    static inline const U8string rUint256{"uint256"_u8str};
    static inline const U8string rUint512{"uint512"_u8str};
    static inline const U8string rUint1024{"uint1024"_u8str};
    static inline const U8string rBool{"bool"_u8str};
    static inline const U8string rAchar{"achar"_u8str};
    static inline const U8string rPchar{"pchar"_u8str};
    static inline const U8string rU8char{"u8char"_u8str};
    static inline const U8string rAstring{"astring"_u8str};
    static inline const U8string rPstring{"pstring"_u8str};
    static inline const U8string rU8string{"u8string"_u8str};
    static inline const U8string rBin16fp{"bin16fp"_u8str};
    static inline const U8string rBin32fp{"bin32fp"_u8str};
    static inline const U8string rBin64fp{"bin64fp"_u8str};
    static inline const U8string rBin128fp{"bin128fp"_u8str};
    static inline const U8string rDec32fp{"dec32fp"_u8str};
    static inline const U8string rDec64fp{"dec64fp"_u8str};
    static inline const U8string rDec128fp{"dec128fp"_u8str};
    static inline const U8string rDec256fp{"dec256fp"_u8str};
    static inline const U8string rDec512fp{"dec512fp"_u8str};
    static inline const U8string rNullptrType{"nullptrType"_u8str};
    static inline const U8string rNoneType{"noneType"_u8str};
    static inline const U8string rAnyTypeList{"anyTypeList"_u8str};
    static inline const U8string rAnyType{"anyType"_u8str};
    static inline const U8string rNcType{"ncType"_u8str};
    static inline const U8string rTypeinfo{"typeinfo"_u8str};
    static inline const U8string rVoid{"void"_u8str};
    static inline const U8string rUnamed_pt{"unamed_pt"_u8str};
    static inline const U8string rUnamed_st{"unamed_st"_u8str};
    /*----------------------------------------------------------------*/

    static inline const U8string rGuess{"guess"_u8str};
    static inline const U8string rGive{"give"_u8str};
    static inline const U8string rBreak{"break"_u8str};
    static inline const U8string rContinue{"continue"_u8str};
    static inline const U8string rTrue{"true"_u8str};
    static inline const U8string rFalse{"false"_u8str};
    static inline const U8string rIf{"if"_u8str};    
    static inline const U8string rElse{"else"_u8str};
    static inline const U8string rWhile{"while"_u8str};
    static inline const U8string rDo{"do"_u8str};
    static inline const U8string rFor{"for"_u8str};
    static inline const U8string rAnd{"and"_u8str};
    static inline const U8string rOr{"or"_u8str};
    static inline const U8string rNot{"not"_u8str};
    static inline const U8string rXor{"xor"_u8str};
    static inline const U8string rOdd{"odd"_u8str};
    static inline const U8string rNone{"none"_u8str};
    static inline const U8string rDefault{"default"_u8str};
    static inline const U8string rEnum{"enum"_u8str};
    static inline const U8string rName_space{"name_space"_u8str};
    static inline const U8string rTypeof{"typeof"_u8str};
    static inline const U8string rMatch{"match"_u8str};
    static inline const U8string rCatch{"catch"_u8str};
    static inline const U8string rRaise{"raise"_u8str};
    static inline const U8string rCp{"cp"_u8str};
    static inline const U8string rImut{"imut"_u8str};
    static inline const U8string rAlias{"alias"_u8str};
    static inline const U8string rR_l{"r_l"_u8str};
    static inline const U8string rTcast{"tcast"_u8str};
    static inline const U8string rBcast{"bcast"_u8str};
    static inline const U8string rNand{"nand"_u8str};
    static inline const U8string rNor{"nor"_u8str};
    static inline const U8string rNxor{"nxor"_u8str};
    static inline const U8string rAnd_eq{"and_eq"_u8str};
    static inline const U8string rOr_eq{"or_eq"_u8str};
    static inline const U8string rXor_eq{"xor_eq"_u8str};
    static inline const U8string rNand_eq{"nand_eq"_u8str};
    static inline const U8string rNor_eq{"nor_eq"_u8str};
    static inline const U8string rNxor_eq{"nxor_eq"_u8str};
    static inline const U8string rNamed_pt{"named_pt"_u8str};
    static inline const U8string rNamed_st{"named_st"_u8str};
    static inline const U8string rCase{"case"_u8str};
    static inline const U8string rIter{"iter"_u8str};
    static inline const U8string rIter_no_end{"iter_no_end"_u8str};
    static inline const U8string rType{"type"_u8str};
    static inline const U8string rVarg_start{"_v"_u8str};
    static inline const U8string rVarg_end{"v_"_u8str};
    static inline const U8string rFn{"fn"_u8str};
    static inline const U8string rGoto{"goto"_u8str};
    static inline const U8string rLabel{"label"_u8str};
    static inline const U8string rDefer{"defer"_u8str};
    static inline const U8string rMem{"mem"_u8str};
    static inline const U8string rType_space{"type_space"_u8str};
    static inline const U8string rMe{"me"_u8str};

    /*Symbol*/
    
    static inline const U8string sArrow{"->"_u8str};
    static inline const U8string sAssign{'='_u8};
    static inline const U8string sEqual{"=="_u8str};
    static inline const U8string sNotEqual{"!="_u8str};
    static inline const U8string sLcurly{'{'_u8};
    static inline const U8string sRcurly{'}'_u8};
    static inline const U8string sLparen{'('_u8};
    static inline const U8string sRparen{')'_u8};
    static inline const U8string sSemicolon{';'_u8};
    static inline const U8string sLessthan{'<'_u8};
    static inline const U8string sGreaterthan{'>'_u8};
    static inline const U8string sLessThan_equalTo{"<="_u8str};
    static inline const U8string sGreaterThan_equalTo{">="_u8str};
    static inline const U8string sPlus{'+'_u8};
    static inline const U8string sMinus{'-'_u8};
    static inline const U8string sMultiply{'*'_u8};
    static inline const U8string sDivide{'/'_u8};
    static inline const U8string sRemainder{'%'_u8};
    static inline const U8string sNot{'!'_u8};
    static inline const U8string sShiftLeft{"<<"_u8str};
    static inline const U8string sShiftRight{">>"_u8str};
    static inline const U8string sRotateLeft{"<<<"_u8str};
    static inline const U8string sRotateRight{">>>"_u8str};
    static inline const U8string sComma{','_u8};
    static inline const U8string sPlusAssign{"+="_u8str};
    static inline const U8string sMinusAssign{"-="_u8str};
    static inline const U8string sMultiplyAssign{"*="_u8str};
    static inline const U8string sDivideAssign{"/="_u8str};
    static inline const U8string sRemainderAssign{"%="_u8str};
    static inline const U8string sShiftLeftAssign{"<<="_u8str};
    static inline const U8string sShiftRightAssign{">>="_u8str};
    static inline const U8string sRotateLeftAssign{"<<<="_u8str};
    static inline const U8string sRotateRightAssign{">>>="_u8str};
    static inline const U8string sIncrement{"++"_u8str};
    static inline const U8string sDecrement{"--"_u8str};
    static inline const U8string sColon{':'_u8};
    static inline const U8string sQuestionMark{'?'_u8};
    static inline const U8string sMemberOf{'.'_u8};
    static inline const U8string sExponention{'^'_u8};
    static inline const U8string sCommercialAt{'@'_u8};
    static inline const U8string sScopeResolution{"::"_u8str};
    static inline const U8string sAlternateLessThan_equalTo{"≤"_u8};
    static inline const U8string sAlternateGreaterThan_equalTo{"≥"_u8};
    static inline const U8string sAlternateNotEqual{"≠"_u8};
    static inline const U8string sLsquare{'['_u8};
    static inline const U8string sRsquare{']'_u8};
    static inline const U8string sAlternateDivide{"÷"_u8};
    static inline const U8string sAlternateDivideAssign{"÷="_u8str};
    static inline const U8string sDivideRemainder{"/%"_u8str};
    static inline const U8string sInitAssign{":="_u8str};
    static inline const U8string sAmperSand{'&'_u8};
    static inline const U8string sPipe{'|'_u8};
    static inline const U8string sExponentionAssign{"^="_u8str};
    static inline const U8string sReverseMinusAssign{"=-"_u8str};
    static inline const U8string sReverseDivideAssign{"=/"_u8str};
    static inline const U8string sReverseRemainderAssign{"=%"_u8str};
    static inline const U8string sReverseAlternateDivideAssign{"=÷"_u8str};
    static inline const U8string sHashedLsquare{"#["_u8str};
    static inline const U8string sDollarSignLparen{"$("_u8str};
    
    
    static inline const U8string int8Max{"127"_u8str};
    static inline const U8string int16Max{"32767"_u8str};
    static inline const U8string int32Max{"2147483647"_u8str};
    static inline const U8string int64Max{"9223372036854775807"_u8str};
    static inline const U8string int128Max{"170141183460469231731687303715884105727"_u8str};
    static inline const U8string int256Max{"57896044618658097711785492504343953926634992332820282019728792003956564819967"_u8str};
    static inline const U8string int512Max{"6703903964971298549787012499102923063739682910296196688861780721860882015036773488400937149083451713845015929093243025426876941405973284973216824503042047"_u8str};
    static inline const U8string int1024Max{"89884656743115795386465259539451236680898848947115328636715040578866337902750481566354238661203768010560056939935696678829394884407208311246423715319737062188883946712432742638151109800623047059726541476042502884419075341171231440736956555270413618581675255342293149119973622969239858152417678164812112068607"_u8str};

    static inline const U8string uint8Max{"255"_u8str};
    static inline const U8string uint16Max{"65535"_u8str};
    static inline const U8string uint32Max{"4294967295"_u8str};
    static inline const U8string uint64Max{"18446744073709551615"_u8str};
    static inline const U8string uint128Max{"340282366920938463463374607431768211455"_u8str};
    static inline const U8string uint256Max{"115792089237316195423570985008687907853269984665640564039457584007913129639935"_u8str};
    static inline const U8string uint512Max{"13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084095"_u8str};
    static inline const U8string uint1024Max{"179769313486231590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137215"_u8str};

    static inline const U8string bin16fpMin{"5.96@-8"_u8str};
    static inline const U8string bin16fpMax{"6.5504@+4"_u8str};
    static inline const U8string bin32fpMin{"1.40129846432@-45"_u8str};
    static inline const U8string bin32fpMax{"3.402823466385@+38"_u8str};
    static inline const U8string bin64fpMin{"4.940656458412@-324"_u8str};
    static inline const U8string bin64fpMax{"1.79769313486@+308"_u8str};
    static inline const U8string bin128fpMin{"6.475175119438@-4966"_u8str};
    static inline const U8string bin128fpMax{"1.189731495357@+4932"_u8str};

    static inline const U8string dec32fpMin{"1.0@-64"_u8str};
    static inline const U8string dec32fpMax{"9.999999@+63"_u8str};
    static inline const U8string dec64fpMin{"1.0@-256"_u8str};
    static inline const U8string dec64fpMax{"9.999999999999999@+255"_u8str};
    static inline const U8string dec128fpMin{"1.0@-512"_u8str};
    static inline const U8string dec128fpMax{"9.9999999999999999999999999999999999@+511"_u8str};
    static inline const U8string dec256fpMin{"1.0@-1024"_u8str};
    static inline const U8string dec256fpMax{"9.999999999999999999999999999999999999999999999999999999999999999999999999@+1023"_u8str};
    static inline const U8string dec512fpMin{"1.0@-16384"_u8str};
    static inline const U8string dec512fpMax{"9.9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999@+16383"_u8str};

    struct TokenData
    {
        U8string token{};
        std::optional<std::uint32_t> optLine{};
        std::uint32_t line{};
        std::uint32_t relativeColumn{};
        std::uint32_t absoluteColumn{};
        bool isReserved{};
        bool isIdentifier{};
        U8string literalType{};
        bool isSymbol{};
    };
    
    using LineNumerToFilePositionMap = std::unordered_map<std::uint32_t, std::uint32_t>;

    private:
        std::uint32_t m_line{1};
        LineNumerToFilePositionMap m_lineNumToFilePos{ {1, 0} };
        std::uint32_t m_absoluteColumn{1};
        std::uint32_t m_relativeColumn{};
        U8string m_fileBuffer{};
        std::uint32_t m_filePosition{};
        std::vector<TokenData> m_tokenDataList{}; //change this to an nc list when moving over to it .i.e ds::list
        U8string m_tokenString{};
        Log m_log{};
        bool m_isDeadZone{};
        bool m_isEndofFile{};
        bool m_AbsoluteColumnStaticBoolean{true};

    public:
        Lexer(U8string& fileBuffer);
        Lexer(U8string&& fileBuffer);
        ~Lexer() = default;

        void lexFileBuffer();

        const std::vector<TokenData>& getTokenDataList() const;
        std::vector<TokenData>& getTokenDataList();
        LineNumerToFilePositionMap& getLineNumToFilePos();
        const LineNumerToFilePositionMap& getLineNumToFilePos() const;
        U8string& getFileBuffer();
        const U8string& getFileBuffer() const;

        void logBoxStyle(Log::BoxLineStyle s);

    private:
        TokenType lexify();

        void newLineFound();
        void nextFilePosAndColumn(std::uint32_t number = 1);
        void prevFilePosAndColumn(std::uint32_t number = 1);
        void recordRelativeColumn();
        void addToTokenDataList(std::optional<std::uint32_t> optLine, std::uint32_t line, bool isReserved = false, bool isIdentifier = false, U8string literalType = {}, bool isSymbol = false);

        TokenType singleLineDiscardComment();
        TokenType singleLineDocComment();
        TokenType multiLineDiscardComment();
        TokenType multiLineDocComment();

        TokenType tokenizeWords();
        TokenType reservedWords();
        
        TokenType tokenizeNumbers();
        TokenType validateIntNumberLiteral(const U8string& base, U8string& literalType);
        TokenType validateRealNumberLiteral(const U8string& base_exp, U8string& literalType);

        TokenType tokenizeCharLiterals(const U8string* literalType_ptr = nullptr);
        TokenType tokenizeStrLiterals(const U8string* literalType_ptr = nullptr);
        TokenType validateNormalCharWiseLiterals(std::string literalTypeName_plus_characterWiseType, const U8string& literalType, const std::vector<std::uint32_t>& rawStr_escape_sequence_size_list);

        TokenType tokenizeSymbols();

        void start_log();
        void start_log(std::uint32_t relativeColumn, std::uint32_t absoluteColumn, std::uint32_t line, bool consideredNewLine_flag = false);
        void start_multiLineLog(std::uint32_t start_line, std::uint32_t relativeColumn);
        void spaceLog();
        void additionalLog();

        void errorOutput();
    };

    //overload version of the output stream for type Lexer::TokenType
    std::ostream& operator<<(std::ostream& out, Lexer::TokenType temp);

} // namespace Nc

#endif