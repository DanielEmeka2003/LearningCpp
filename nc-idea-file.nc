fn format(type u8str x)
{
    guess cut_string ::= x[type pair(x.find('i'), x.find('.'))]

    cut_string.insert("XXXXX", 2)

    ``Are none and nullptr that important to be reserved identifiers for their types?

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
}

fn panny(typeof(class_mut) me) //for getting the type of a class easily
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

    match e:i{case 2: {}; default: 34}
    match e:i{case(3, 4, 5): emeka}
    println!("{}", match e:i{case 2: {} case<int32>: 2 default: 23})
    
    array[type pair(0, 3)]
    array[0..3]
    (0..3).to_it()

    guess array ::= type f_array(3, 10112₃)
    guess (a, b, c) ::= array
    How will i:
    1. Make **a** an immutable reference to element[1]
    2. Make **b** a mutable reference to element[2]
    3. Make **c** copy it's value

    Can be done if the they are directly initialized:
    guess (a, b, c) ::= #(imut_lref array[1], mut_lref array[2], cp array[3])

    guess a ::= &imut a;
    guess a ::= imut a;
    gmem -> is back

    :(2, 3, 4)|fcn
    #(2, 3, 4)|fcn

    iter _ : (1, 3, -1) output(2, 3)
    rng _ : (1~7) output(2, 3)

    named_pt red, blue, seal{}
    named_st color{type red, type blue, type seal}

    fn current_color(type color c) type any<?>
    {
        match c:i{ case<red, blue, seal>: give type any(i) }
    }

    fn main()
    {
        guess curr_color = current_color()
    }
    &d.goand

    iter i:(desired_digits, 0, -1)
    {

    }
    for infer i := desired_digits; i < 0; --i:
    {

    }
    3,,4
    3,,5
    3,:4
    3,:3
    3~3

    b = $(2, 4, 5, 6)
    $(2, 3, 4, 5, 6) + $(2, 3, 4, 5)
    type ustr a()
    a[3,,5]

    infer a := 2, 3
    guess a := 3, 4
    auto a := 3, 4

    #[list_name_space_content(boost, no_namespaces, no_functions)]

    3,,4
    3,,4
    3~4

    - value references would different type different from non-value-reference, only it's mutability status would not be different
    - stabilize the above: type si32:"imut [&]"
    
    fn var_arg(...)
    fcn(1, _, _, 3)
    fcn(1, default, default, 3)
    ::@si32
    nc(_, _, _, .start := )
    nc(_, _, .start := 1)

    \"emeka daniel"u8 := 3
    \"emeka & daniel"p := 5
    "emeka"u8
    u8"emeka"
    '÷'u8.info()
    u8'÷'.info()

    \"daniel & esther's bread"u8.
    ``it shouldn't declare another literal when using as an identifer
}

`{
    [Slovable Notes]
    * My language needs to be exception safe - read that rust's article about it
    * Alias should take an <expression>
    * How would i be able to return types?
     - typeof()
     - { type unamed_st<si32, si4> e; if e.get_current_type(): {}? }
    * Think about how comparison of types with mutability should go
    * Remove <jump_statement>s from conditional-expression
    * No more boxed attribute: type si32:"[&]"
    * Store the unicode-code-point along the utf8 character
    * Think more about which things can be parsed as expressions, for example an <if_statement>
    * If/loop-constructs statements should only just accept expressions only
     - what of else if constructs
    * For nc-happy-size types, copying is the default
    * Something like this '\_' is parsable? 👀
    * What would not be the discard identifier?
    * '_' should be the default value identifier not the discard value
    * Should i support any order function parameter entry like in mojo and python?
     - {
            fn e(type si32 a, type b32fp l, type d32fp k){}
            e(.k := 2, _, _)
       }
    * Provide both the 'mut' and 'imut'
    * Remove this @::si32
    * How to design meta-programming
     - like get line number?
     - get identifier names
     - get function-give-type
     - get name_space contents
    * Change guess to infer
    * I need a refined semantic for <objetc_exp>
     - Lifetime
     - Being used as function calls to supply multiple arguments to named objects, redefine things like move and copy constructor
     - Being used as standalone expressions
    * Cement semantics around function declarations and how they can be treated like objects
    * Should i create syntactic suger for working with sumTypes?
    * Conditional expression should have a compulsory else expression
    * The enforcement of using [mut &<expression>] should have a clear semantic definition, that allows override when it becomes inconvinient
     - {
            ``instead of:
            io::out(mut &2, mut &3, mut &4, mut &5, mut &6)
            ``do this instead:
            io::cout(2, 3, 4, 5, 6)
       }
    * Create a concrete semantic definition around initialization, should they be allowed to be initialized after the fact?
     - {
            type si32 s
            if i == 3: s = 2 else s = 4
            b = i == 3?(2:4)
            b = { if i==3: give 2 else give 4 }
       }
    * [3,,4] for pair expression and [3~4] for inclusive range
     - how would i parse them, as operators? or sole expressions?
    - Change cp to copy, mv to move and imut to mut. And think about how i should parse them
     - like this: copy(a) + b or this: copy(a + b)
     - Liken them to how rust parses their own
    * Really think about what and what compiler communication interface can do
     - should it be used in an expression
     - should it deduce function return types #[fn::getGiveType(dama)]
     - they should embody meta programming and nothing else right?
     - they shouldn't be used to affect async and await right?
    * Should defer accept multiple arguments?
    * Pre and post condition entry
    * Change what happens when a real-number literal type is found on an integer and vice-versa
    * Since assembly languages compute the result of a division as it's quot and remainder, find out if llvm provides a similar function
    * <alias_decl> should accept an <expression>
    * should the nc-decimal-fp format be refactored?
    * When validating real numbers, dynamically pick the right nc-decimal-fp to use depending of the precision and exponent size, only when it isn't
     in deadzone mode
    * Why is odd an operator
    * In real number validation, i don't need to normalize nc decimal floating implementation twice, so change it
    * check if rust does that annoyning stuff of this std::uint32_t(std::int8_t{-3})
    * reason more about the sumTypes in my language, becuase initialization of them are horrendous compared to rust's way
    * since we already know what type the identifer has, assignment to accept an arg
     - and also, me function assign synmonous with assignment
    * change i32[...] to si32[...]
    * nc should be immutable first because a programmer should have a clear reason while coding of what object needs to be mutable, it essentially forces the programmer to reason more about his/her code
     - Due to that, standardize how the new [mut] keyword is going to be used
    * iter has a problem down below:
    * can i write something like this: [for (auto i = operand_digit_count - desired_digits; ; ){}] in nc
    * make looping dynamic using compiler attributes like:
     - in for and iter loop, the modifier should not always be at the end
     {
        iter i:(mut 5, 1)
        {
            #[loop::after_modification_do_this]
            {

            }
            ``or
            #[loop::modify_instead];
        }
        
        for infer a:= 5; a < 1; --a:
        {
            #[loop::after_modification_do_this]
            {

            }
            ``or
            #[loop::modify_instead];
        }
     }
    - in while to help when [continue] is used and it skips the modification expression there causing an infinite loop
    {
        while a < 7:
        {
            if a == 5: continue
            #[loop::modification_exp_mark] ++i;
        }
    }
    * swapping should use that xor way

    * check how you did the syntax for the C for loop
    * block is going to be parsable in the global space for things like match os::which_os {case(os::windows): case(os::linux): case(os::mac): }

    * should a <block> accept <type_space> and <name_space>?
    * Look for a more convenient name for guessing type instead of 'guess'
    * Reason about why raw identifers should escape reserved identifers?? what is the purpose
    * Warn about something like this: [__]
    * Addition of mem operator for to replace & which is now a reference operator
    * Adding [type_space] and replacing [namespace] with [name_space]
    * Add an alternate parameter entry: [me, imut me, &me, imut &me] -> ['imut']['&']'me'
    * How would i solve the problem of function overloading semantics example:
     - for objects, if an object can reference an overloaded function through its identifier, should objects with function type have the ability to
       overload themselves using <function_exp> as their source?
     - For overloaded functions and function-objects with multiple call operator overloads, when guessing the function-type, how would that go
    * Addition of <qualified_ident_list> for the declarations that need
    * Integers of variable sizes according to what llvm supports
    * Break character for integer and real number entry, becuase of how large it can potentially be
    * Add support for decimal floating point 8 and 16
    
    * Remember to add a condition in character function for getting the size when unicode name(\{}) and entity(\()) are introduced
    * unicode name and entity entry would be left for another time
    * unicode name check and replacement with the code-point entry be addressed
    
    * Real and integer number literal optimaization checking

    * Try to outline what the production-rule expands to when logging like in <r_l>, when <r_l_arg_list> is not found or <relational_op>
    * Error recovery to be done
     - find a fix for something like: (+) or r_l(+)

    * Support for range loops; possible syntax: 'rng' <identifier_list> ':' <expression_list> <statement>
     - A range operator for number literals -> [1..3]
    * A pair operator for entry of value pairs -> [ e:.3 ] -> [e .: 3] -> [e:.3] -> [e-:3] -> [e:-3]
    * add catch block; syntax: 'catch' ['(' [<parameter_list>] ')'] <block>
     - until i study exception implementation
    * Add the file name of the current file it is compiling, like: current file is main.nc

    * Think more about how expressions at the end of a block would qualify as return values?
    if e * 2 = 3:{
        2
    }
    * [panic], output and input shouldn't be a reserved word no more
    * An optimization for conversion between bases should be implemented for integer and real number lexing,
        so that only numbers within nc's range are converted

    * Provision for platform agnostic integers: for signed(int) and for unsigned(uint); respective literals are: i and ui

    * Add generics, varadic generics and specialization of said generics support
     - Constraints should be added to enrich generics

    * think about how multiple files would interact with one another[nc_impl, nc_inf, nc and (the import system)]
     - i.e think more about the import system of nc

    * Think about how attributes should be used in my language(they should have namespace like syntax though: function_body::forLoop::eval_once)
    * CompileTime code runablity
    * Compiler ability to force things like: [must be a return value of a function] or [must call a particular function after object initialization]

    * arena_ptr and non_arena_ptr are the two heap allocation objects we have in nc
    * think more about slices
     - A general concept is needed to classify range data structures and that concept would be iterators
     - And the common interface for using data-structures
    * Reflect more on the part of the static analyzer that enforces memory safety
     - How safe are pointers going to be

    * Think about error handling for my language .i.e things like: error_code, error_msg, panic, exceptions and so on

    * A negative index should be interpreted from the back for subscripting
    * What happens when a <rng> references a range mutably and one tries to modify the range(collection)?
    {
        guess int32_darr := type d_array(_v, 1, 2, 3, 8, 9, 0)
        rng i : i32_darr
        {
            io::out(i)
            i.erase(2, 3, 5)
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
    * The convention of defining relational operators is to encourage defining only equality and lessthan
    * My string implementation would be [copy-on-write], meaning it initially has a view to the string until a write operation is triggered
    * imbedding of markup languages like html and styling languages like css in my language to be used in the web
    * temporary objects will not be allowed to have type qualifiers
    * They should be like a function that gets executed at the start and end of an nc program like the .basrc script
    {
        type io::fd cout();
        attach_at_begin(cout.boolalpha)
        attach_at_end(fuck_off_fcn)
        
        fn main()
        {

        }
    }
    * for classes, allow the default function parameter be a member variable not like c++, i.e. type uint32 e ::= me.absoluteColumn
        {
            The above won't work, just tell the user that default arguments are compileTime constructs while member objects are runtime
            constructs, but it can be achieved by overloading
        }
    * Warn about function identifiers not being called
    * Moving should be the default instead of coying
    * When implementing unsigned addition, make it like this: max(lhs, rhs) - min(lhs, rhs); to prevent reckless overflow
    * When converting from a lesser signed to a greater unsigned integer, disregard the sign and perform a clean conversion
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
     - it's syntax won't be conventional
     - it will support extraction without checking
     - it give clear error messages if the syntax is wrong
     - it will support error recording of unmatched strings
    * my own list implementation that provides random access without that need of an internal array
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

    d(i, f(e, 9))
    i.::d(e.::f(9))
}`