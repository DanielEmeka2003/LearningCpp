fn main()
{
    for e : (1, 10, -2) ``I might want to specify an incremental or a decremental operation other than an operator to use, example in the incase of the integer, a function that allows overflow
    {}
    .step_control= +=
    .step_ctrl= si32::overflow_add()
    .step= e += 3
    : What of a comparison function?
    .comparison= #op<
    .comparison= islt(me, 34)

    1234343'@-334;

    `` From swift, try to replicate in nc
    let arrayOfArrays: Array<Array<Int>> = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]

    - normal arrays in c++ cannot take arrays of different type than itself
    - std::arrays in c++ cannot take arrays of different size
    - nc arrays can

    + Try accepting input from both (linux and window)s system, to test what encoding they use
    + There should be a platform agnostic way for file entry($root::user::document::learningCpp or $root::home::emeka::document::learningCpp)

    type atom a(type nucleus(type proton(46), _), type electron())
    while true:
    {
        printf("{atom}")
        printf("%0 + %0", 1)

        use mod thread
        thread::sleep(12`sec)
        atom.energy_spike(3.4`kv)
    }
}

`{
    : Self referential types Like lists and tree structures(ast for example)
    {
        : Circular referencing is not allowed, read up on it

		struct node<type t> { type *node<t> next, type t value }
		type node<si16> a(34`si16, .next= type )

		: Something like this is not allowed for inferred argument entry
		{
			fn _init_<type t>(type some<t> _){}

			fn _mInit_()!
			fn _cInit_()!
			fn _cleanup_()!
		}
    }
    : There is no need for casting since typespaces can be extended to have more constructors
    : Not pertinent to the problems of nc
    {
        Encapsulation: information/data hiding
        Abstraction: is the hiding of unnecessary complexity
        Interface: is a shared communication point that allows different but functionality related entities to interact with those functionalities

        The advent of high-level abstractions should cost little to no overhead, and if it needs to, a compromise should provide

        How i will type operations be treated when meta-programming?

        ranges: what is it?
        Type Concept: is an abstract idea that groups types into specific categories
        : is an abstract idea that groups types into specific categories, to operate on them in uison
        : an idea or mental image which corresponds to some distinct entity or class of entities, or to its essential features, or determines the
        application of a term (especially a predicate), and thus plays a part in the use of reason or language.
        a general interface?

        [progressive disclosure complexity]
    }
    : How would type any and sum work? Because these are things that need to give/return types
    {
		: They don't wotk like that
        : How would i store the current type of a sumtype
        {
            named_p red, blue, green{}
            named_s color{type red, type blue, type green}

            type_space color{
                fn current_variant_type(me){
                    type plain_ptr<si32> \"si32*"
                }
            }
        }
		#statement::\"str->code"
		#decl::\"str->decl"("struct made{}")

		index.ncml
		{
			use mod timer
			struct s_counter { type si32 x, type chrono::clock c }
			type s_counter c()
			fn action()
			{
				c.x += 1
				io::out.writef("%{c.x} ;;; %{c.c.now(_)}") ``variadic one or more would not allow this
			}

			<mouse>
			{
				onClick: action()
			}
			<p>
			{
				action() + 34
			}
		}
    }
    : New syntax emission for both object expressions and functions
    {
		<type> [<initializer>]
		'fn' <identifier> ['('<type_parameter>{',' <type_parameter>}')'] <block>
		'fn' ['('<type_parameter>{',' <type_parameter>}')'] <block>
    }
    : For resolving overloaded identifiers [to<*>(*)]
    : Define the scope of what gets seen globally in a scope? only objects cannot be seen globally
	{
		: Search what lexically scoping means
		: And what rust, go and other modern languages call their type of scoping for function and types
	}
	: Only way to discard a value
	{
		infer _ = <expression>
	}
	: Statements parsing
	{
		: All statements [if, while, do, for, match and jump]
		: <match> statement new syntax
		{
			<match_branch> := 'br' <statement>

			'match' [<object_decl> {',' <object_decl>} ';'] <expression> [':' <identifer>|'('<identifier_list>')'] '{' <match_branch> {',' <match_branch>} '}'
		}
		: <for> statement new syntax
		{
			'for' [<object_decl> {',' <object_decl>} ';'] <identifier> ':' ('('...')' | '|'...'|') <statement>

			for x : |&mut collection| {}
		}
    	: Statements [if, while, for and do] when used as a substatement must be done so in blocks
		: Addition of U+D7 as the alternate multiplication operator
		: How should jump_statements be parsed?
		{
			- 'give' [<expression>]
			- 'break'|'continue' ':' [<integer_number_literal>]
		}
	}
    : Primitive string and character definition
    {
		: Primitive type u8str
		{
			Memory model:
			- If it is defined in the stack space and the size is below 64, it would be allocated on the stack
			- Else it would be allocated on the readonly section of program memory

			Description:
			It has two and-fields, the data, it's size and it's character size, the data stores string values as a utf8 encoded bytes
		}
		: Primitive type char
		{
			Description:
			It has just one and-field, the codepoint
		}
    }
    : Multiline string and integer parsing
    {
        : For strings
        {
            : [ \ ] would be used to escape any whitespace after it, leading and trailing newline are ignored
            {
                infer y = "     \
                emeka
                "
                infer x = "     emeka"

                assert(x == y) ``assertion is true
            }
        }
        : For multiline integers that dosen't regard leading whitespace
        {
            123243434\
            234334343\
            2343
        }
    }
    : Error handling
    {
        Errors in nc are treated just as values but with special language syntax to make working with them easier.
        A type that uses the special syntax of errors is called an error handling type, the following are constraints of an error
        handling type:
        - The type-creator must be a sumtype of binary or-fields
        - Both or-fields must be a type parameter
        - Neither or-fields are allowed to be an error handling type
		- Neither or-fields are allowed to an optional handling type
		- Neither or-fields are allowed to be a zero-sized type
        - The language treats the first or-field as the expected value while the second or-field as the error value.
        LCI make_error_handling_type directive is used to make type-creators error handlers

        example:
        {
            #dir::make_error_handling_type
            union result<type [expected, error]>{ type [expected, error] }
        }

        : Special error syntax and semantics
        {
            ``using type char's constructor for decoding utf8 bytes, which maybe be failiable in ways such as the ones in wikipedia

            ``trying the expression gives the expected value if it is the current alternative and panics if not
            try type char(*) ``panic if the current alternative is second or-field

            ``propagating the error if it is the current alternative, a multiple type variadic sumtype is choosen to propagate multiple errors in the same function
            prop type char(*) ``propagate the second or-field up the call-stack if it is the current alternative

            ``handling the error section
            type char(*) => (*) {...}       ``handle the error. The block must contain a <jump_statement>
            type char(*) => |*|             ``handle the error by suggesting a fallback value
            type char(*) => |*| (*) {...}   ``handle the error by suggesting a fallback value and doing some extra stuff with the error like logging. The block must not contain a <jump_statement>
            type char(*) => (*) |*|         ``handle the error by suggesting a fallback value from the error
            type char(*) => (*) |*| {...}   ``handle the error by suggesting a fallback value from the error and doing some extra stuff with the error like logging. The block must not contain a <jump_statement>

            ``example of how [try] panics { panicf("on line(%{}), error: [%{}] was found") }
			``NOTE: only expression can use the error syntax
			``All error syntax allows the programmer to treat the error prone value as the expected value
        }
		: Functions related to error handling
		{
			- panic
			- panicf
			- assert
			- exit or libc::exit
		}
    }
	: Optional handling
	{
		The special syntax for error handling are also used for optional handling. The following are constraints of an optional handling type:
		- The type creator must be sumtype of binary or-fields
		- Only the first or-field is allowed to be a type parameter
		- The second or-field must be a zero-sized type
		- The first or-field is not allowed to be an optional handling type
		- The first or-field is not allowed to be an error handling type
		- The first or-field is not allowed to be a zero-sized type

		To make a type an optional handling type, the LCI dir::make_optional_handling_type is used:
		```
		#dir::make_optional_handling_type
		unit
		{
			union maybe<type t>{type [t, void]}

			union optional<type t>{type [t, void]}
		}
		```

		The only special syntax not allowed for it is [prop], everything else is allowed for optional handling.
	}
	: [!] and [type void] semantics
	{
		: [void] is the language's default value-abesent type which is zero-sized
		: [!] is the embodiement of type absence
	}
	: [infer] used in function parameters and as the give type do not have the same meaning
    : CHECKOUT
    {
		print(&1, &2, &45)
		print(&[1, 2, 45])
		write(&[34, 56, 78, 23, 56])
		write(&32, &45, &34)
		eprint()
		error_write("34 + 56 =", 123)
		write(34, &type expensive(34, 5), 90)

		#allow_efficient_pass<t:?>
		fn write<type t:?>(...){}
		fn read() type str{}

        io::write(&emeka, daniel)

		write(&2345999999999999999999999999999999999999999999999999999999999999999000000000000000000000000000000000000000000000000000005`si3456)
    }
    : Resolving identifiers is not needed for delay construction infact it is not allowed
    {
        type [si32, si345, si89] x
		x := type si32
		x := type si345
		x := type si89

		infer [x, x, x]
		x := type si32
		x := type si34
		to<si45>(x) := type si45 ``is not allowed even though it is inituitive
    }
	: Mutability resolved semantics
	{
		: Mutability is a feature of stack objects or objects allocated on the stack
		: Mutability is not a feature of and-fields
	}
	: Think more about how types 'farray' and 'u8str' would be treated
	{
		: Both require static sizes
		: Both require sizes that won't be directly modifyable in member-functions
		: Both are in desperate need of assignment regardless
		: There would be no functions to mutate them except the assignment operators

		: Dealing with type 'u8str'
		{
			: Remember the individual bytes and characters cannot be modified -- "emeka daniel"(3) = 5

			x(2) + x(3) + x(5)
			x(56) => { `* do something *` } + try x(78) + try x(45)
		}
		: Dealing with type 'farray'
		{
			: 'farray' is the special type that one needs if allocation of multiple stack objects is required
			: It's size would both be readonly and static -- meaning the size disappears at runtime
		}
		: A definition of what a primitive type
		{
			: Are types that are provided as builtins by nc-lang and also consists of types that cannot be created directly in user-code
			examples: type [integers, floating-point, farray, u8str]
		}
	}
    : Copy and move semantics
    {
        : Only [copy] would be added
        : What of this -> copy &a
        : Types that have a move/steal constructor are chosen as the default constructor for indirect constructor
        : Since moving is the default indirect constructor to use when it is specified in a type, adding [move] isn't necessary
    }
    : Formula that relates the dividend, divisor, quotient and remainder [divisor * quotient + remainder = dividend]
	: For now <alias> would not accept expressions
	: Introduction of platform integers
	{
		type [si, ui] x
		: How would something like this be implemented without the compiler's help
	}
	: Lexer cannot tokenize the following
	{
		- 2.add()
		- Can literal types take raw-identifiers? example: \"emeka"'👀' or 12343`\"daniel" ?
	}
    : Primitive types integers and real-numbers error handling initiative
    {
		: For integers
		{
			: For integers and their hardware overflow property, they would panic if encountered
			: Integer divide by zero would panic if encountered
			: Functions for signed integers to have control over arithmetic operations
			{
				: For error operations like divide by zero
				- checked functions
				- unchecked functions

				: For overflow
				- saturate functions
				- overflow functions
			}
			: Functions for unsigned integers to have control over arithmetic operations
			{
				: no_overflow_sub
			}
		}
		: For reals
		{
			: Arithmetic operators and math functions would use the already IEEE error values i.e nan, +/-inf
			: For control and alert purposes
			{
				For now it is stalled and nothing is thought of yet
			}
		}
        : Think about the nan types(qnan and snan)
    }
	: Operator removal/addition
	{
		: [$] is now the get-internal-type-representation operator
		: [×] (U+D7) the alternate multiplication operator
		: [memoryof]
		: [addressof]
        : [++|--] going to be removed
		: Total ordering operator: '<=>' would be introduced in nc
		: Add elementof operator to my language[ 2 ∈ 2~4 ]
	}
    : Operator overloading semantics
    {
		Only operators not overloadable are
		- assignment operators
		- pipe operator
		- try and prop
		- value-reference operator

        fn #op+(&lhs, &rhs) infer{
            lhs + rhs
        }
    }
	: Loop mechanics
    {
        std::uint32_t count{};
        for (size_t i = formerFilePos; m_isEndofFile? i <= m_filePosition : i < m_filePosition; ++i)
        ++count;

        type ui32 count()
        for i : (formerFilePos, void)
        {
            if m_isEndofFile? (i <= m_filePosition : i < m_filePosition):
            break

            ++count
        }

        : Warn for while-loops that have true or false as their condition and error for for-loops that don't have a termination condition
        : Warn for recursive functions that dosen't have a termination condition
    }
    : No more iter loop, it's all merged with the for loop now
    {
        : The normal for-loop variant needs to allow functions that also modify the loop counter, not only operators, and cement what the functions
        should so take as expression arguments and what they give also

        : Think more about the for loop for ranges semantics
        {
            : Define the languages iteration model first, go watch that video of iterators in different languages

            for i : |&mut x| {}
            for &i : x print(4)
            //and also this:
            for i, j, k : ([1, 2, 3], 4, 5) {}
        }
    }
	: Iterator model in nc
    {
        : [<expression> ~ <expression>] ranges?
		: What are generators and how can they be implemented with iterators
    }
    : Delayed assignment [:=] semantics
    {
		: Only statements it can be in are sure runtime constructs i.e if-else and match
    }
    : No more end-expression being promoted to the give value of the block
    : If a type is dependant on type parameters due to it's field, during typespace declaration for the type, the type parameter must be present
    {
        struct farray<type t>{...}
        type farray<type _>{} or typespace farray<type t>{}
    }
    : Integer literal fix on what gets put in the constructor and what dosen't
    {
        type si34 x = 12`si34
        type si34 x(12)
    }
    : Type parameter oddities
    {
        : A type parameter can only accept the exact type it asked for, to make a type parameter appilicable to all, use the type extension [?]
        {
            fn fcn<type _>(){}
            fcn<si32>()

            fn fcn<type _:&>(){}
            fcn<si32:&>()

            fn fcn<type _:&*>(){}
            fcn<si32:&*>()

            fn fcn<type _:?>(){}
            fcn<si32>()
            fcn<si32:&>()
            fcn<si32:&*>()
            fcn<si32:**>()

            struct some<type t>{type t:& x}
            struct some<type t>{type t:? x}
        }
        : When an unextended type parameter is used in an expression parameter it can have multiple versions of itself all defined in the expression parameter
        {
            fn fcn<type t>(type [t:&, t:?, t:&*] x){ if #exp::is_same<t, t:?>: print(&["not equal"]) }

            : An extended type parameter cannot participate in such
            {
                fn fcn<type t:&>(type t:* _){} ``ERROR extended type parameters cannot participate in such

                ``Reason is because it is less ambiguous to have this

                fn fcn<type t>(type t:& _){}
                fcn<si32>(&23)

                ``Than this

                fn fcn<type t:&>(type t:* _){}
                fcn<si32:&>(type si32:*(23))
            }
        }
    }
    : I can't take [:=] out because i need it for value referencing construction delay { type heap }
	: Why the assignment operators now give [!]
	{
		Because the rule of all expression being used undermines assignment operators giving themselves after an operation. And now due to this, i
		can now have an additional way to discard values [_ = 23]

		```
		_ = [34, 45, some()]
		infer _ = [34, 45]
		```

		: Assignment chaining in C/C++ are done with grouping syntax instead
		```
		b = x = y = 4			``deals with precedence and associavity
		[y, x, b] = [4, y, x]	``deals normal
		```
	}
	: Special case for 'infer' when used in grouping syntax
	{
		Since the type isn't specified along with the declaration, a grouped construction can act on a single identifier just like expressions can.
		```
		infer x = [2, 3`si34, true]
		```
	}
    : Primitives that are non empty constructed
    {
		- value-reference types
		- function types
    }
	: Should sum types not rely on pattern matching
	{
		?? like supporting other means of matching like using the [if]

		match x: i { br<> i += i }
	}
	: Constructor stuff [join up]
	{
		: Three types of constructors provided by default by the language
		{
			- Empty constructor -> if any other constructor is specified, the empty constructor would not be specified
			- Copy constructor -> if a copy constructor is specified, the copy constructor would not be specified
		}
		: Why copy and move constructors aren't value reference based like they should
		{
			So they would not be limited semantically to simple value-reference and mutability, meaning semantically an immutable object can
			be moved. To also allow for value-reference construction and assignment semantics.

			Also the same reason why destructors aren't value-reference based, due to the semantic limitation of it.
		}
	}
	: Value categories
	{
		- abstract value : are values that do not have some sort of identity to refer to for later use
		- concrete value : are values that have some sort of identity to refer to for later use

		Make a decision whether objects are constructed on the fly and destructed when no longer used or end of block scope
	}
    : What qualifies as an unused value?
    {
        A value that is not used in an expression with the exception of the lhs of an assignment operator, mind you not the compund assignment
		operators.
		Unused values are not allowed in nc.
    }
	: Type reference extension
	{
		type si32: &
		type si32: *
		type si32: ? ``not allowed because it is simply not monomorphic

		: Mutable cannot bind to immutable
	}
    : Value reference bind semantics
    {
		: Binds to everything
		{
			type si32:& x(&45)
			type si3:&*mut(&type si3:*)
		}
		: Type identity duality
		{
			An already constructed value-reference object discards it type identity and only retains it during assignment.
			Example:
			```
			type si32:& x

			#dir::eval if #exp::typeEq<si32, #ty::exp(x)> : print("Ref(㑖)")

			x := &type si32

			#dir::eval if #exp::typeEq<si32, #ty::exp(x)> : print("!Ref(㈲)")

			x = 34 ``the type identity of the value-referenced type

			x = &34 ``the type identity of the value-reference
			```

			This is done in order for an object of value-reference type to mirror the object's type. This is true even for object expressions
			```
			fn fcn(type si32:& _){}

			fcn(type si32:&)``ERROR expected [type si32:&] because the expression above loses it's type identity afer construction
			fcn(&type si32:&) ``correct
			```
		}
		: Value reference types now support object mutability and assignment
        {
			type si32:& x:mut(&34)

			x = &45
			_ = &45
        }
        : Immutable non-reference and reference objects cannot bind to mutable-reference objects
		{
			type si32 x:mut = 34`si32
			type si32:* ptr(addressof x)
			type si32:*mut x(type ui(ptr)) ``forbid something like this
		}
		: Direct and indirect construction for value referencing are the same
        {
            type si32:& x(&23)
            type si32:& x = &23
        }
    }
    : Memory-address reference bind semantics
    {
		: New syntax of memory-address-references/pointers [type si32: *], [type si32: *mut], [type si32: &*] and [type si32: &*mut]
		{
			type si32: &mut*mut
			type si32: &*mut = & addressof x
			type si32: &mut*mut x
			type si32: *mut
		}
		: Get internal type representation operator [$] for memory-address-reference gives an optional
		: Only operator overload for it would be the plus and minus operators
		: Memory address reference constructor semantics
		{
			: No more
			{
				type si32:*mut x = addressof 34
				type si32:* _(x)

				--instead--
				type si32:*mut x = addressof 34
				infer _ = {match $x: (i, _) {br<si32> give addressof i br<void> give type si32:*}}
			}
			: Memory-address reference mutability guarantees
			{
				type si32 a

				type si32:*mut _ = addressof a ``cannot bind, a is immutable
				type si32:* x = addressof a
				type si32:*mut _ = addressof (try $x) ``cannot bind the memory referenced by x because it references it immutably

				type si32:&mut _ = &a ``cannot bind, a is immutable
				type si32:& y = &a
				type si32:&mut _ = &y ``cannot bind, y is immutable
			}
		}
		: Binds to everything except a value-reference
		{
			type si32: *& x ``not even parsable, should be parsable right
			--reason--
			??
		}
        : [addressof] operator would be used to give the memory address of an object
		{
			type si32:* x = addressof 23

			: The address of a memory-address-reference cannot be queried, therefore disqualifying the C/C++ pointer->pointer
		}
        : Then for memory-address referencing regardless of the memory-value type [?:*] is used
		{
			type ?:* x = malloc()
			``getting the internal type representation is not defined for the type

			#dir::language_safety_violation
			type u8:* _(x)
		}
		: Differences between what the ? memory-address reference and typed memory-address reference takes in the constructor
		{
			- [typed:*] takes untyped/any memory-address reference. Is a language safety violation
			- [?:*] takes typed memory-address reference. Is not a language safety violation
		}
		: Getting the memory-address as an integer
		{
			type ui _(addressof 23)
		}
    }
    : Compound statement has additional syntax
    {
        : <match>
        {
            match type si34 z; x : i {}
            match type si34 z; x : (i, _, k) {}
        }
        : <if>
        {
            if type si32 x, type si67 c; true: {}
            if type [si32, si67][x, c]; true: {}
        }
        : <while>
        {
            while type si32 x, type si67 c; true: {} else {}
            while type [si32, si67][x, c]; true: {} else {}
        }
        : <for>
        {}
        : <do>
        {
            do type si34 x, type si67 v; {}
            do type [si34, si67][x, v]; {}
        }
    }
	: No more conditional operator
    : A bitwise shift operator
    {
        8 >>> type pair(2, .base = 4)

        4 >> type math::circular_shift(3, _)
        4 >> type math::norm_shift(2, _)
        4 >> type math::recovery_shift(3, _)

        : Remove [>>> and <<<]
    }
    : Dereference operator is renamed to the get operator
    : No more do-while but just a do-loop similar to rust's loop
    {
        do: #dir::static type si8 x = 4;
        {
            alias size = x;
            type heap<si1024> x(size, .init= 34)
        }
    }
    : Only an unextended type can be aliased
    {
        alias d = type si32:*
    }
    : Various uses of the 'use' reserved-word
    {
        use mixin|interface|mod|pkg <identifier>

        : It would use grouping syntax also
        {
            use pkg std::[io, string, unicode::[], regex_all, nc_regex, regex]
            use [pkg, mod] [std, boost::io]
            use mixin [myMixin, myMixin2]
        }

        : Using modules, pakages, files and directories
        {
            -- not yet
        }
    }
    : How encapsulation would be handle
    {
        [dir::private]       : can be accessed only within the current module. Conversely, cannot be accessed outside the current module
        [dir::private(pkg)]  : can be accessed only within the current package. Conversely, cannot be accessed outside the current package
        [dir::private(type)] : can be accessed only within the type. Conversely, cannot be accessed outside the type
        [dir::readonly]      : can be read-from and written-to only within the current module. Conversely, can only be read-from outside the current module
        [dir::readonly(pkg)] : can be read-from and written-to only within the current package. Conversely, can only be read-from outside the current package
        [dir::readonly(type)]: can be read-from and written-to only within the type. Conversely, can only be read-from outside the type
    }
    : Type parameter specialization is not allowed, instead an inline version is supported for functions
    {
        named_p darray<type t>
        {
            #{t == bool} type bytestream x
            #{t != bool} type heap<t> x
            type ui64 size
        }

		#{give #exp::is_same<si32, t>}
		fn fcn<type t>(){}

		#{give #exp::is_same<si3, t>}
		fn fcn<type t>(){}
    }
    : [type me: &] is the type of the function parameter syntax -> [ '&'['mut'] <identifer> ]
    : Construction and Destruction semantics - even the type argument entry for constructor supposed to be here
    {
        : Constructors give value type must either be a reference to the type itself or error-handling type
        : <struct> construction
        {
            : Has the concept of the following constructors [_construct_, _cConstruct_ and _mConstruct_]
            : For _construct_
            {
                Language provides default empty and member constructors only when these conditions are met, no access restrictions on the member-
                objects/and-feilds and no specification of the same constructor
            }
            : For _cConstruct_
            {
                : Only used when the move constructor is not defined or when explicit copy is requested via the [copy] operator/marker

                Language provides default copy constructor only when not provided by the programmer

                fn _cConstruct_(&mut me, &src) type me:&;
            }
            : For _mConstruct_
            {
                Language only provides a default move constructor if one or more and-fields has one

                fn _mConstruct_(&mut me, &mut src) type me:&;
            }
            : All member-object/and-feilds must be constructed in constructors and no member function is to be called in a converting constructor
        }
        : <union> construction
        {
            Only converting constructors are a concept for sumtypes and among those converting constructor, only two constructors are defined
            - empty constructor which selects a default value for construction
            - a constructor that askes for argument to assign to the sumtype

            of the two the only on that can be specified by the programmer is the empty constructor
        }
        : <enum> construction is the same thing with the <union>
        : Defined identifiers for various type member functions are
        {
            : operators
            {
                - [lhs and rhs] for binary operators
                - [rhs] for prefix operators
                - [lhs] for postfix operators
            }
            : for the first identifier of a member function
            {
                - [me]
            }
            : for the second identifier of copy and move constructors
            {
                - [src]
            }
        }
        : <struct> destruction
        {
            Language provides default destructors only if one or more and-fields have one

            fn _destruct_(&mut me)!

            The language has special semantics for destructors, it coerces any any immutable object to a mutable for destructing this is known as
            destructor value-reference

            Destructors cannot be called explicitly, example
            {
                struct x{ type [str, ui8][x, y] }
                typespace x{
                    fn _destruct_(&mut me){ x._destruct_() y = 34567 }
                }
            }
        }
        : <union> destruction
        {
            Destructors are not to be specified by the programmer, it is handled by the language i.e if the current alternative or or-field has a
            destructor, it would be called
        }
        : <enum> dosen't have the concept of destructorion
        : No more object type parameters, instead there would be type argument entry for constructors
        {
            struct bag {}
            typespace bag{
                fn _construct_<type t>(type si67 x){ type t x = x `*shouldn't i error against this?*` }
            }

            : For objects
            {
                type bag x<si89>()
            }
            : For object expression
            {
                type bag<><si89>()
            }
        }
    }
    : All declarations that have both disclosures and definitions must be defined above all and the disclosures only be for FFI purposes
    : Type-creators disclosures semantics
    {
        : When a type-creator disclosure is found any use of it would be done so through indirection
        named_p s;
        type s:* x()
    }
    : Default arguments that need member data shall be specified like so:
    {
        fn fcn(&me, type os::si end_column = me.end_column)
        {}
    }
    : New syntax for multi-line doc comment
    {
        `*emeka daniel*`
    }
    : What are enums really?
    {
        : Is a type creator that creates a type out of a fixed list of integer-number-literal or character constants
        {
            integer-number-literal have addition to keep it going, but characters don't
        }
        : Similar to
        {
            struct unicode_properties{}
            typespace unicode_properties{
                type ui8 [block, script, whitespace, math][0, 1, 2, 3]

                fn operate_on_unicode_properties(&me){
                    match me{
                        arm(@unicode_properties::block){}
                        arm(@unicode_properties::script){}
                        arm(){}
                    }
                }
            }


            ``but what the programmer is free to enter any value of type si8 they want
            ``example
            operate_on_unicode_properties(45`ui8)
            operate_on_unicode_properties(200`ui8)

            ``but with enums
            enum unicode_properties: type ui8{block, script, whitespace, math}

            typespace unicode_properties{
                fn operate_on_unicode_properties(&me){
                    match me{
                        arm(@unicode_properties::block){}
                        arm(@unicode_properties::script){}
                        arm(){}
                    }
                }
            }

            operate_on_unicode_properties(45`ui8) ``not an enum value of the unicode_properties enum type
        }
        : enum typespaces cannot contain non-member-objects because of collision with enum values
        : Noteable default member function provided by the compiler
        {
            - tobasetype()
        }
    }
    : Object Discuss
    {
        : Because literals are not objects
        No because they refer to data(data-value) itself. An identifier can both be a data-value and an object
        objects give values and values give it's type

        Every literal is a value
        Every identifier is a value
        Every anonymous object is a value

		Object declaration [type si32 x]
		Object/Object-Identifier [x]

		Objects are instances of a type [look into what groupings called them]
    }
    : [######]
    {
        ────────

		: Can one inherit from a memory-address-reference?
		{
			struct ptr<type t>: type *t{}
		}
        : Figure out how to get more metadata
        {
            : member data names
            : enumerator names

            #default_output
            strcut hyper{type [si32, u8str, d32fp][volume, detail, density]}
            typescope hyper
            {
                #default print
            }

            #enumerator_names<enum_color>
            for i : |#enumerator_names<enum_color>|
            {
                i|io::out
            }
        }

        type si32:* x
        type si32:&* x

        type si32:* b(x)
        type si32:* c(addressof x)``address of a memory address reference cannot be queried

        : String use to be merged with the other issue
        {
            type u8str x
            type char x
            type str x

            - copy-on-write or stack-happy string

			[copy-on-write: Needs to know when the string is moved or dies so that it the contents can be copied]

            : Problem lifetime
            {
				type slice y

                fn scholar
                {
					y := type slice(& s"Daniel Emeka")
					...
                }``dies here

                fn use_here
                {
					me.someDarray.pushBack(y)
                    ... ``which causes a nasty runtime error that would be spent hours debugging
                }
            }
        }
        : Singular slice type for referencing of heap data
        {
            type slice x ``how would it be implemented

			: Would be implemented using the abstract requirements for ranges
        }
        : Value reference types are the only odd types in the language
        {
            - It dosen't have the concept of direct and indirect construction
            - It loses it's type identity after construction
        }
        : typescope cannot have userscope, mixin and interface
		: The [@] cannot be used for dynamic and function types
		: Dynamic and function types are banned from typescope extention
        : Open vs Closed system
        {
            C++'s namespace is an open system
            typescope and userscope are closed systems
            type-creators are closed systems along with inheritance possibility
        }
        : [.] is only for type members no more [b.::]
    }
    : Special semantics --add to the bit about constructors and change the name to factory functions--
    {
        Copy and Move Semantics
        ───────────────────────
        Even though the copy and move constructors are defined to take value references, they are a different concept entirely that the language
        dosen't equate to the constructors that are defined to accept them. There are two reasons why the language has them defined that way:
        1. For copy and move to be defined more intuitively on semantics that make it clear when a copy or more is happening
        2. For value reference semantics to work

		Destructors are similar in as the special semantics does not reflect conventional semantics

		When Special Semantics Discards conventional Semantics
		────────────────────────────────────────────────────
		1. Copy constructors
		2. Move constructors
		3. Destructors

        A copy is the default indirect construction unless a move is defined for that type. After a move, the moved value cannot be touched without
        an re-assignment.
    }
    : Because all values must be used, the assignment operators must give [!]
	: Non overloaded operators
	{
		- memoryof
		- addressof
		- assignment operators
	}
	: Introduction of new items
	{
		- [unit]
		- [typescope]
		- [userscope]
	}
    : The memory address of a memory-address-reference|pointer cannot be queried in nc
    : Parameter
    {
        Parameter
        ─────────
        in relation to programming, is a list of one or more requests made by an item to be fufilled when it is used.

        Types of Parameter
        ──────────────────
        1. Expression parameter
        2. Type parameter

        Type parameter
        ──────────────
        is the type of parameter that is used for requesting types. It can only be used in <function_decl>, <struct_decl> and <union_decl> items.

        ```
        fn fcn<type t>(){}

        struct dac<type t>{}
        ```

        Expression parameter
        ────────────────────
        is the parameter type that is used for requesting expressions. It can only be used in the <function_decl> item.

        ```
        fn fcn(type si32 x){}
        ```
    }
    : Argument
    {
        Argument
        ────────
        in relation to programming, is a list of one or more answers to the request of a parameter when it is used. Since there are only two types
        type of parameters, there are also only two types of arguments namely:

        1. Type arguments - (fcn<>()) can be entered without consequence and has no effect on resolving overloads
        2. Expression arguments

        ```
        fcn<si32>()
        dac<b32r>()

        fcn(34)
        ```

        Argument Entry
        ──────────────
        is simply the entry process of one or more arguments to their parameters. Same applies for argument entry types, namely:

        1. Expression argument entry
        2. Type argument entry : If the arguments are zero the angle braces can be omitted

        Argument Special Entry Types
        ────────────────────────────
        1. Explicit default argument entry
        2. Implicit default argument entry
        3. Argument inference entry
        3. Parameter-argument entry

        Explicit Default Argument Entry
        ───────────────────────────────
        parameters can specify arguments for themselves to use, these arguments are called default arguments. To use a default argument, the
        placeholder identifier [_] is used. The use of the placeholder identifier[_] is known as explicit default argument entry. Explicit default
        argument entry is a feature of both expression and type argument entry.

        ```
        fn fcn(type si32 _ = 45){} ``fcn(_)

        struct lose<type _ = si56>{} ``type lose<_> x()
        ```
        For an expression parameter that is based on a type parameter to specify default argument for itself, the type parameter must first have
        a specified default argument.

        ```
        fn fcn<type t= si34>(type t x = 90`si34){}
        ```

        Implicit Default Argument Entry
        ───────────────────────────────
        With implicit default argument entry, the placeholder identifer[_] need not be used only when they are the last arguments to be entered.
        Implicit default argument entry is only a feature of type argument entry.

        ```
        fn fcn<type t = si89>(){} ``fcn<>() or fcn()
        ```

        Argument Inference Entry
        ────────────────────────
        This is the scenerio where an argument of one argument entry type can be infered from another argument entry type, when infered, the argument
        behaves as if it the parameter provided a default argument and therefore is subject to default argument features. Argument inference is
        only a feature of type argument entry, meaning a type argument can be infered from an expression argument. And since it is only a feature of
        type argument entry, implicit default argument entry can be used.

        ```
        fn fcn<type t>(type t _){} ``fcn<_>(45) or fcn(45) [behaves exactly like this] fcn<type t = si32>(type t _){}
        ```

        Argument inference is only possible if the parameter being inferred dosen't already specify a default argument.
        ```
        fn fcn<type t = si90>(type t _){}
        ``fcn<si567>(89`si567)
        ```

        Parameter-Argument Entry
        ────────────────────────
        This type of argument entry simply provides a way yo associate arguments with their parameter identifers. Parameter-argument entry is only
        a feature of expression argument entry.

        Syntax
        ──────
        '.'<identifier> '=' <expression>

        Parameter-Argument entry is not valid for variadic expression parameters and [_] parameters.

        Types of Parameter-Argument Entry
        ─────────────────────────────────
        1. Ordered parameter-argument entry
        2. Unordered parameter-argument entry

        Ordered Parameter-Argument Entry
        ────────────────────────────────
        Is when parameter-arguments are entered orderedly. Because they are ordered, they can be mixed with non parameter-arguments.

        ```
        fn dex(type [si32, si32] [x, y]){ infer [_, _] = [x, y] }

        ``dex(.x= 34, .y= 56)

        fn dex(type [si32, si32, si89, si34] [a, b, c, d]){ infer [_, _, _, _] = [a, b, c, d] }

        ``dex(23, .b= 34, 45`si89, .d= 34`si34)

        ```

        Unordered Parameter-Argument Entry
        ──────────────────────────────────
        Is when parameter-arguments are entered unorderedly. The language allows this because it can be solved easily by requiring that from the
        starting point of the unordered parameter-argument entry, only parameter-arguments are allowed from then on.

        ```
        fn dex(type [si32, si32, si89, si34] [a, b, c, d]){ infer [_, _, _, _] = [a, b, c, d] }

        ``dex(23, .c= 34`si89, .b= 45, .d= 34`si34)
        ``dex(.d= 34`si34, .c= 34`si89, .b= 45, .a= 23)

        ```
    }
	: Identifier Items semantics
	{
		Identifier Item
		───────────────
		are items that declare identifers

		Examples
		────────
		- <mixin_decl>
		- <interface_decl>
		- <type_creators_decl>
		- <function_decl>
		- <object_decl>
        - <alias_decl>

		There are two properties that govern the behaviour of identifier-items namely
		1. Scope
		2. Namespace

		Namespace
		─────────
		refers to the naming space of identifer-items where the language requires each identifier of an identifier-item in a namespace be unique
		so as not to have an identifier conflict. It basically means that in any particular identifer-item namespace, an identifier must appear only
		once. Identifier-items in distinct namespaces can never have identifier conflict with one another while identifer-items with the same
		namespace have more room for identifier conflict since it now has to make sure that there is no identifer conflict between itself and the
		other identifer-items it shares namespace with. The language does have an exception for idenifier conflict which would be discussed later.

		Examples
		────────
		- mixin namespace
		- interface namespace
		- type creators namespace
		- userscope namespace
		- function and object namespace

        <alias_decl>'s namespace is dependent on the identifer-item it is currently aliasing.

		Techinal Pharse
		───────────────
		If multiple identifier-items are in the same namespace, it is said that they share the same namespace. Keyword here is 'share' because the
		same identifier-item is does not share namespace with itself, it dosen't make sense to say that.

		SCOPE
        ─────
		refers to the area where identifiers are declared and made accessible. All identifier-items have their own scope which would be discussed
		briefly.

		Examples
		────────
		- Identifier-item scope
		- Expression block scope
		- Global or module scope

		Scope and Namespace
		───────────────────
		With the definition of both scope and namespace, it is further defined that identifier-items in the same namespace and scope must have
		unique names for their identifers.
		Meaning something like this:
		```
		userscope sameIdentifier
		{
			userscope sameIdentifier{}
		}
		```
		Is possible because they are not in the same scope even though the are in the same namespace.

		There are two types of scope:
		1. Anonymous scope
		2. Known scope

		Anonymous Scope
		───────────────
		this type of scope does not have an identifier or any special syntax to refer to it.

		Known Scope
		───────────
		this type of scope does has an identifier and special syntax to refer to it.

		Scope Access
		────────────
		From the various definitions of scope types particularly the known scope type, it is insinuated that an identifier-item declared in a scope
		can be accessed from outside the scope, which is true, there are two symbols used to exactly this, [::] and [.], one is just a plain symbol
		which acts like an operator and the other is an operator which acts like a symbol.

		For Every Other Known-Item except Type Creators
		───────────────────────────────────────────────
		[::] which is called the scope accessor symbol is used.

		Syntax
		──────
		<identifier>{'::' <identifier>}

		The left <identifier> been a scope or whole-identifier-item idenifier while the right been any part-identifier-item.

		For Type Creators
		──────────────────
        Type creators have a special operator for accessing member operators [.]
        There is no current alternative for [.] when it comes to accessing member data.
        b.fcn() ==> @<type>::fcn(b)
        b.data

		External Scope Access
		─────────────────────
		Take this example:
		```
		userscope some
		{
			fn fcn{}
		}

		struct kafa{}

		typescope kafa
		{
			fn kafa_something(&_){}
		}
		```
		Function [fcn] in the userscope is accessed like this some::fcn, while member function [kafa_something] in the kafa typescope is accessed
		like this:
		```
		type kafa x()
		x.kafa_something()
		```

		Internal Scope Access[1]
		────────────────────────
		Inner self scope access is not allowed in nc.

		Take this example:
		```
		userscope some
		{
			fn fcn{}

			userscope some2
			{
				fn callfcn
				{
					fcn() ``our focus
				}
			}
		}
		```
		How is the inner scope(userscope some2) able to access the outer scope's(userscope some) function 'fcn'?

		It is possible because of the following language exception:
		- During inner scope access, if there is no identifier conflict between any of the outer scope's items and the inner scope's item,
		  scope access can be done without the scope accessor symbols.

		When the exception applies:
		```
		userscope some1
		{
			fn fcn{give 4000}

			userscope some2
			{
				fn fcn{give '8'}

				fn another()
				{
					give fcn() ``to access the outer scope's fcn, [some1::fcn()] is used
				}
			}
		}

		userscope some
		{
			fn fcn{give 450}

			userscope some
			{
				fn fcn
				{
					give fcn()
					``to access the outer's scope function fcn [some::fcn()]
					``but there is one problem with this, the inner and outer scope's userscope both have the same identifier [some]
					``But since inner self scope access is not allowed, can't the language just assume it is from the outer scope? No the language
					``would not assume that because inner self scope access is not allowed period. But then how is the inner scope(userscope scome)
					``supposed to access the outer scope's(userscope some) function fcn? The outer scope's(userscope some) is accessed by using it's
					``outer scope(i.e the global scope in this case) to clear the ambiguity, which is [::some::fcn()]
				}
			}
		}
		```
		Same with the above example but more complicated
		```
		{
			userscope g
			{
				userscope f
				{
					userscope v
					{
						fn fcn(){}

						userscope v
						{
							fn fcn(){
								g::f::v::fcn ``which??
							}

						}
					}
				}
			}
		}
		```

		Scope Visibility
		───────────────
		refers to the range at which the identifers of an identifer-item can be seen and therefore used within a scope, or it refers to the
		visibility range of identifiers of an identifer-item. There are two types of scope visibility namely
		1. Universal scope visibility
		2. From-declaration scope visibility

		Universal Scope Visibility
		─────────────────────────
		In this type of scope visibility, the identifer of an identifier-item visibility range begins from the start to the end of the scope it is
		declared in, meaning it can be seen and therefore used everywhere within the scope.

		Examples
		────────
		- <mixin_decl>
		- <interface_decl>
		- <type_creators_decl>
		- <userscope_decl>
		- <function_decl>

        <alias_decl>'s scope visibility is inherited from the item-identifer it is aliasing.

		From-declaration Scope Visibility
		────────────────────────────────
		In this type of scope visibility, the identifier of an identifier-item visibility range begins from the declaration of the identifier-item
		to the end of the scope it is defined in.

		Examples
		────────
		- <object_decl>

        Scope visibility types cannot mix in code, example:
        ```
        type si32 [a, b] = [fcn(), a]

        fn fcn(){give b}
        ```
        [Error: a's construction depends on function fcn which violates scope visibility by using b which isn't visible to a during construction]

		Overloading
		───────────
		As mentioned in the definition of namespace, the language has an exception for identifier conflict which is called overloading. Overloading
		is a language feature that allows identifier conflict for both the <function_decl> and <object_decl> identifier-items only when the data type
		of each identifer is different. This is done because the language has mechanisms for resolving each identifier.

		Term
		────
		- overload: refers to the individual identifer of an identifier-item involved in the idenifier conflict

		Overload Resolution
		───────────────────
		Is the solution that empowers the overloading language feature. Overload resolution is the process of resolving an overload to it's
		identifier-item. There are two types of overload resolution namely

		1. Using the language overload resolution function [to]
		2. Using the call operator on function overloads

		Using the language overload resolution function
		───────────────────────────────────────────────
		[to<>()] simply accepts the data type you wish to resolve to and the overload. It is the most unambiguous form of overload resolution.

		```
		fn fcn() type si8{} ``to<fn()si8>(fcn)
		fn fcn() type si90{} ``to<fn()si90>(fcn)

		type si89 fcn() ``to<si8>(fcn)
		```

		Using the call operator on function overloads
		─────────────────────────────────────────────
		This method is limited - in as it is only avialable to function overloads and then only works for certain function overloads - and ambiguity
		prone.
		```
		fn fcn(type si34 _){} ``fcn(234`si34)
		fn fcn(type d32fp _){} ``fcn(23.0)
		```

		But it can't work for function overloads with the same parameter type and number but different give type.
        ```
		fn voice() type sound{} ``fcn() ??
		fn voice() type image{} ``fcn() ??
		```

		It is also ambiguous when it comes to certain function overloads with type parameters, this is because argument inference entry is a feature
        of type argument entry.
		```
		fn fcn<type t, type t2>(type t2 _){} ``fcn<si32>(45`si9) without the previous overload, this function would have been called
		fn fcn<type t>(type t _){} ``fcn<si32>(45`si9)
		```
		The language resolves this by resolving to the best match.

		If it the call operator method is not preferred, you can always use the function [to] method.
		```
		fn fcn<type t, type t2>(type t2 _){} ``to<fn(si9)>(fcn<si32, si9>)(34`si9)
		fn fcn<type t>(type t _){} ``fcn<si32>(45`si9)
		```

        Note: the function [to] overload resolution method is not possible for constructors, only the call operator method, that means constructors
        are limited in function type specification

        Overload Resolution Conflict With Internal Scope Access
        ───────────────────────────────────────────────────────
        For the <function_decl> and <object_decl> identifer-items, the overload resolution methods would to be an alternative for scope access during
        internal scope access when identifier conflict is encountered, but this is not allowed by the language which forces the use of scope access
        regardless of the alternative.

        Take this:
        ```
        userscope some
        {
            fn fcn{}
            userscope son
            {
                fn fcn()type si9{give type si9}

                fn dac
                {
                    give to<fn()!>(fcn) ``Scope access is expected here, although function [to] can do this, scope access is still preferred
                }
            }
        }
        ```

        But since the expression block item has an anonymous scope, thereby making scope access impossible, this alternative is allowed to be used
        for scope access during internal scope access. Because of this, the overload rule is appiled and thus preventing items <function_decl> and
        <object_decl> who both share the same namespace, from identifier conflict across scope.
        ```
        {
            fn fcn{}
            {
                fn fcn{give 56}
                infer _ = to<fn()!>()
            }
        }
        ```
	}
    : Code-reuse and polymorphism
    {
        - static-dispatch/monomorphism and dynamic-dispatch/polymorphism
		- entry of heterogenous types is supported in nc through dynamic objects(mixin, interface and subtyping) and sumtypes

        Mixins
        ──────
        Are identifier-items which define functionality to be mixed-in with types. They are part of the language to introduce a modular system
        for sharing functionality amongnst types. The only items allowed in them are <function_definition_decl>s.
        Mixins are added to types via the 'use' syntax:
        ```
        struct bigData{...}

        mixin copy_data
        {
            fn whole(&me){...}
        }

        typescope bigData
        {
            use mixin copy_data
        }

		type bigData x
		x.whole()
		x.mixin(copy_data)::whole() ``is to be used when identifer conflict is encountered, unless the mixin items should act as if it is from the type

        ```
        The term used for the above is type [] uses mixin []. Multiple mixins can be used in a type and in the case of identifier conflict, the
		mixin's items are protected by qualification, only operators don't allow this.

		```
		mixin x_y { fn cause{} }
		mixin y_z { fn cause{} }

		typescope si32 { use mixin [x_y, y_z] }

		@si32::mixin(x_y)::cause()
		@si32::mixin(y_z)::cause()

		``for operators

		mixin add{ fn #op+(lhs, rhs){ ... } }
		typescope si32 { use mixin add } ``already has operator+ defined for it

		``SEMANTIC ERROR: The rule that allows multiple mixins or interfaces to be used in a despite despite identifier conflict is not applicable
		to operators because they are operators not identifiers
		```

        Interfaces
        ──────────
        Are identifier-items which disclose functionality for types to define. They are part of the language to introduce a modular system for
        defining contracts between types. The only items allowed in them are <function_disclosure_decl>s.
        Interfaces are added to types via the 'use' syntax:
        ```
        struct someType{...}

        interface output
        {
            fn output(&me)!
        }

        typescope someType
        {
            use interface output
            ``Then the type must define the functions
            fn output(&me)! {...}
        }
        ```
		Multiple interfaces can be used in a type and in the case of identifier conflict, the interface's item are protected by qualification, only
		operators don't allow this.

		```
		x.interface(output)::output()
		```
        Interfaces can also contain <function_definition_decl>s used for specifying default definitions that can be overriden when used in types.

        Generic Typing and Monomorphism among Mixin and Interface items
        ───────────────────────────────────────────────────────────────
        The function definitions in both mixins and interfaces are monomorphic therefore they support generic typing same with functions that have
        type parameters. [type me] the language type for in house types is the only type to be specified for declaring member functions, along with
        the special syntax for it [&<idenifier>]
        ```
        mixin some_mixin
        {
            fn fcn(&me)
            {
				me.c = me.b + me.a ``without knowing what type me.c, m.b and m.a is
            }
        }

		struct some{type si32 [a, b, c]}
		typescope some{use mixin some_mixin}
        ```

		Inheritance
		───────────
        Inheritance is the mechanism that allows same identifier-items to share their items with one another.

        Types of Inheritance
		────────────────────
        1. Mixin Inheritance
        2. Interface Inheritance
        3. Type Inheritance

		Mixin and Interface Inheritance
		───────────────────────────────
		These type of inheritance does not allow identifier conflict between the inheriting item's items and the inherited items.

		Type Inheritance
		────────────────
		This type of inheritance allows identifier conflict between the inheriting item's items and the inherited items because it has a way of
		resolving the conflict by using the [tosuper<>()] function.

		The [tosuper] Function
		──────────────────────
		It is used to get the supertype from a subtype, and can be used in overload resolution.
		```
		struct a{...}
		struct b: a{...}

		typescope a{ fn fcn(&me){} }
		typescope b{ fn fcn(&me){} }

		type b x()
		type a _ = tosuper<a>(&x)

		tosuper<a>(&b).fcn()
		```

		Non Inheritance Types
		─────────────────────
		1. Reference types
		2. Dynamic types
		3. Function types

		Use And Inheritance Conflict
		────────────────────────────
		1. The same mixin and interface cannot be used or inherited more than once
		```
		mixin output {fn output(&me){}}
		mixin input {fn input(&me){}}
		mixin io { use mixin [output, input] }

		typescope si32
		{
			use mixin [output, io] ``io already contains the items of output through mixin inheritance
			``or
			use mixin [output, output]
		}
		```
        Dealing With Redundant Inheritance
        ──────────────────────────────────
        A          A				A
        B: A       B: A				B
        C: A       C: B + A + A		C: A + B
        D: B + C					D: C + B

        Due to the fact that more than one identifier-item can be inherited at the same time, the problem of redundant inheritance is brought
        to light. When left unchecked, it leads to terible inheritance chain like the diamond problem and ambiguous situations, for these
        reasons it is redundant inheritance is disallowed in the language and when encountered, an error is given urging the programmer to
        fix the problem.


		[NOTE] Only value references are disallowed from typescope extention because of thier type identity duality


        Dynamic Object
        ──────────────
        {
            type (mixin: mad) x
            type (interface: mad) y
            type (subtypeof: mad) z
        }

        [GENERAL RULE]
        {
            : Interfaces as a dynamic object is only applicable in the context of type use and not getting items from a type(@- fromType)
             - same with function types and subtypeof
        }
    }
	: Constraint block
	{
		fn fcn(me)type si32 -> {} {}

		: Constraint blocks are only allowed for generic functions(functions that support generic typing)
		: Constraint blocks must contian type related quering and nothing else

		: Both type parameters and type query block are semantically not allowed for function expressions
		{
			fn<type t> -> {give false} {}
		}
	}
	: Additional LCI
	{
		#exp::is_parameter_arg_result_of_function(to<fn(si32)!>(normalize))
		#literal
		#ty::integer_number_literal
		#ty::real_number_literal
		#ty::string_literal
		#ty::character_literal
	}
	: Defining custom literals
	{
		#at::static
		unit
		{
			fn #literal(type #ty::integer_number_literal<si4> sec)
			{give type chrono::second(sec)}

			fn #literal(type #ty::real_number_literal<b32r> complex)
			{give type math::complex(complex)}

			fn #literal(type #ty::string_literal c++_lang)
			{give type lang::c++(c++_lang)}

			fn #literal(type #ty::character_literal u8)
			{give type unicode::utf8(u8)}

			The literal data-types are only to be used as parameters for [#literal] function
		}
	}
	: [##] Implementation of autocompletion of my editor
	{
		: Let's say the user types a word and the sugggestions there are of different case, change the particular character that is of different case and suggest that word to the user
		{
			Cov - [but there is only coverage, so suggest it as Coverage] - Coverage
		}
		: [some different case] should not suggest [another different case]
		{
			t - [X] - T
		}
		: How would choosing symbols for end-to-end completion work
		{
			Example: entering '(' then ')' auto completes itself, same for ["]
			
			In normal text mode, only brace symbols(expect '<' '>') should be used
			In source code mode, it is choosen by the language

			It also covers the symbols that performs end-to-end completion on selected text
		}
		: Replace the quotation marks with square braces and watch what happens '(' ')'
		: Indentation of the file and moving between indentation like ctrl+home but selective
		: Selecting words but with a different command than ctrl+d
	}
	: Coverage of (if-else) and (match)
	{
		This coverage is used when (if-else) and (match) are used in expressions, delay initializing and assuring that a give statement would
		be in a function thereby not causing undefined behaviour.

		1. For (match) and (if-else) when operating on known value sets such as sumtypes or enums, a full list of the possible branches is
		   sufficient or a default branch, same for (if-else) when using enums.
		2. For (match) and (if-else) when operating on unknown or undetermined value sets such as integers, a default branch must be present.

		: When used in expressions
		{
			Too lazy to define it, but you know what you want!!

			b = { if true: give 34 } ``no error
			b = { if #at::eval fcn(): give 45 } ``no error
			b = { if fcn(): give 45 } ``error
			b = { if fcn(): give 45 else give 32 } ``no error
		}
		: When used in delay initializing
		{
			Same as the above essentially
		}
		: When assuring that a give statement is present
		{
			Could occur in two case
			: Explicitly specifying the give type
			: Only specifying a give type in one branch of (if-else) and/or (match)
		}
	}
	: What is the difference between the following
	{
		if #at::eval fcn():
		give 45

		#at::eval
		if fcn():
		give 45

		2 == 3 ``evaluated at compile time
		2 == b ``evaluated at run time
		
		#at::eval
		type si32 b
		type si32 c

		b == c ``evaluated at run time
		For an expression to be static time, all parties must be static time
	}
	: If the error isn't going to be used, instead of using the placeholder identifier, the parenthesis can be ommited completely
	{
		x => {continue}
		x => {give}
	}
	: Something like this will cause a conflict
	{
		fn fcn(type [si32, b32r]_ [_empty_, 34`b32r]){}
		fn fcn(type [si32, d32r]_ [_empty_, 34`d32r]){}

		fcn(34, _) ``cannot resolve, the language shouldn't even permit such
	}
	: How would slice be defined, crucial to this problem's solution => type slice<char>(str, ...) or type slice(str, ...)
	{
		typescope slice<type t>
		{
			fn _construct_(type t<t> x)
		}
	}
	: For loop safety on number iteration
	{
		for i: (.end= void)
		{
			``The compiler must know that the loop would break and it is given this assurance by checking for code that relates the iteration 
			``identifier [i] to a break jump_statement, how it does this is by checking for any (if-else) whoose conditional expression queries
			``the iteration identifier[i] and executes the body which contians a break jump_statement or checking for any expression that uses
			``the iteration idenitifier and performs a safety check that gives an error when failed and the expression uses the jump statement
			``in the event of the error been given

			``examples 
			if i == 45
			break: 1

			``or
			match str(i, control)
			{
				br<char> {...}
				br<error> {break: 1}
			}

			``or
			str(i, control) =>{break: 1}
		}
	}
	: Can my for loop work for list iteration
	{
		for infer it = list; x != end; x.next()
		{}

		--make it work goddamn it--
	}
    : What did i later settle on regarding UFCs
    {
        x.::fcn()
        --remember the pointer conomdrum i.e calling from a userscope and how value references seem ambiguous due to it--
    }
	: Look at std::bitset member functions to add to integers
	: [unsafe] semantics
	{
		Language default [unsafe] operations:
		- initializing a typed MAR with an untyped MAR
		- accessing a mutable global object(?)

		: Individual expressions can be marked as [unsafe]
		: If a converting initialization function is marked [make_unsafe] then the whole type declaration is to be marked [unsafe]
		: A function declaration can be marked as [make_unsafe] meaning any use of it is marked as [unsafe]
		: A function declarations not marked as unsafe can still be called unsafely as an expressions if the boolean [is_unsafe] is used in them
	}
	: Init
	{
		[_init_] should not have the same value as the type, because that would meain it is a copy initialization

		fn _init_(&me, &src){}
		fn _init_<type t...>(&me, type d_array<t>... x){  }

		Think more about this in relation to having array of arrays

		: I forgot syntax for variadic only parameter name entry --[...me] ?--
	}
    : Owning MARs introduction/semantic-definition
    {
		Transmutation
		Having owning MARs means you cannot replicate the iteration model in c++
		Can a non-owning MAR bind to an owning MAR

		[ | | | | ]
		(x + 4) gives !
		$x

        type {*}si32 x
		type &{*}mut si32

		: Is the only MAR that can be iterated using the [+] and [-] operators, all of which gives !
		: It cannot be copied, only moved and value-referenced
		: Both MAR types cannot bind or reference each other
		: Assignment of an owning MAR --nothing significantly special about them--
		: Transmutation of the same type erased owning MAR to more than one owning MAR with the same lifetime range is not allowed
		{
			type {*}mut ? x = malloc(4)

			#at::unsafe
			unit {
				type {*}mut si32 _(x)
				type {*}mut si32 _(x)
			}
		}
    }
	: From Linus Trovalds said in an email, gotten from a [...] site
	{
		any compiler or language that likes to hide things like memory allocations behind your back just isn't a good choice for a kernel.
	}
    : A memory model that prioritizes safety and allocator awareness
	: Allocator implementation in nc --return--
	{
		How to handle allocator failures - a global mutable log should be setup and that exits with an error as it's default action and also should be able to call a function upon exit
		type alloc<?>
		type arena<?>
	}
	: Define value-reference referencing a MAR
	{
		type &{*}
		type &mut*mut si32: mut
	}
	: Listen to Johnthan Blows talk
	: Expression Aliases hath cometh again and this time shath stay
	{
		alias one = 1
		alias result = {give 34 + 34} + me.fcn
		alias print = io::out.write
		alias some = me.fcn
		me.fcn()
		Me::fcn(&me)
	}
	: LCI to add
	{
		#at::make_unsafe
		#at::unsafe
		#exp::is_unsafe
		#exp::type_alignment<*>
		#exp::typesize<*>
		#exp::is_parameter_arg_result_of_function(*, *)
		#exp::is_parameter_arg_result_of_function<*>(*)
	}
	: Inheritance is only possible for struct type creators
	: Static time type introspection would be added
	{
		x.isMember(*)
		x.usesMixin(*)
		x.usesInterface(*)
	}
    : Priorities
    {
        - Learning about allocators and how to integrate them in my language
        - Refining MARs(memory address references) semantics in relation with the introduction of an Owning MARs
        - Understanding runtime type introspection and it's uses via type erasure, serialization and standard data-type printing
        - Focusing more on the metadata that can be accessible via LCI like static time introspection and so much more
        - Understanding static time evaluation and all it's niches(specific areas of expertise) and naunces
        - Learning more about IO and how they would be implemented in my language, considering the claims that fast io makes
        - How not to make an array of values default initializes every time due to the languages strict stance on memory safety
		- Implementing zero size types
    }
	: All ideas gotten so far
	{
		fcn(...) --external objects i.e function parameters must be static to be considered for static evaluation

		type si32 x = 34
		#at::eval fcn(x) --no, x is not static--
		#at::eval fcn(34) --yes, literals are static by default--

		static and run time code can interact and when done, the entire code is run time

		(static + run)time = (run)time

		[eval] for expressions, statement and object declaration items
		[make_static] for function definition declaration items

		Oh yh! [at] for attribute

		What of something like these:
		{
			#at::make_static fn some(){}
		}
	}
	: LCI static time debugging
	{
		- only works when main is run at static time
		- similar to code explaination but on steriods

		#at::debug(#exp::mark(Parser))
		#at::setDebugBreakpoint

		\"compile down to what language": (Java|Javascript|C++|C|Rust|Go|Scala|C#|Webassembly)
	}
	: WHY?
	{
		type tuple<si34, si32> c()
		
		#at::static
		for i: (.end= c.size)
		c(i) + 34

		: Would marking only the integer static allow it to be some accessed at runtime use in a while

		: Why is tuple iteration possible while it isn't for normal structures

		struct some{type [si32, si34][a, b]}
	}
	: typescope cannot be access restricted
	{
		: Define it well
	}
	: Function type new syntax
	{
		type *fn(&si32)&si32 x:mut
	}
	: Object of function type vs Object that overloads the call operator
	{
		str(1) --> gives si32 and si64 based on the overloads choosen, but since it is an object that overloads the call operator, it should behave like it only has the call operator in it's arsenal
		to<fn(&me)si34>
		Since it overloads the call operator, it would be the only means of overload resolution it would have. But a function type can extract the
		call-like function in the type

		Given [fcn] which has several other overloads
		type fn(si32)! x(fcn)
		type fn(si32)! x = fcn ``Error many overloads detected
	}
	: Lifetime tracking implementation
	{
		References must reference objects with the same lifetime range

		Written mathematically
		Given two objects m of reference type and n of non-reference type, their lifetimes lₘ and lₙ, if lₘ != lₙ then m cannot reference n
	}
	: [type f_array] defined semantics
	{
		type f_array<si34>
		type d_array<si34>

		struct f_array<type t>
		{
			type {*}mut t data,
			#at::static
			type si32 [size, capacity]
		}

		: I need to store the capacity to preallocate memory and also enabling [push]
		: [push] -back- would not grow the capacity of a fixed array -meaing it is error prone
		: [pop] -back- would not shrink the capacity of a fixed array
		: To prevent against stack waste, the following measures are taken
		{
			- allocation of objects on first use
			- deallocation of objects on last use
		}
		: So a fixed array has an owning-MAR and due to lifetime tracking rule, it must reference objects with the same lifetime range, stopping something like this:
		{
			type f_array<si32> x()
			fn fcn
			{
				x = type f_array(1, 2, 3, 4, 5, 6) ``ERROR: x.data(type {*}si32) is referencing objects with a different lifetime range
			}

			``and this

			fn my
			{
				type f_array<si32> x(.capacity= 32)
				fcn(&mut x)

				fn fcn(type &mut f_array x)
				{
					x.push(45)
					x = type f_array(.capacity= 10) ``ERROR: x.data(type {*}si32) is referencing objects with a different lifetime range
				}
				type f_array<si32> global(.capacity= 40) --global
			}
		}
	}
	: Learn more about types that are expensive to copy
	: LCIs to think about
	{
		#exp::filepath
		#exp::module_location
		#exp::package_location
		#exp::location
		#exp::callsite_location

		location = "module: {filepath} line: {line} column: {startcolumn}"

		{
			#at::inline
			fn assert_eq(type si32 [a, b])
			{
				infer [str_a, str_b] = [a, b] | #exp::\"parameter_arg->str"
				assert(a == b, "[ASSERTION FAILURE] On location: %,  (% != %) : {% != %}", #exp::location, a, b, str_a, str_b)
			}
		}
	}
	: A new identifier item [unique] that makes an existing type distinct
	{
		unique handle = type si32
		unique kilometer = type si32
		unique str<type t> = type darray<t>

		: How would these work fare during overloading, according to my implementation of not wraping the type
	}
	: Arbitary integers are disqualified
	{
		bit  8, 16, 24, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536
		byte 1, 2,  3,  4,  8,  16,  32,  64,  128,  256,  512,  1024, 2048,  4096,  8192

		ui8, ui16, ui24, ui32, ui64, ui128, ui256, ui512, ui1024, ui2048, ui4096, ui8192, ui16384, ui32768, ui65536
	}
	: Grouping syntax addition
	{
		member -> postfix -> prefix

		([a, b, c].[f, g, l])()             -> [(a.f)(), (b.g)(), (c.l)()] --possible--

		[a, b, c].[f, g, l]()               -> [a.(f()), b.(g()), c.(l())] ``error
		($[a, b].[f, g])()                  -> [($a.f)(), ($b.g)()] --possible--
		$[a, b].[f, g]() 					-> [$(a.f)(), $(b.g)()] --possible--
		$([a, b].[f, g])() 					-> [$((a.f)()), $((b.g)())] --possible--

		: Add postfix operator -call operator- when parsing for initialization for grouped object declaration
		{
			type [ui1, ui4] [custom_variadic_pos, past_i][(), i] --check this out--
		}
	}
	: Addition of inclusive and exclusive number-range operator [~]
	{
		inclusive range [~=]
		exclusive range [~]
	}
	: Things like recursion on function that have static parameters, how would it go
	{
		fn definition(#at::static type si32 f)
		{
			if f == 0:
			give 34
			else
			definition(f-1)
		}
		
		definition(45)

		fn definition()
		{
			definition(44)
		}
	}
	: The character to be used for raw string escape sequence entry is now [*]
	{
		"\*{\"emeka"}*" --[no more]-- "\#{\"emeka"}#"
	}
	: Block
	{
		Describes two types
		1. Block expression
		2. Block statement
	}
	: [else] effect on for loop that does not have and [end]
	{
		for _: (.end= void)
		{}
		else
		{}
		: Should warned against because [else] does not have a possibility of executing
	}
	: What and what initializers should be avialable to type_creators by default as defined in the language
	{
		: And also factor in access restriction
	}
	: Design problems of pseudo_writef
	{
		- pseudo_write(type slice<char>(str, old_i ~ (i-1))), how do i expect slice to work out the box with u8str?
		- all these \"->str" functions, what will they really give and take even?
	}
	: There are two memomry spaces in which objects can be allocated
	{
		- stack space
		- global space

		Initialization of [type f_array] in an _init_ function is not feasible due to lifetime inconsintencies -study more
		Or i can make an exception were [type f_array] fields can only use the _init_ function that takes the capacity
	}
	: NC regex
	{
		([a-z1-9_]+):(struct|union|enum|nil)
		$[$identifier()$+] : $[struct|union|enum|nil]

		*	nil:si32\n
		*	struct:book;page_number:si16;book_colour:colour\n
		*	union:anything;si32;si512;si2084;d32r;b64\n
		*	enum:even_numbers;(si8);two:2;four:4;six:6;eight:8;ten:10\n
		*	struct:novel;(book);book_edition:edition;accolades:d_array<accolades>\n

		regex"\*{
			$alias nc_identifier = \"$(unicode, min~max)$*"|$<identifier>$*
			$alias varaint1 = $[$nc_identifier] ${: $[$nc_identifier|$(number)]}$*
			$alias variant2 = $[($nc_identifier ,$nc_identifier$*)]

			$[$[struct|union|enum|nil] : $[$nc_identifier]] ; $[$varaint1*|$variant2]
		}*
		"
		$...

		alias nc_identifier = ('\"' <unicode: mix, max> '"')*|<identifer>*
		[['struct'|'union'|'enum'|'nil'] ':' [nc_identifier]] ';' ... [nc_identifier] (':' [nc_identifier|<number>])*
	}
	: Piping is going to be disallowed and <arg> would be looked at more
	{
		(1, 2, 3, 4, 5)|fcn
		I am going to remove piping for now
	}
	: Since some LCI's are consider to be functions, can they be stored in an object like normal functions can
	{
		infer x = #exp::variadic_size
		Two reasons to support this
		1. In order for programmers to better understand
		2. In order for accepting (str)s to be consider normal
	}
	: editor
	{
		Bringing out the column number for selection functions like shift + mouse-click
	}
	: LCI to add
	{
		#exp::operator_precedence
		#exp::type_creator_metadata<*>
		#exp::type_creator_comprehensive_metadata<*>
		#exp::which_type_creator<*>(*)
		#ty::\"str->item"(*)
		#exp::\"str->exp_item"
		#exp::\"->str"
		#exp::\"<-str"

		#exp::\"ty_item->str"
	}
	: Format for type_creator_metadata
	{
		struct book{type [si16, colour] [page_number, book_colour]}
		union anything{type [si32, si512, si2084, d32r, b64r]}
		enum even_numbers: type si8{two(2), four(4), six(6), eight(8), ten(10)}
		struct novel: type book{type [edition, d_array<accolade>] [book_edition, accolades]}

		/*
		*	Remember struct type creators can inherit other struct type creators
		*
		*	nil:si32\n
		*	struct:book;page_number:si16;book_colour:colour\n
		*	union:anything;si32;si512;si2084;d32r;b64\n
		*	enum:even_numbers;(si8);two:2;four:4;six:6;eight:8;ten:10\n
		*	struct:novel;(book);book_edition:edition;accolades:d_array<accolades>\n
		*/

		/*
		*	Remember struct type creators can inherit other struct type creators
		*
		*	nil:si32\n
		*	struct:book;page_number;book_colour\n
		*	union:anything;si32;si512;si2084;d32r;b64\n
		*	enum:even_numbers;(si8);two;four;six;eight;ten\n
		*	struct:novel;(book);book_edition;accolades\n
		*/
	}
	: Output and serialize implementation focus --visit later--
	{
		[type str] has the printing style of this : write("Emeka") => Emeka
		So when type slice<?> is references it, it can just use the style of [type str]

		interface output
		{
			fn output(&me) type str
		}

		fn output(&me) type str
		{
			//#exp::type_creator_metadata()
			//#exp::type_creator_metadata<#ty::\"str->code"("novel")>

			``Generic default implementation of output
			``There should be two types of metadata one that is comprehensive and the other that is not

			#at::static
			unit
			{
				infer type_creator_metadata = type str(#exp::type_creator_metadata<#ty::exp(me)>).slipt_on(';')
				//type f_array<tuple<str, str>>

				type str str()
			}

			infer begin_pair = type_creator_metadata(1).slipt_on(':')
			str.fmt("% % !", begin_pair(1), begin_pair(2))

			for i: (.end= void)
			{
				if i ∈ ':':
				{
					infer other_pair = type_creator_metadata(i).slipt_on(':')

					if begin_pair(1) == "struct":
					str.fmt("%: %", pair(1), me.( #exp::\"str->item"(pair(1)) ))
					else if begin_pair(1) == "enum":
					str.fmt("%: %", pair(1), pair(2))
					else if begin_pair(1) == "union":
					str.fmt("%", pair(1))
				}
				else
				{
					if begin_pair(1) == "struct":
					{
						infer supertypes = (&type_creator_metadata(i)).trim('(', ')').slipt_on(',')
						str.fmt("supertype!")

						for j: (.end= void)
						{
							alias supertype = type #ty::\"str->item"(supertypes(j))

							str.fmt("%", tosuper<supertype>(me))

							if j == super_types.size:
							{str.fmt("!") break: 1}
							else
							str.fmt(", ")
						}
					}
					else
					{
						str.fmt( "basetype{%}", type_creator_metadata(i).trim('(', ')') )
					}
				}

				if i == type_creator_metadata.size:
				{str.fmt("!") break: 1}
				else
				str.fmt(", ")
			}
			
		}
	}
	: [pseudo_writef] implementation --visit later--
	{
		// io::out.writef("%{c.x} ;;; %{c.c.now(_)}") ``variadic one or more would not allow this

		#at::inline
		fn pseudo_writef<type t...>(#at::static type u8str u8str, type ?t... args)
		{
			#at::static
			{
				if u8str.size == 0:
				static_write("String is empty, pls fill")

				infer [str, variadic_size, variadic_pos_set] = [type str(u8str), #exp::variadic_size(args), type set<ui8>()]
			}

			#at::static
			for type ui32 [old_i: mut, variadic_pos: mut](1); i: (.end= void)
			{
				#at::inline
				fn \"just found %"() --[i] dosen't need updating--
				{
					pseudo_write(type slice<char>(str, old_i ~ (i-1)))
					pseudo_write(#exp::variadic_access(args, variadic_pos))

					variadic_pos += 1
					old_i = i+1 ``forward skip %
					variadic_pos_set.push_back(variadic_pos)
				}

				``Atleast one branch in branch statement i.e in coverage of the loop must break from either the parent loop or the both parent and child loops

				if str(i, control) =>{if old_i < str.size: pseudo_write(type slice<char>(str, old_i ~= str.size)) break: 1} == '%':
				{
					i += 1

					match str(i, control) =>{\"just found %"() break: 1}
					{
						br('<paren>')
						{
							pseudo_write(type slice(str, old_i ~= (i-2)`*backwards skip of % and <paren>*`))

							[i, old_i] = [i+1, i] ``Skipping <paren> by incrementing [i]

							do
							{
								match str(i, control): [_, i]
								{
									br<out_of_bounds_error> {static_write("EOS reached but <paren> was never found") continue: 1}
									br<char>
									{
										if not i.isDigit(10):
										{
											if to<char>(i) != '<paren>':
											{
												static_write("Right <paren> was expected instead of this [", to<char>(i), ']') continue: 1
											}
											else if old_i == i: ``i.e if the no custom variadic number was found
											{
												static_write("No number literal was given! WHY?") continue: 1
											}

											break: 2
										}
									}
								}

								i += 1

								//if not str(i, control)=>{static_write("EOS reached but <paren> was never found") break: 2}.isDigit(10):
								//break: 2
							}

							infer custom_variadic_pos = type ui8(type slice(str, old_i ~ i)) =>(err){static_write("Custom varg is errorneous:", err) continue: 1}

							if custom_variadic_pos > variadic_size:
							{
								static_write("Custom variadic pos (i.e variadic position given) is > variadic size:", custom_variadic_pos, '>', variadic_size)
								continue: 1
							}

							variadic_pos_set.push_back(custom_variadic_pos)

							//to prevent something like this: pseudo_writef("%meka %(2) %(2)", 'E', daniel)

							pseudo_write(#exp::variadic_access(args, variadic_pos))
							old_i = i+1 ``forward skip of right <paren>
						}
						br('<curly>')
						{
							pseudo_write(type slice(str, old_i ~= i-2`*backward skip of %<curly>*`))

							[i, old_i] = [i+1, old_i] ``skipping <curly> by incrementing [i]

							while not (str(i, control) =>{static_print("EOS reached but left <curly> was not found") continue: 1} != '<curly>'):
							i += i
							else
							{
								static_write("No expression was given! WHY? Curly brace content is zero")
								continue: 1
							}

							pseudo_write(#exp::\"str->code"(#at::static type slice(str, old_i~i)) =>(err){static_write(err) continue: 1})
							old_i = i+1 ``forward skip right <curly>
						}
						br('*') pseudo_write(type slice(str, old~i)) //write % without '*' "%*"
						br() \"just found %"()
					}
				}
			}

			if variadic_pos_set != 1 ~= variadic_size:
			static_write(variadic_size - variadic_pos, "variadic argument(s) given to pseudo_writef are not formatted:")

			#at::static
			for i: (variadic_pos, variadic_size)
			{
				static_write(#exp::parameter_arg_asStr(#exp::variadic_access(args, i)))
			}
		}
	}
	: [unit] is needed in block scope for objects and functions
	: For advanced static evaluation
    {
        : Parsing DSLs -- since string literals are statically evaluated, it is already assured
		{
			c++_lang"
				int main()
				{
					2 +
				}
			"
		}
        : Making .writef work
        {
			.write("handle")
			.writef("% + % ÷ %(1)", 1, 2)
			.writef("%{ #exp::tostr(1 + 2 ÷ 1) }")

			{
				type *si32 x
				type &si32 X
				type {*}si32 x
				type {*}mut si32

				type introspect x(#exp::\"type->str"<&si32>)
				\"exp->str"(2 * 3)
				\"str->exp"("2*3")
				\"type->str"<si32>
				\"str->type"("si32")
				\"parameter_arg->str"(b)

				if current_union_alternative<si32>(x):
				[(try $x) + (try $x)]

				: It is now called the constriant block for generic typing and the only allowed operation is type querying
			}

			#dir::inline_eval
			#dir::exp_to_inline

			
			: Question how slice would work with u8str and many other things
			{
				type u8str x(type slice(34))
				struct slice
				{
					type &u8str x
					type variant<&u8str, internal> x
				}
				struct u8str
				{
					type {*}ui8 data
					#at::static
					type ui32 size
				}
			}
        }
        : Performing bounds checks
        {
			fn #op()(&me, type si32 pos)
			{
				if pos > me.size
				panic(s"On line % from column %, (object %(3)(1) of type %){%(3)(1)} is greater than (%){%}"
				.format(#exp::callsite_line(), #callsite_startcolumn(), #exp::tostr_smart(pos), #exp::tostr_smart(me.size)))

				#exp::asStr()
				#exp::is_statically_evaluated
			}

			#dir::inline
			fn #op()(&me, type si32 pos)
			{
				if pos > me.size
				exit_program()

				give me.data(pos - 1)
			}
        }

		: A static body may only contain external static values, runtime values are not allowed
		: Language default static values
		{
			- literals i.e integer-number-literals, real-number-literals, boolean-literals, character-literals and string-literals
			- immutable primitive types like integers, reals, string

			: With the above, what happens with these
			{
				infer _ = addressof 34
				#dir::eval type si32 x()
				infer _ = addressof x

				#dir::eval infer _ = addressof x

				infer _ = #dir::eval addressof x
				infer _ = #dir::eval fcn()
			}
		}
		: Something like this is not allowed
		{
			#dir::eval
			fn fcn(#dir::eval type si32 x) {}

			But type does it albiet the technicalities

			#dir::eval type exact x() --even though exact has functions that are opt_eval, eval and inline_eval
		}
		: Effects of static evaluation on function declarations and expressions
		{
			Function declarations can be made statically evaluated, meaning any and all calls to them would be resolved at static time. Making
			them function exactly like the languages default static time values when called.

			None static expressions can be made static using the LCI. It is redundant to make an already static expression static and would be
			highlighted by the language when encountered.

			Function declarations can also be optionally statically evaluated based on it's arguments been static or not.
		}
		: Effects of static evaluation on statements
		{
			Statements except jump_statements, can be statically evaluated, meaing the statement is statically evaluated but not it's body. Their
			method of static evaluation is different but complies with the statements intent:

			- <if_statement> : it's conditional expression and that of any else-if chain is evaluated resolving to a singular body if the body isn't static
			- <for_statement> : it's computed and the body is duplicated the number of computed times, if the body isn't static
			- <while_statement> : same with <for_statement>
			- <do_statement> : same with <for_statement>
			- <match_statement> : does pratically the same thing with <if_statement>
		}
		: Effects of static evaluation on objects i.e type instances
		{
			Objects can be made static thereby making the entire object's type item statically evaluated. This isn't only limited to objects
			declared in block or global scope, this it true for even the ones declared at function parameters and type fields.

			: What exactly happens to statically evaluated parameters ?
			{
				fn work(#dir::eval type si32 x, type si32 y)
				{}
			}
		}

		{
			#dir::eval
			fn factorial(type si2084 n)
			{
				if n == 0 give 1`si2084

				type #ty::exp(n) fact(1);

				for i: (2, n)
				fact *= i

				give fact;
			}

			fn call()
			{
				#dir::eval factorial(34`si2084) ``INFO: factorial is already statically evaluated
				#dir::eval {infer x = factorial() give comisc(x)}
			}

			fn example_print(type si32 n)
			{
				type io::output x(io::stdout)

				x.write("Starting here")

				for type ui32 j(1); i: (n, 1)
				{ x.writef("%{j} number[%{i}]"); j += 1 }
			}

			fn main
			{
				#dir::eval example_print(89)
			}
		}
    }
    : Language-Communication-Interface(LCI) definition
    {
		- directives(dir namespace)
		{
			- private(mod|pkg|type)
			- readonly(mod|pkg|type)
			- extend_lifetime
			- make_error_handling_type
			- make_optional_handling_type
			- manages_heap_lifetime
			- function_may_end_heap_lifetime
			- function_ends_heap_lifetime
			- callfirst
			- call_last
			- mustcall
			- mark
			- enforce_efficient_passing -- change to efficient for the type or would writing code for it be better?
			- static
			- opt_static

			- explain_code

			- #dir::make_threadlocal
            - #dir::enforce_efficient_passing<*>
			- #dir::make_optional_handling
			#at::private(mod)
		}
		- statement-like-directives(statement namespace)
		{
			- \"enable auto $"
			- \"disable auto $"
			- mark
		}
		- expression(exp namespace)
		{
			- line
			- startcolumn
			- endcolumn
			- columnpair
			- callsite_line
			- callsite_startcolumn
			- callsite_endcolumn
			- callsite_columnpair
			- typename
			- typesize
			- typeEq
			- isRefType
			- isValueRefType
			- isMemoryAddressRefType
			- variadic_access
			- variadic_size
			- mark
		}
		- type(ty namespace)
		{
			- exp
			- decay
			- fn_givetype
			- variadic_access
			- integer_number_literal
			- real_number_literal
			- string_literal
			- character_literal
		}
		- operator
		{
			- op <every-operator-in-nc>
		}
		- literal

        : LCI
        {
			#dir::mark(daniel) 2 * 2
			#statement::mark(daniel) 2*3

			#dir::explain(#exp::mark(daniel))

            {
                #exp::typeItem<si32>(member, fn main()!)
                #exp::typeItem<si32>(@item::function)
                #exp::namespaceItem()
                #exp::mixinItem()

				#exp::hasItem<si32>("fn some(me: &)!")
				#exp::hasItem<si32>(me)
				userscope_contents(emeka)

				#dir::line(#exp::mark(emeka))

				item(@itemType::struct, "u8str", "member-functions")
				itemlookup(@)
				hasItem(@itemType::struct, "u8str", "fn #op+(type [me, me])")
				\"inItemscope?"()
				\"usesMixin?"

				#exp::asStr(2*3)
				#exp::asStr(me.size)
				#exp::tostr(3 + 3 + 5)

				#dir::mark(emeka) 2 * 3
				#exp::tostr(#exp::mark(emeka))
            }
            : How to get callsite line number information?
            {
                fn somecall(){ log(#exp::callsite_line) }

                A global array of all the line number location sites of the function, then a simple index to acess the info
            }
        }
    }
    : Lifetime tracking uniqueness
	{
		type ui8:& x

		fn some{ x := &45 some2() }``x referenced value dies here

		fn some2{ _= x + x }``leaving this function with a valid value of x only when called in [some()]

		fn some3{ some2() }

		[main]
		{
			use mod [cat, dog]

			fn main{  }
		}

		[dog]
		{
			use mod cat

			type cat cat()

			fn _construct_(&_){ cat.bark() }
		}

		[cat]
		{
			use mod dog

			type dog dog()

			fn _construct_(&_){ dog.bark() }
		}
	}
    : Variadics syntax and semantics
    {
        : Variadic semantics
        {
            Variadics also called variadic argument entry is a special feature in nc that allows the entry of an arbitrary number(>1) of arguments.
            It is a language abstraction that makes use of the monomorphisation that type parameters apply to functions, meaning that for each
            arbitrary number of arguments a new function is created for it, also meaning that variadics joins type parameters as an abstract
            language feature

            Usage:
            : Variadics acting on a normal type parameter
            {
                fn fcn<type t>(type t... _){}

                fcn<char>('2', '3', '4')
                fcn<si34>(1, 2, 3, 4, 5)

                : Of course if one wishes to have the variadics avialable for only a particaular type or set of types, a constraint can be appiled to the type parameter

                #{give #exp::is_same_ty<si8, t>}
                fn fcn<type t>(type t... _){}

                fcn<char>('2', '3', '4') ``Error: type parameter [t] only accepts type si8
                fcn<si34>(1, 2, 3, 4, 5)

                struct some<type t>{ type t... x }
                struct some<type t:?>{ type t:?... x }
            }
            : Variadics acting on a variadic type parameter
            {
                fn fcn<type t...>(type t... _){}

                fcn<char, si8, b32fp, d32fp>('3', 45`si8, 45.7`b32fp, 3.)
            }

            [NOTE] due to type argument entry supporting default implicit argument entry, the following can be rewritten as this:
            {
                fcn('2', '3', '4') ``Error: type parameter [t] only accepts type si8
                fcn(1, 2, 3, 4, 5)
                fcn('3', 45`si8, 45.7`b32fp, 3.)
            }

            Due to nc-lang supporting function overloading and default implicit argument entry for type parameters, variadics can tend to get a bit
            confusing and function identifier overload resolution
            takes place, example:
            {
                [1]
                {
                    fn fcn(type [si8, si8, si8] _){}
                    ``and
                    fn fcn<type t>(type t... _){}

                    fcn(2`si8, 2`si8, 2`si8)

                    fcn<si8>(2`si8, 2`si8, 2`si8)

                    fcn(2`si8, 2`si8, 2`si8, 2`si8)

					fcn(_v_, 2,3,4,5,6, _v_)
                }

				Overload Resolution even in the case of variadics always goes for the better match
            }
        }
        : Variadics do not have a mechanism to support default argument entry
        : In a Varaidic argument entry, one or more must be entered unlike C++ where they are zero or more
        : To avoid identifier-conflict between non-variadic argument entry where the non-varaidic overload of the function would be preferred by
          the compiler, identifier [_v_] is used to specifiy both the start and end(if needed) entry of a variadic argument
		: For now a variadic identifier would not be able to spread in fear of what c++ does with thier version of spreading that invovles recursion
        : LCI variadic
        {
            #exp::variadic_size(*)
            #exp::variadic_access(*, * `*has to be static*`)
			#ty::variadic_access(*, *)
        }
    }
	: Floating point number types gets renamed to real numbers
	{
		b32fp -> b32r
		d32fp -> d32r
	}
    : [struct], [union], [tuple] and [variant] would be introduced to replace the formers
    : <arg> does not give any value, it is just parsed as an expression
    : The member operator is the only operator whoose rhs must be an identifer
    : For object expression, when used in the construction of an object it's declaration, no matter if it is consists of many construction levels, it's constructor would only be called once for that object
    {
        [ type si32 x = type si32(34) ] same as [ type si32 x(34) ]
        [ type si32 x = type si32(type si32(type si32())) ] same as [ type si32 x() ]
    }
    : Function type semantics
    {
		: [...............]
		{
			Functions are treated like objects in nc, they give data type that consists of the function's signature, they can be stored in an instance
			of that type(i.e object of the type)

			: What of thier relationship with the call operator
		}

        : Only argument taken for a direct construction is an object that overloads the #op()
        {
            type str x
            x(1)
            type (fn(str:&, si32)char:&) x(to<str>(x))
            type (fn(str:&, si32)char:&) x = to<str>(x) ``ERROR value of type str is not of type (fn(str:&, si32)char:&)
            type [(fn(str, si32)!), si32] x[..., 34`si32 `*ERROR expected an argument for construction*`]
        }
		: An object overloads the call operator while a function and function type does not
    }
    : How would i solve the problem of function overloading semantics example
    {
        - for objects, if an object can reference an overloaded function through its identifier, should objects with function type have the ability to
        overload themselves using <function_exp> as their source?
        - For overloaded functions and function-objects with multiple call operator overloads, when guessing the function-type, how would that go

        : Big question to answer, what are functions, function-objects, call operator, objects that implement the call operator

        : How will overloaded functions interface
        - {
            fn fcn(){}
            fn fcn(type si32 _){}
            infer a := fcn

            infer x = fn<type t>()
        }

        type (fn()!) x = fn(){} ``same with function overloading
        type (fn(si32)!) x = fn(si32){}

        ``Add to the stuff about functions

        fn <type t>(type t:& _){}
    }
    : Lifetime tracking for both stack and heap allocated memory
    : Object construction and grouping syntax semantics
    {
        : Object expression is disqualified from participating in grouping, because of two primary reasons
        {
            - conflict with object declaration:
                { type [si32, si8][(2), ()] } is distinguishable but this { type [si32, si8][a, c] } is not
            - i want object expression to represent one singular expression like f(), 1
        }

        An object in nc is an allocated memory region that contains valid and user-defined values of the object's type

        Construction/Initialization
        ────────────────────────────
        Is the process of giving an object valid and user-defined values of the object's type

        Holds the post-condition that after construction the object contains valid and user-defined values of it's type
        Holds the post-condition that states an object must be constructed before it's use

        Types of construction
        ──────────────────────
        1. Direct(function call like): takes the arguments for construction/initialization of the object
        2. Indirect(assignment like): takes an already constructed object of the same type

        [Delayed construction/initialization]
        Construction Delay
        ───────────────────
        Construction of an object can be delayed by not specifying an form of construction and later using the delay-construction-assignment
        operator(:=) to construct the object(s).

        Conditions of Construction Delay
        ────────────────────────────────
        1. It may be delayed constructed in if-else or match statements that must have a default branch
        2. it may not be delayed constructed in loop statements

        [(x, m), (y, p), (l, h)]|[fn1, fn2, fn3]
        [x, [y, [z, [c, g]]]] + [a, [b, c]]

        type [si32, si8] a = [type si32(), type si8()]
        type [si32, si8] a()
        type [si32, si8] a[(), (2, 3)]
        type [si32, si8] a()
        type [si32, si8] a = a
        a + [a, b, n]
        type [si32][a, b, c]
        [(2, 3), (4, 5)]|[f, c]
        [(3, 4), (0, 9)]|[[f, c], g]

        Groupings
        ─────────
        Is a language feature that treats multiple items as a single unit. It's main inclusion to the language is to eliminate unnecessary
        typing that sometimes leads to code verbosity and to add a more high-level touch to the language.

        Explanation
        ───────────
        The entities used for enclosure are the left and right square braces '[' ']'.
        Groupings can be acted upon, meaning it can be scaled by another grouping or single item

        example:
        type si32 [a, b, c] = { [1, 2, 3] } - good
        fn mac() type [si32, si32] { [2, 3] } - bad

        The contents of an enclosed group must be entites separated by a comma. So singletons like this [x] do not qualfiy as an enclosed-
        group

        Levels of grouping:
        ───────────────────
        Levels of groupings essentially means the nesting levels of a grouping, just like how blocks can be nested
        example:
        [a, [l, o]]
        └───┼──┬─┼┘
            │  1 │
            └──┬─┘
                2

        [x, [c, [b, [k, l]]]]
        └───┼─┬─┼───┼────┼┼┼┘
            │ 1 │   │    │││
            └───┼─┬─┼────┼┼┘
                │ 2 │    ││
                └───┼─┬──┼┘
                    │ 3  │
                    └──┬─┘
                        4
        The language syntax permits only an expression to have mutiple levels of a grouping

        Grouping follows two basic rules for it's use:
        ──────────────────────────────────────────────
        1. When acted upon, both sides must have full coverage of each other or to be more succint, all items of one side must have a
        corresponding item to act on the other side

        example:

        type si32 [a, b, c] ✔
        type [si32, si45, si80] [d, e, f] ✔
        type [si32, si32] [a, k, g] ⤬

        [b, a, c] = v ✔
        [b, a, c] - [v, g, n] ✔
        {
            🟢 b - v
            🟢 a - g
            🟢 c - n
        }
        [b, a, c] + [f, j] ⤬
        {
            🟢 b + f
            🟢 a + j
            🔴 c + ?
        }
        [x, [y, [z, [c, g]]]] - [a, [b, c]] ✔
        {
            🟢 x - a
            🟢 [y, [z, [c, g]]] - [b, c]
            {
                🟢 y - b
                🟢 [z, [c, g]] - c
                {
                    [z - c, [c - c, g - c]]
                }
            }
        }
        [x, [y, [z, [c, g]]]] - [a, [b, c], o] ⤬
        {
            🟢 x - a
            🟢 [y, [z, [c, g]]] - [b, c]
            {
                🟢 y - b
                🟢 [z, [c, g]] - c
                {
                    [z - c, [c - c, g - c]]
                }
            }
            🔴 o - ?
        }
        [x, [y, [z, [c, g]]]] - [a, [b, c, m]] ⤬
        {
            🟢 x - a
            🟢 [y, [z, [c, g]]] - [b, c, m]
            {
                🟢 y - b
                🟢 [z, [c, g]] - c
                {
                    [z - c, [c - c, g - c]]
                }
                🔴 m - ? ``dosen't offer full coverage
            }
        }

        2. Grouping for items is valid only in contexts where it is also valid in normal code

        example:
        - alias [myInt, bestReal] = type [si32, d32fp]
        {
            ``The above uses both type of groupings i.e unclosed-grouping for the alias identifiers and grouping for the type
            ``It is valid as the expansion can already be implemented without parenthesis grouping

            alias myInt = type si32
            alias bestReal = type d32fp
        }
        - named_st arithmetic{ type [si32, si64, si128, b32fp, d32fp] }
        {
            ``The above is also valid because the expansion can already be implemented without parenthesis grouping
            named_st arithmetic{ type si32, type si64, type si128, type b32fp, type d32fp }
        }
        - fn fcn() type [si32, si8]
        {
            ``The above is not valid because the expansion cannot be implemented without grouping
            fn fcn() type si32 type si8 ``SYNTAX ERROR: reserved identifier 'type' found at unexpected location
        }
        - type si32 [a, b, c]()
        {
            type si32 a()
            type si32 b()
            type si32 c()
        }
        - type si32 [x, y] = [2, 3]
        {
            type si32 x = 2
            type si32 x = 3
        }

        operators:
        - It performs ordered expression wise evaluation and gives a grouping of the results of the operation
        {
            ``[->] denotes the equivalent operation
            d.*fcn()

            [[a, b, c].[f, g, l]]() -> something like this should not be allowed, because enclosed grouping requires multiple entity stuff

            ([a, b, c].[f, g, l])()             -> [(a.f)(), (b.g)(), (c.l)()]
			() | ([a, b, c].[f, g, l])			-> [()|a.f, ()|b.g, ()|c.l]

            [a, b, c].[f(), g(), l()]           -> [a.(f()), b.(g()), c.(l())] ``error
            $[a, b].[f, g]()                    -> [$((a.f)()), $((b.g)())]

            $[a, b, c] += [n, m, c]             -> [$a += n, $b += m, $c += c]
            -[a, b]                             -> [-a, -b]
            b = [1, 2]                          -> [b = 1, b = 2]
            [a, b] = [c, d]                     -> [a = c, b = d]
            [a, b] - 1.9                        -> [a - 1.9, b - 1.9]
            [a, b, c] + [d, e, f]               -> [a + d, b + e, c + f]
            [a, c] * [b, x] + [e, g]            -> [a * b + e, c * x + g]
            [[c, v], p] =/ [k, l]               -> [[c /= k, v =/ k], p =/ l] ``lhs has a two level grouping
            [x, [y, [z, v]]] - [a, [b, c]]      -> [x - a, [y - b, [z - c, v - c]]] ``lhs has a three level grouping
            [x, [y, [z, [c, g]]]] + [a, [b, c]] -> [x + a, [y + b, [z + c, [c + c, g + c]]]] ``lhs has a four level grouping
            [x, y, v]|fcn                       -> [x|fcn, y|fcn, v|fcn]
            [(a, b, c), p]|[fcn1, fcn2]         -> [(a, b, c)|fcn1, p|fcn2]

            {
                [a, b, c].[precision_mul, precision_add]()
                [a, b, c].precision_mul()
                [a, b, c].[precison_mul, _empty_,precison_mul]()
            }

            (x, y, b) is called an <arg>

            ``example for swapping objects using an intermediate object as a store
            ``The two objects to swap are [a, b]
            ``The temporary object that stores an intermediate object value is [c]
            [c, a, b] = [a, b, c]               -> [c = a, a = b, b = c]
        }
        others:
        - It only expands when used as args in the call and pipe operator and it expands by the total number of entities in the resultant
        grouping
        {
            fcn(x, [y, z])              -> fcn(x, y, z)
            fcn([a, [b, [c, [e, f]]]])  -> fcn(a, b, c, e, f) ``the compiler should warn about this being irrelevant
            fcn([m, n] + [s, c])        -> fcn(m + s, n + c)
            (a, [c, b])|fcn             -> (a, c, b)|fcn
        }

        More Info
		─────────
        When used in expressions, to leave an expression unevaluated can be done just by using the [_empty_] identifer
        [a, _empty_, b] + [x, y, c] -> [a + x, y `*unevaluated*`, b + c]
        This is not particularly useful, only in cases where an argument entry is needed for evaluation before passing:
        {
            fn pass_something(type [si32, b32fp, si32] _){}

            fn main()
            {
                infer [a, b, c] = [type si32(34), type b32fp(), type si32(56)]
                pass_something([a, b, c] * [a, _empty_,c])
            }
        }
        It can also be used to delay construction of an object
        {
            type si32[a, b, c][_empty_, 3, 90]

            if b == 3:
            a := b * 2
            else
            a := b * 3
        }

        INTIALIZATION FOR OBJECT DECLARATIONS USING ENCLOSED GROUPINGS:
		───────────────────────────────────────────────────────────────
        type si32[a, b, c][(), (4.5), (4.5)]
        type si32[a, b, c] = [type si32(), type si32(), type si32()]
        type si32[a, b, c] = {
            ...
            [1, 2, 4]
        }
        type si32[a, b, c] = a
        type si32[a, b, c](2.4)
        type si32[a, b, c] = [2, 4, 5] * [4, 56, 9]

        - dosen't hold any significance whatsoever, just wrote it for no reason
        fn \"give"(type [si34, si89, si7] _ [_empty_, _empty_, 5.6₃`b32fp])
        {}
    }
    : Think more about what and what can have from declaration-point visibilty in module or global scope scope
    {
        all declarations except object
        [typespace] must be under it's type
    }
    : Warn for dumb things like unnecessary enclosure like this [[[2, 3]], [2, 3], [2]] or (((3)))
        [2, [[2, 3], [4, 5]]] | [f, c]
    : Member operator needs to have a higher precedence than the call operator
    : Delayed construction operator ':='
    : Identifier, how would [\"_construct_"] be equal to [_construct_] and [\"_"] be equal to [_]
    {
        : It is used to disambiguate between user and language defined use of reserved identifers

        fn _construct_() {}
        fn _mConstruct_() {}
        fn _cConstruct_() {}
        fn _destruct_() {}
    }
    : [memoryof]
    {
        [memoryof] for reading the bits in memory of an object
        no more 'bitcast' because the size of the type is always known statically, so it isn't something the programmer needs bothers his/her self with
        It is even more safe because bitcasting constructs the type-to from the bits of the type-from which is the most unsafe thing that can be done in programming
        And the main reason programmers want to bitcast anyway is to see the bits of the type as unsigned integers not as how they can construct types from bits
        I will still need a way to construct a type from bits, for optimized swap or assignment, or should the language just handle that itself?

        x = ? and y = ?
        (memoryof a) xor_eq (memoryof b)
        (memoryof b) xor_eq (memoryof a)
        (memoryof a) xor_eq (memoryof b)
    }
    : Arbitary integer bit sizes from i2 to i65536 2^16
    : Pre and post condition entry for compiler communication level
    {
        fn normalization(type str:& x)!{ #dir::\"must be a give-value of"(is_valid_base) }

        #dir::\"call this function first"(open)
		#dir::arg_must_be_a_give_value_of()
    }
    : Solidify the type entry that require type prefix --[type <identifier> ':' <type-reference-extension>]
    {
        - Everthing except type argument entry and function signature
    }
    : Single io interface [to be solved later]
    {
        : How would i implement the type of overloading interface that c++ has with [operator<<(std::ostream&, ...)] and [operator>>(std::istream&, ...)]
        : I have solved the type of io format i would use in nc
        {
            io::out.writef("%0 is %1years old %(34)20", "emeka daniel", 23)
        }
    }
    : Global object initialization on first use
    : Memory model that prioritizes safety
    : Zero sized types cannot be referenced
    : Concurrent safe programming
    : Greater and less than or equal to can be evaluated differently i.e !(x < y) or !(x > y)
    : Make a table class
    : Parser error reporting needs to change ???
    : Strong and weak typing and static and dynamic typing
	: Rust's font-family used in the web, [open-sans, sans-serif]
	: In nc decimal floating point implementation, what happens when a really samll number is reduced significantly, does it yeild zero or not?
    : Serialization and deserialization of nc objects look into reflection
    {
        struct foo{}
        typescope foo{ fn foo(_){ io::out.write("Hello from foo") } }

        type (object) x("foo")
        x.getFnItem("foo")()
    }
    : For the bootstrap versions
    {
        - static evaluation
    }
    : For self-hosting versions
    {
        - unicode
        - unicode name and abbreivation escape sequences
        - ncregex
        - table?
        - package system
        - compiler(JIT and AOT) configurations which needs serialization
    }
	: Blog to write [C++'s Illusion of Immutability]
	{
		int const a{}

		int* _{&a} ``cannot not bind
		int* const y{&a}
		int* _{&*y}
	}
}`

`{
    [Bringing back blackjack]

    ("🐀" = "\[1f400]") 👀

    Why am i storing the token and not referencing it?
    Even line content is just being copied for no reason, a copy on write string would solve this

    The compiler would have a json configuration file
    The build tool would require a json style configuration file

    - I think the character size should be accepted at the semantic level

    The lexer shouldn't be concerned or bother about user-literal-types, it should be concerned about only the tokenization.
    The syntaxer should worry only about parsing the tokens into valid syntax, then transforming that into an ast
    The semantic should worry about the rest

    Literals can become types - it involves the type of meta-programming that allows free form manipulation of certain types of code data

    Number Literals:
    - can be multiline, and that multiline would be would be discarded in the lexer[U+2938 ⤸]
    - the digit seperator would follow specific rules, such as it always being amgonst digits and that only one can appear in a sequence
    - they would be type unchecked in the lexer

    CharacterWise Literals:
    - a nc character must be only one unicode character
    - a nc character is not multiline
    - a nc string is a combination of unicode characters of arbitrary sizes
    - strings would support expression interpolation, i.e "emeka daniel is: "\&{ "2"+"3" }&", "{2+3}"

    [Slovable Notes]
    * Ask chat-gpt why underflow for IEEE binary floating-point literal constants are tolerated and rounded to zero in programming languages while
     overflow isn't?
    * Create a visual representation of IEEE binary floating-point using the SFML library
    * A function to accurately calculate the min and max values of IEEE
    - inf/-0 should be implemented right
    * Fun fact: powershell uses '∞' instead of 'inf' or 'INF'

    * Reason about why raw identifers should escape reserved identifers?? what is the purpose = interoperability with other languages
     - escaped identifers and escaped raw identifers

    * Change reserved and identifier to rIdentifier and nIdentifier
     - if not, fix it

    * Remember to add a condition in character function for getting the size when unicode name(\{}), alias(\()) and abbreivation(\<>) are introduced

    * Error recovery in the syntxer to be thought of
     - find a fix for something like: (+) or r_l(+)
     - try to outline what the production-rule expands to when logging like in <r_l>, when <r_l_arg_list> is not found or <relational_op>
    * Remove 'defer' and 'default'
    * Remove this @::si32 from [fromType]
    * Change 'guess' to 'infer'
    * [3~4] for inclusive range
    * Remove 'odd' in favour of member function: 2.isOdd()
    * Should a <block> accept <typespace> and <namespace>? Yes it should

    * Identifiers with starting and ending with [_] should not be allowed
    * '_' would be both the discard and default identifer
    * Conditional expression should have a compulsory else expression
    * The enforcement of using [&mut <expression>] should have a clear semantic definition, that allows override when it becomes inconvinient
     - {
            ``instead of:
            io::out(&mut 2, &mut 3, &mut 4, &mut 5, &mut 6)
            ``do this instead:
            io::out(2, 3, 4, 5, 6)
       }

    * Nc-lang should be immutable first because a programmer should have a clear reason while coding of what object needs to be mutable, it essentially forces the programmer to reason more about his/her code
     - Due to that, standardize how the new [mut] keyword is going to be used
    * Enforce safe while-looping safe using compiler attributes like:
    - in while to help when [continue] is used because it skips the modification expression thereby causing an infinite loop
    {
        while a < 7:
        {
            if a == 5: continue
            //change the name though
            #dir::loop_modifier ++i;
        }
        : Even for loop will need it when the loop itself is not doing the iterating
    }
    * Add the file name of the current file it is compiling, like: current file is main.nc

    * My language needs to be exception safe - read that rust's article about it

    * Check if rust does that annoyning stuff of this std::uint32_t(std::int8_t{-3})

    * Add support for nc decimal floating point 8 and 16 - in a later version of the language
    * Think more about slices
     - A general concept is needed to classify range data structures and that concept would be iterators
     - And the common interface for using data-structures

    * Reflect more on the part of the static analyzer that enforces memory safety
     - How safe are pointers going to be

    * What of this [simd] type?

    : A compiler tool that explains the nc code, would be avialable in the LCI directly too
    _______________________________________________________________________________________________________________________________________________

    something like this is very dangerous:
    {
        type darray a(1, 2, 3, 4, 5) type ui32 count()

        a.assign(a.begin() + count, a.end())

        ``because function @astring::assign(type darray<si32>:&mut, type @darray<si32>::iterator, type @darray<si32>::iterator) dealloctes itself
    }
    _______________________________________________________________________________________________________________________________________________

    [Unsolvable Notes]
    * Positional base agnostic precision arithmetic for integers
    * A negative index should be interpreted from the back for subscripting
    * For total-ordering, a special function that returns boolean values for equality and lessthan
    * The convention of defining relational operators is to encourage defining only equality and lessthan
    * My string implementation would be [copy-on-write], meaning it initially has a view to the string until a write operation is triggered
    * imbedding of markup languages like html and styling languages like css in my language to be used in the web
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
            - find_first_sequential
            - count_sequential_until_c_is_greater_than_n: \"count sequential until c > n"
        }
    * my own regex implementation that embodies clarity and brevity - inspired by EBNF
     - it's syntax won't be conventional
     - it will support extraction without checking
     - it give clear error messages if the syntax is wrong
     - it will support error recording for the parts of strings that don't match ??
     {
        Find and replace
        "emeka@cyril.com"
        "<unicode:all_lang_alphabets> '@' ['gmail'|'hotmail'|'outlook'|'yahoo'] ['.'] 'com'|'n_com'"

        match_str[1] = "nc_mail"
        match_str[2] = "nc"
     }
    * my own list implementation that provides random access without that need of an internal array
	* document editor like microsoft word
    * text/code editor features --study what makes vim and emacs so good then copy it, even it vscode--
    {
        - arbitrary selection of highlighted text starting from a start to end line: turns out vscode already has, depending on where the cursor is, hold shift down and click on an location you want
        - movement of highlighted or focused text to arbitary locations: can be able to apply whitespace inducing keys on it
        - bookmarking for highlighted or focused text and lines
        - keybindings for spwaning and despwaning the sugggestion window: truns out vscode uses the escape key for it
        - highlight lines with only whitespace and lines ith trailing whitespace
        - backspace and delete would be set to remove a grapheme-cluster by default while shift+backspace gives the option of deleting par code-point
		- move forward and backward between "words", similar to ctrl+d but performs a move action instead
		- select a word, similar to ctrl+d
		- combining nc regular expressions with selection --> i think vim does it
	}
	* Your opinon Your Identity - article to write
    _______________________________________________________________________________________________________________________________________________

    *11110000 10011111 10001100 10111001
    *            11111   001100   111001 -> correct
    *            11111     1100 00111001 -> wrong
    *
    *
    *[11110]000 [10]011111 [10]001100 [10]111001

    true or false and true or false
    (true or (false and true)) or false :[ C/C++ parse]
    true or (false and (true or false)) :[ nc parse ]

    «Emeka, Daniel»

    d(i, f(e, 9))
    i.::d(e.::f(9))
_______________________________________________________________________________________________________________________________________________
    Intermediate arithmetic base(10) digits requirements

    - [Multiplication]:
        lhs and rhs should be converted to the result's integer type when computing
        prove: (9{max_real_part}²).digit_count()

        b32fp(7+7 == 14 digits) -> si49
        b64fp(16+16 == 32 digits) -> si109
        b128fp(35+35 == 70 digits) -> si235
        b256fp(73+73 == 146 digits) -> si488
        b512fp(149+149 == 298 digits) -> si993

    - [Division]:
        - [lhs < rhs]: leading zeros are accounted for, we can prove it by using two methods
        Only the lhs need the integer of such size

        prove(1): (9{max_real_part-1} ÷ 9{max_real_part}).digit_count() + {implementation_precision+1}.digit_count()
        lhs = 9{max_real_part-1}
        lhs.digit_count() + {implementation_precision+1}.digit_count()

        prove(2):
        lhs = 1 ÷ 9{max_real_part}
        1 + {number_of_leading} + {implementation_precision+1}

        b32fp(7+8 = 15 digits) -> si50
        b64fp(16+17 = 33 digits) -> si110
        b128fp(35+36 = 71 digits) -> si236
        b256fp(73+74 = 147 digits) -> si489
        b512fp(149+150 = 299 digits) -> si994

    - [Addition]:
        {implementation_precision+1} + 1

        b32fp(7+1+1 = 9 digits) -> si30
        b64fp(16+1+1 = 18 digits) -> si60
        b128fp(35+1+1 = 37 digits) -> si123
        b256fp(73+1+1 = 75 digits) -> si250
        b512fp(149+1+1 = 151 digits) -> si502

    - [Subtraction]:
        b32fp(7+1 = 8 digits) -> si27
        b64fp(16+1 = 17 digits) -> si57
        b128fp(35+1 = 36 digits) -> si120
        b256fp(73+1 = 74 digits) -> si246
        b512fp(149+1 = 150 digits) -> si499

    What I don't like about Rust:
    - the assignment operators don't return the subject of the assignment
    - it dosen't have the concept of value categories
    - it did away with useful operators like [++]
    - why do away with c's for loop syntax?
    - no overloading of any sort: function and operator that is
    - no idea of default parameters
}`