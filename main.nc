// [singleLine-discard-comment]
/* [multiLine-discard-comment] */
`` [singleLine-doc-comment]
`{ [multiLine-doc-comment] }`

//9.9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999₃₂@+16383`d32 xxxxxxxxxxxxx ppppppppppppp

`{
    Expressions: are entities that associated with a type and define their existence as operands that may or may not return a value
    Statements: are rudimentary lines of code that can only be found in blocks
    Declarations: are language abstractions used for the convientent manipulation of data, which the compiler requires explicit information
    on to setup for the programmer to use
     - Definitions: also called declaration definitions are the concrete bodies of declarations
     - Disclosures: also called declaration disclosures are declarations without a concrete body, so they are abstract declarations such that the
       compiler dosen't posses enough information to setup and therefore allow the programmer to use

    Definitions
    ───────────
    [Type]: a type is simply a classification that dictates how a value is to be interpreted and what operations can be performed on that value.
    Example:
    A memory can contain this: [11110000 10011111 10001100 10111001]
    When interpreted as a signed integer, the value becomes -257'979'207₁₀
    When interpreted as an unsigned intger, the value becomes 4'036'988'089₁₀
    When interpreted as a utf8 character, the value becomes the unicode character rose 🌹
    When interpreted as a dec32 real, the value becomes -nan
    When interpreted as a bin32 real, the value becomes -3.950259@+29
    And so on...

    [Object]: an object in nc is an allocated region of memory(either physical or virtual) that stores a value that is associated with a type.
    
    So the requirement of some memory to be called an object in nc are:
    1. The memory must be explicitly allocated, .i.e it must belong to the process
    2. The memory must be contian a value associated with a type
    
    The term used to refer to memory regions that do not fufill the above two conditons are non-objects

    There are two types of object entities in nc:
    1. <object_decl>
    2. <object_exp>
    
    The above two are futher more classified into a general category:
    1. named objects: objects which posses an identifier
    2. unnamed objetcs: objects which do not posses an identifier

    A named object is basically an <object_decl> entity, which requires an <identifier> as part of its declaration.
    An unnamed object is much more broad, it ranges from elements of data structures to <object_exp> entities.
    Here's a full list of them:
    - elements of data structures
    - return values in an expression
    - <object_exp> i.e type int32(2) + (type int32 ::= 78)

    For clarification purposes:
    To be pedantic all expressions in nc return/give an object as its value
    
    |Object Lifetime|: the lifetime of an object is tied to the scope in which it was defined
    Objects defined in expressions have expression scope
    Objects defined else-where have either block or global scope
    
    [Functions]: a function is simply just a block of code, that can be called arbitary in expressions
    Functions in nc aren't objects, but they can be treated like objects to some degree.
    Because function can be treated as objects, they do not support all the features that functions provide
    Like overloading, example:
    In a block scope, an object identifier must be unique to a type i.e
    {
        type int32 a
        type int64 b

        ``a and b can only be redefined as identifiers in objects with differing types than the original
        ``i.e
        type int64 a
        type int32 b

        ``but functions do not share this limitation, they can be redefined using the same identifiers with a condition that either the number
        ``of parameters are different or the types of the parameters are
        ``example:
        fn gang(type int32 _){}
        fn gang(type int64 _){}
        fn gang(type int32 (a, b)){}

        ``for identifers
        type fn(int32)void gang := fn(int32){}
        ``it works because of the redefination of type stuff
    }

    Example:
    {
       ``Reason about what happens when an overloaded function gets assigned to an object of a concerte type or worse an object of an un-guessed type??
        fn fcn(){}
        fn fcn(type i32 _){}
        fn fcn(type i32:"imut" _)
        fn fcn(i1024 _)

        type fn(type i32)type void a;
        a ::= fcn

        guess a ::= fcn
        ```
        compile error: cannot guess the type because the fcn identifier has too many overloadeds
        ```
    }
    
    [type void]: the nc language's only value-ignorant type

    Understanding value categories and referencing
    ──────────────────────────────────────────────
    Values also known pedantically as objects are divided into two categories:
    - abstract-values
    - concrete-values
    
    [Memory reference] references the memory of concrete-values
    [Value reference] references the value of concrete-values but those concrete-values can be results of concrete-value promotion

    A reference is in nc is an object that simply refers to another object in memory. There

    For a function to be treated as a member function, the first parameter must have the function parameter only type
    The function parameter only type works like this:
    {
        fn example(type any: "[]" me)
        {
            if r_l(type ncType(arg.1), ==, or)()
        }

        - Attribute: "[]" -> is a function parameter only type that performs cvalue promotion only if the type is not trivially movable.
         That is, it is a pseudo-value-reference type that favours the promotion of avalues to cvaules within a function call context, only if it
         is not trivially movable.

        - Attribute: "[&]" -> is a function parameter only type that also performs cvalue promotion but with no pre-condition of it being trivially
         movable. That is, it is a reference type(not pseudo-reference) that promotes rvalues to lvaules within a function call context

        - Attribute: "imut []" -> the same with {attribute:"[]"}, but immutable
        
        - Attribute: "imut [&]" -> the same with {attribute:"[&]"}, but immutable
    }
        {
            lvaules always get match to &me
            rvalues always get matched to imut &me

            I need a way of manipulating 
        }
    'imut', 'mv' and 'cp' are lower-marker-prefix-operators parsed as expressions
    '&' is the only higher-marker-prefix-operator that is parsed along with expressions
    
    cp a = b
    imut a * b
    (imut &a) + b

    imut: is a lower-marker-prefix-operator that can be used in type-guessing and immutable-reference inference

    [<object_exp> are not allowed to have type qualifiers]->[they are now allowed]
    (guess ::= &a).fcn() -> has something to do with value-referencing
    type i32(&a).fcn()

    also how can literals have call member functions when they are rvalues: 2.set(2), "33".insert(0) and so on

    [A special root compiler attribute that relaxes some of the rules of the language, example: instead of write(imut &"emeka", imut &3)]
    
    fn dance(&me, i32 (a, b, c)){}
    type_space(Emeka)
    {
        fn dance(me, i32, i8){}
        fn dance(me, i32, i8){ #[copy_function_body_from(dance, fn(&me, i32))] }

        fn normalize(type: "[]" back){}
        fn dance(type: "[&]" me){}
    }
    
    Solidify the entities that require the type prefix
    {
        - Everthing except type argument list and function signature
    }
    Solidify default contruction/initialization
    {
        ``a combination of rust's and c++'s model
        type i32:"imut&" a := imut &23
        type i32 b()

        guess a := &23
        guess b

        if b == 3: fan()
        if b = 3: fan()
        b := 3
        guess a := 2
        :(2, 3)
    }

    name_space E {}
    type_space E {}
}`

fn jester() void
{
    me
    
    match e:i
    {
        case<int32, int64>: outputf("{}", i * 34)
        case<>: outputf("{}", i)
    }

    fn(){ 2*3 }

    //iter 2 : (default, =, .step := 2)
    //r_l(+)
    //type int32 match
    //if
    //r_l()()
    //r_l()+3
    //type daniel &daniel := 4
    
    //r_l(90)(,)

    r_l(90, ==, and)()

    fn sum() type void {}
    named_pt emeka{ type int32 a, u8string (a, b, c) := () }
    //guess: "immut-lref" a := mut_lref k
    

    match k
    {
        case(3):
        case():
    }

    default
}

fn sum()
{
    `{ multi-line comment `{}`  }`
    ``single line comment ⟨⟩ `% - %` `!

    ``for none : (default, ≠, 23)
    
    io::cout.write("emeka \[2'FF'0] ")

    type int32:"imut&" a := 34
}

``enum theta: int32 { ead, error, fase := 23 };

`{
    name_space Awesome::Daniel::Main
    {
        named_st daniel
        {
            type int32,
            type int64
        }
    }

    named_st 🛺_isPoor ``sumType declaration
    { bin32fp, u8string }

    named_pt 👀_😈 ``productType declaration
    { int32 a, pstring c }
    
    fn 👩🏿👀(int32 a(23), int32, u8string g ::= "er") void ``function declaration
    {
        while (true)
        io.sing()
    }
    guess (a, c, d, e) ::= arg(2, type int32(3), 3, 0.)
    #(2, 3, 4, 5)|fcn
    arg(2, 3, 4, 5)|fcn
}`

fn language() ``fixing accidental C-style syntax and expression-list error
{
    ``f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);

    d.fcn();
}

//👩🏿 = 👩+ 🏿

fn main() int32
{
`{
    !((int32, achar)int32) a := 9
    type fn(int32, achar)int32 a := main
    type fn(i32, achar)i32 a := 9

    2 + {5 = 9 - 8 * 0}
    {3} + 5
    {4} - {if(8) give {0}}
    2 + 3? 1:8 *3;
    -*34
    e({}, {+5})

    r_l(=, and, 23)(12, {23}, 8)
    a(2, 6, {})

    tcast<i32> bcast<i32>()

    iter _ : (default, 23, -2, ≠)
    io::cout.write("emeka")
}`
    io::get_string()
}

`{ --commented-out--
name_space A
{
    name_space B
    {
        name_space C
        {
            name_space D
            {
                name_space E
                {
                    sumType ErrorCase;
                    type_alias E := !(()int32);

                    named_st errorCase;
                    $(2, 3, 4, 5)|f
                    lo(jk(fg(fc(fcn(2, 3, 4)))))
                    $(2, 3, 4)|fcn|fc|fg|jk|lo
                }
            }
        }
    }
}

"
\#{emeka daniel\l\j}#  \##{"emeka daniel\c\m\l}##
\##{"emeka daniel\c\m\l}##
\[1'114'111₁₀]
"
}`

enum Father : type bool
{ daniel, diego := 23, gatsby(-23) }

enum gender: type int8 {male, female, other}

type_space Father, gender
{
    fn which(&me)
    {
        me.value_as_string()
    }
}

named_pt red, blue, green{}

named_st color {red, blue, green}