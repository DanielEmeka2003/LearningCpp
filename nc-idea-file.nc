
format(type u8str x)
{
    guess cut_string ::= x[type pair(x.find('i'), x.find('.'))]

    cut_string.insert("XXXXX", 2)

    ``Are none and nullptr that important to be reserved identifiers for their types?

    "\##{emeka\n\t}##"
    "\##{emeka}##"
    "\{emeka}"
    "${emeka}"
    "{9*0} {{  }}"
    "\${2*2}"

    "@{emeka}"
    "${emeka}"
    
    iter 4 0 : (default, =, .step ::= 2)
    r_l(+)
    type int32 match
    type match int32
    (1, 2, 3 4, 5)
    f(1, 2, 3, 4 5)
    f(1, 2, +, 4, 5)
    f(1, +2)

    type int32 match ::= 3
    
    Error Recovery: is a process taken by the parser during the parsing of a production-rule when an unexpected token is found.

    Error Recovery Strategies in nc:

    1. Selfishness: is when the parser exhibits a degree of selfishness for the expected token by checking if the token after the unexpected token
     matches the expected token. If true, the unexpected token is skipped with a meaningful error being logged, then parsing continues as if the
     expected token was found first.
    
    2. Altruism/selflessness: is when the parser exhibits a degree of selflessness on behalf of the expected token that wasn't found by following
     to rudimentary conditions:
     - If the unexpected token is the next expected token to be found, the unexpected token should be left alone
     - If the token after the unexpected token matches the next expected token, the unexpected token is to be skipped
    
    3. Cradling: is the process of limiting the amount of errors that get logged when an unexpected-token throws off parsing by skipping the parsing
     of other entities in the production-rule. This is only successful when step(2)[altruism] returns true when any of its rudimentary conditions are
     met, despite the actual expected token not been found. So in a way, it relies on deception

    Cradling??????

    Altruism should cover the entire production-rule
}

panny(typeof(class_mut) me) //for getting the type of a class easily
{
    #(1, 2, 3, 4) | fcn

    iter e : (1, 10, -2)
    {

    }

    do: type int a(3)
    emeka(), #()|emeka
    while true;
}

fn main()
{
    type fn (type int, type int)type int ::= main
    type fn (int32, int32, int32) bin32
    type (int32, int32, int32) bin32
    guess g ::= fn();

    fcn(fn open(){})
    type fn(u8string)int32: "imut_lref" a ::= fn master(){}
    type fn(u8string)int32: "imut_lref" a ::= fn(){}

    plain_ptr<fn(u8string)int32> g ::= fn(){}

    fn gas(int32 a, int8 int8){}(2, 0`i8)

    gasup(fn kase(){})
    fn(){}()

    //temporary object of function type
    type fn(int32)int32 {}
    //unnamed function
    fn(int32)int32{  }
}

`{
    [Slovable Notes]
    * Remember to add a condition in character function for getting the size when unicode name(\{}) and entity(\()) are introduced
    * unicode name and entity entry would be left for another time
    * unicode name check and replacement with the code-point entry be addressed
    
    * Try to outline what the production-rule expands to when logging like in <r_l>, when <r_l_arg_list> is not found or <relational_op>
    * [panic], output and input shouldn't be a reserved word no more
    * add the colon-type thing for <enum_decl>
    * Update <jump_statement> and <block_item>'s BNF syntax to reflect the goto_label and lable changes
    * Conditional expression must always have an else expression
    * An optimization for conversion between bases should be implemented for integer and real number lexing,
        so that only numbers within nc's range are converted
    * Change the outcome of what happens when a multiLine comment is unterminated
     - instead of that, do something similar to rust's raw string output that has the [...] stuff in-between
    * Think more about this : [type ::int32][type int32::fromType int]
    * A range operator for numbers -> [1..3]
    * A pair operator for entry of value pairs -> [ e:3 ]
    * literal types should be entered and directly parsable by the lexer
    * rebrand the conditional operator to this: <expression> '?' '(' <expression>|<jump_statement> ':' [<expression>|<jump_statement>] ')'
    * find a fix for something like: (+) or r_l(+)
    * add support for lambdas: syntax 'lambda' [ '['<expression_list>']' ] '('<parameter_list>')' <block>
    * provision for platform agnostic integers: for signed(int) and for unsigned(uint); respective literals are: i and ui
    * think more about how allocators would fit in my language, specifically arena-wise allocation
    * addition of grouped initialization: guess (a, b, c) ::= type unamed_pt{1, 2, "e"}
    * add catch block; syntax: 'catch' ['(' [<parameter_list>] ')'] <block>
    * a special marker should be used to distinctly identify a variable as an expression: asExp(type malloc_ptr<int32> a())
    * add the file name of the current file it is compiling, like: current file is main.nc

    * support for range loops; possible syntax: 'range' <identifier_list> ':' <expression_list> <statement>
    * add support generics, varadic generics and specialization of said generics
    * constraints should be added to enrich generics after things like ncType and ranges have been defined concretely    
    * think about how multiple files would interact with one another[nc_impl, nc_inf, nc and (the import system)]
    * think about how attributes should be and used in my language(they should have namespace like syntax though: function_body::forLoop::eval_once)
    * arena_ptr and malloc_ptr are the two heap allocation objects we have in nc
    * think more about slices
    * reflect more on the part of the static analyzer that enforces memory safety
    * think about error handling for my language .i.e things like: error_code, error_msg, panic, exceptions and so on
    * a general concept is needed to classify range data structures and that concept would be iterators
    * a negative index should be interpreted from the back for subscripting
    * support for interpolated strings - for embedding expressions within strings: "Emeka is = [$|@]{2 + 3}", then either $ or @ would need to be escaped
    

    * compiler ability to force things like: [must be a return value of a function] or [must call a particular function after object initialization]

    * template parameter should not contain an expression, because compile-time ability to evaluate expressions is already present
        {
            type f_array<2> a()
            ``and
            type f_array a(2)

            are the same if the parameter has been declared as compile-time

            fromType f_array::_construct_(&[compileTime] type int32 size);

            because i don't want let there be confusion on things like this; and due to this, marking a function as compileTime or maybe_compileTime
            is not going to be supported

            figure out how the size of a data structures would be taken at construction, i.e. type f_array<#int32, 2> a(1, 2) =? type f_array<#intt32> a(.size: 23, )
        }
    * match expression should be like this:
        {
            match a: i
            {
                case type int32:"immutable":
                case 23:
                case fcn()()[23]:
                case(23, 3, 5):
                case(34):
                case(type int32:"immut"):
                default:
            }
        }
    _______________________________________________________________________________________________________________________________________________    

    something like this is very dangerous:
    {
        type astring a() type uint count()

        a.assign(a.begin() + count, a.end())

        ``because function @astring::assign(type astring:"immut l-ref", type @astring::iterator, type @astring::iterator) dealloctes itself
    }
    _______________________________________________________________________________________________________________________________________________

    [Unsolvable Notes]
    * temporary objects will not be allowed to have type qualifiers
    * think about how [defer] can be implemented
    * they should be like a function that gets executed at the start and end of an nc program like the .basrc script
    {
        type io::fd cout();
        attach_at_begin(cout.boolalpha)
        attach_at_end(fuck_off_fcn)
        
        main()
        {

        }
    }
    * for classes, allow the default function parameter be a member variable not like c++, i.e. type uint32 e ::= me.absoluteColumn
        {
            The above won't work, just tell the user that default arguments are compileTime constructs while member objects are runtime
            constructs, but it can be achieved by overloading
        }
    * warn about function identifiers not being called
    * varadic agruments should be entered starting with are reserved-identifier
    * moving should be the default instead of coying
    * when implementing unsigned addition, make it like this: max(lhs, rhs) - min(lhs, rhs); to prevent reckless overflow
    * when converting from a lesser signed to a greater unsigned integer, disregard the sign and perform a clean conversion
        {
            (type int8 -> type uint32) != (type int8 -> type int32 -> type uint32)
            but this:
            (type int8 -> type uint32) != (type int8 -> type uint32)
        }
    * How would the compiler differentiate between something like this:
        {
            type u8_str e('2'u8, '4'u8, '3'u8)
            type u8_str d(e.iterator_start(), e.iterator_end())

            what if i want the template type to be an iterator

            template<varadic T> _construct_(type T t);
            template<T iterator> _construct_(type iterator start, type iterator end)

            same problem goes for how the size would be inputted
        }
    * built in static array, dynamic array, list, map, uo_map, stack, queque, set: all data structures i hope to implement
    * functions for integers
        {
            - zero
            - isPos
            - mem_layout
        }
    * algorithmns for data-structures
        {
            - shift_left
            - shift_right
            - count_sequential
            - find_from : takes an index to start finding from, then returns the result plus the offset to get an accurate index
                {
                    - drawbox_around_text::spacing needs it for handling of newline inputs
                    - in file ncLog.h slipt needs it to locate the delimeter(m)
                }
            - slipt
            - during comparsion, a pair should be taken indicating the start position for comparsion for the two operands, similar to find_from
        }
    * my own regex implementation that embodies clarity and brevity - inspired by EBNF
    * my own list implementation that provides random access without that need of an internal array
    * allocation of stack memory would be location dependent, unless in a loop, which would therefore allow the specification the size to be a runtime value.

    _______________________________________________________________________________________________________________________________________________

    The advent of high-level abstractions should not cost little to no overhead unless need be
    _______________________________________________________________________________________________________________________________________________

    *11110000 10011111 10001100 10111001
    *            11111   001100   111001 -> correct
    *            11111     1100 00111001 -> wrong
    *
    *
    *[11110]000 [10]011111 [10]001100 [10]111001

    match e {
        case 1, 2, 3, 4: {
            
        }
        case int_range(1, 4):{

        }
    }

    match type unamed_st(1, 2.333`b32) : i{
        case type int32, type bin32:{

        }
        default:
    }

    for i, j, k : (!{default, 0, 0}, <, 32, !{1, 1, 1})
    io::stdout.write(i)

    for i, j, k : (!(23, 0, 0), 32, !(1, 3, 5))

    true or false and true or false
    (true or (false and true)) or false :[ C/C++ parse]
    true or (false and (true or false)) :[ nc parse ]

    imbed(C++)
    {
        #include <stdio>

        void main()
        {
            int a;
            printf("%d", 23);
        }
    }

    imbed(html)
    {
        <head>Emeka</head>
        <p>I have been barren for a long time</p>
    }

    u8"""
    erre
    dark days are coming
    "Emeka Daniel"
    """

    u8"Emeka Daniel is amazing"

    u8"
    Emeka Daniel at 2003
    \"Emeka\"
    \n
    "
    a"
    ${3, 3}
    \u{
        Emeka Daniel
     }
    \u[2222'222]
    "
    «Emeka, Daniel»


    arg(1, 2, 3, 4, 5) | fcn
    ag(1, 2, 3, 4, 5) | fcn
    fcn(1, 2, 3, 4, 5)
    fcn(d(b(c())))
    c() | b | d | fcn

    d(i, f(e, 9))
    i.::d(e.::f(9))
    
    Logical operators in nc are divided into two categories, [Boolean-logical-operators] and [Bit-logical-operators]
    List of the operators:
    - and
    - or
    - not(!)
    - nand
    - nor
    - xor
    - nxor
    + ∧=
    + ∨=
    + ⊻=
    + and_eq
    + or_eq
    + xor_eq
    + nand_eq
    + nor_eq
    + nxor_eq
    
    The following are to be used for both boolean and bit logical operators

    Expanding upon the traditional set of operators found in C++ can indeed add versatility and expressiveness to your programming language. Here are a few suggestions:

    1. **String Concatenation Operator**: While C++ supports string concatenation using the `+` operator, having a dedicated concatenation operator like `.` in languages such as Python or `&` in Visual Basic could enhance readability and consistency, especially in contexts where string manipulation is prevalent.

    2. **Safe Navigation Operator**: Popularized by languages like Kotlin and Groovy, a safe navigation operator (often denoted as `?.`) can simplify code when dealing with potentially nullable references, allowing for more concise and safer navigation through object hierarchies.

    3. **Elvis Operator**: Also inspired by Kotlin, the Elvis operator (`?:`) provides a concise way to express a default value when dealing with nullables. It's often used as a shorthand for conditional expressions.

    4. **Range Operator**: Languages like Swift and Ruby offer range operators (`..` or `...`), which facilitate iterating over a sequence of values within a specified range. This can be particularly useful in scenarios where iteration over a range of numbers or characters is common.

    5. **Membership Test Operator**: Adding an operator like `in` from Python, which checks for membership in a collection, can simplify code when dealing with collections and sets.

    6. **Bitwise Rotation Operators**: While C++ provides bitwise shift operators (`<<` and `>>`), adding bitwise rotation operators (`<<<` and `>>>`) as found in languages like Java or JavaScript can be useful for certain bitwise manipulation tasks.

    7. **Matrix Multiplication Operator**: With the increasing use of linear algebra in programming, having a dedicated operator for matrix multiplication, such as `@` in Python, could improve readability and maintainability of mathematical expressions involving matrices.

    8. **Pipeline Operator**: Inspired by functional programming languages like F#, Elixir, and JavaScript (with proposals like TC39's pipeline operator), a pipeline operator (`|>` or `>>`) allows for chaining functions or transformations in a clear and concise manner.

    9. **Null-Coalescing Operator**: Commonly used in languages like C#, the null-coalescing operator (`??`) provides a succinct way to handle null values by returning a default value if the operand is null.

    When considering adding new operators, it's essential to maintain a balance between expressiveness and complexity, ensuring that the language remains intuitive and easy to learn while offering powerful features for developers. Additionally, thorough documentation and examples can help users understand and leverage these operators effectively.

    Sure, logical operators are used to perform logical operations on boolean values. In most programming languages, common logical operators include AND, OR, NOT, NAND, NOR, XOR, and XNOR. Here's a list of these operators along with their symbols and descriptions:

    Logical Operators[Boolean and Bit Logic operators ]

    1. **AND**:
    - Symbol: `&&` (double ampersand)
    - Description: Returns true if both operands are true; otherwise, returns false.

    2. **OR**:
    - Symbol: `||` (double vertical bar)
    - Description: Returns true if at least one of the operands is true; otherwise, returns false.

    3. **NOT**:
    - Symbol: `!` (exclamation mark)
    - Description: Returns true if the operand is false, and vice versa. It negates the input value.

    4. **NAND** (NOT-AND):
    - Description: Returns true if at least one operand is false; otherwise, returns false. It's the negation of the AND operation.

    5. **NOR** (NOT-OR):
    - Description: Returns true if both operands are false; otherwise, returns false. It's the negation of the OR operation.

    6. **XOR** (Exclusive OR):
    - Symbol: `^` (caret)
    - Description: Returns true if exactly one operand is true and the other is false; otherwise, returns false.

    7. **XNOR** (Exclusive NOR):
    - Description: Returns true if both operands are either true or false; otherwise, returns false. It's the negation of the XOR operation.

    Certainly! Here's a comprehensive list of various text editing options you can use with ANSI escape codes in a terminal:

    **Text Color:**
    - Black: `30m`
    - Red: `31m`
    - Green: `32m`
    - Yellow: `33m`
    - Blue: `34m`
    - Magenta: `35m`
    - Cyan: `36m`
    - White: `37m`

    **Background Color:**
    - Black: `40m`
    - Red: `41m`
    - Green: `42m`
    - Yellow: `43m`
    - Blue: `44m`
    - Magenta: `45m`
    - Cyan: `46m`
    - White: `47m`

    **Extended Colors (8-bit):**
    - Use `38;5;<color_code>m` for text color
    - Use `48;5;<color_code>m` for background color

    **Extended Colors (24-bit):**
    - Use `38;2;<r>;<g>;<b>m` for text color
    - Use `48;2;<r>;<g>;<b>m` for background color

    **Text Styles:**
    - Reset: `0m`
    - Bold: `1m`
    - Underline: `4m`
    - Italic: `3m`
    - Blinking: `5m`
    - Inverse: `7m`
    - Conceal: `8m`
    - Strikethrough: `9m`

    **Special Effects:**
    - Reset background: `49m`
    - Reset text color: `39m`
}`