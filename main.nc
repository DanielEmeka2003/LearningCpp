jester() void {  }

sum()
{
    `{ multi-line comment }`
    ``single line comment ⟨⟩

    for none : (default, ≠, 23)
    io::stdout.write("emeka")

    int32:(c p) a := 34
}

enum Theta: int32 { ead, error, fase := 23 };

namespace Awesome::Daniel::Main
{
    sumType daniel
    {
        int32,
        int64
    };
};

sumType 🛺_isPoor ``sumType declaration
{ bin32fp, u8string };

productType 👀_😈 ``productType declaration
{ int32 a, pstring c };

👩🏿👀(int32 a!{23}, int32, u8string g := "er") void ``function declaration
{
    while (true)
    io.sing()
}

language() ``fixing accidental C-style syntax and expression-list error
{
    f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);

    d.global::fcn();
}

``next to implement is namespace association with identifiers
``after that compsite types[ sum, product and enums ] => forward declarations of said type
``addition of the [this] hidden composite type pointer
``match expressions
``👩🏿 = 👩+ 🏿

main() int32
{
    !((int32, achar)int32) a := 9

    2 + {5 = 9 - 8 * 0}
    {3} + 5
    {4} - {if(8) give {0}}
    2 + 3? 1:8 *3;
    +-@34
    e({}, {+5})

    #(=, and, 23)(12, {give 23}, 8)
    a(2, 6, !{})

    typecast<int32, 2 >;
    (int32!{})
    
    ``lambda(int32 a, int32 b){ a * b };
    ``typecast<int32, (2 > 3), 2>()

    for none : (default, ≠, 23)
    stdout.write("emeka")

    io::get_string()
}

namespace A
{
    namespace B
    {
        namespace C
        {
            namespace D
            {
                namespace E
                {
                    sumType ErrorCase;
                    type_alias E := !(()int32);
                };
            };
        };
    };
};

enum Father : bool
{ daniel, diego := 23, gatsby!{-23} };

`{
    for i, j, k : (!{default, 0, 0}, <, 32, !{1, 1, 1})
    io::stdout.write(i)

    `{
        `{
            `{
                `{
                    `{
                        `{
                            `{
                                `{
                                    `{
                                        `{
                                            `{
                                                `{
                                                    `{

                                                    }`
                                                }`
                                            }`
                                        }`
                                    }`
                                }`
                            }`
                        }`
                    }`
                }`
            }`
        }`
    }`

    imbed(C++)
    {
        #include <stdio>

        void main()
        {
            int a;
            printf("%d", 23);
        }
    }

    [Lexer Problems]
    1.this dum problem i am encountering with an unrecognized token been put at the begining of a file should be diagnosed ✓
    2.remove the conversion from the given base to base 10 in function validateIntLiteral ✓
    3.add the dynamic integer guess size that when a specific literal isn't given, like 232313232334, should be i64 ✓
    4.mathematical symbols and super and sub scripts unicode characters should not be allowed as valid tokens[Dingbats included] ✓
    5.fix the lessThan function in myfunctions.cpp to work how you wrote it ✓
    6.validate floating point literals ✓
    7.keywords: panic, raise(check if i can raise a SIGINT in C/C++), lambda, alias, namespace
    8.multi-line comments should be nested ✓
    9.token [**] should be used for exponention and token[//] should be used for finding the root of a number

    [Parser Problems]
    1.find a fix for the something like this: a main() {{ function declaration }} ✓
    2.find a fix for something like this: int32 main() {{ function declaration }} ✕
    3.implement block as an expression ✓
    4.implement type as an expression ✓
    5.implement jump_statement as an expression ✕
    6.remake mapTokenToCategory ✓
    7.add alternate mathematical operators for not-equal-to(≤≥≠), lessThan-equal-to, greaterThan-equal-to ✓
    8.support for alternate mathematical operators for multiplication and division would be provided later
    9.ncFile should store a variant of either a global variable or a function declaration ✓
    10.remove null-statement[;] from statement options ✓
    11.remove cast expression ✓
    12.rethink the attribute problem that spawns doubt for (p) => [stackPtr<int32:(c)>, heapPtr:<int32:(c)>] ✓
    13.remove type as an expression, and change ncType to typeInfo, that takes a template parameter for it's type to act on ✓
    14.add user defined literals definition(possible implementation: (2#i8)s)
    15.support for range loops (possible syntax: "for" "(" <identifier> "|"<expression> "," <expression>) "|" ")" <statement> ✓
    16.add support syntax for compiler given parameter variables in function type:
    example: 
    !((int32, achar)void)
    {
        parameter<int32>() + parameter<achar>()
    }
    17.add compileTime support for various things like marking a block as compileTime
    compileTime{
        if (e = 3)
        give 20
    }
    18.add support for lambdas, i.e for forms of callable types {until support for classes is done}
    19.add support for complex types, like: anynmonus and unanynmonus product types, anynmonus and unanynmonus sum types and enums (class support comes later)
    20.support for match expressions comes next
    21.add support generics and specialization of said generics
    22.add support for elipses(...) or something similar to it
    23.change the template agrument syntax to this: typecast<int32>, it should only accept primary expressions and types ✓
    24.add exception support, such that things such as try-catch and propagate have concrete definitions
    25.reflect on error handling for my language, while adding exception support ✓
    26.also reason about member functions for first class types(integer types, floatingPoint Types, typeInfo, functionTypes and so on)
    27.constraints should be added to enrich generics after things like ncType and ranges have been defined concretely
    28.think about how attributes should be and used in my language(they should have namespace like syntax though: function_body::forLoop::eval_once)
    29.type_alias and namespace_alias should be added
    30.namespaces support should be added
    31.think about how multiple files would interact with one another[nc_impl, nc_inf, nc and (the import system)]
    32.heapPtr should be added as a type for providing heap memory acess[also an addition pointer should be added to decrease fragementation] +
    33.add the file name of the current file it is compiling, like: current file is main.nc
    34.change the parsing of brackets, to that of type attribute brackets ✕
    35.heapMemory would have an alternative called customHeapMemory whoose main aim is to reduce fragementation +
    36.with the way i currently parse local-variables and temp-variables, problems would arise when something like this is met [ (int) ], local-variable
    would take [int] as a valid type and error when an identifier is not found, which would be catastrophic, so you i fix it ✓
    37.support range loops; possible syntax "range" "(" <identifier>: "|" <expression> "|" ")"
    38.built in static array, dynamic array, list, map, uo_map, stack, queque, set: all data structures i hope to implement
    39.for my ast, instead of making this void, incorporate (none) into classes whoose members have sumTypes that are not exhaustive[expression, statement, funcDeclaration]

    let me just say it now, code-generation won't happen for a really long time.
    you are the one that wanted to design a language that compares to c++, 🤣
}`