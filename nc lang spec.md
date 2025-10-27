# Nc Programming Language Official Technical Specs

This document defines the formal syntax and semantics of the nc programming language. Unlike most formal documentation, this document is written in a completely beginner friendly way.

**Nc** does not define an acronym nor is it the name of the language, the formal name of the language is **nc programming language** while the informal name of the language is **nc-lang**.

To describe the syntax of the nc programming language, a modified version of the BNF(Backus Naur Form) created just for the nc programming language is used, it is called nc-BNF(Nc Backus Naur Form). Documentation on nc-BNF can be found in [***TODO: create documentation for ncBNF***].

##### TODO: write about comments syntax

## Tokens And Tokens Syntax

Nc-lang source code is first parsed and collected as a list of tokens. Tokens are the fundamental units of nc-lang source code.

### Types of Tokens

- Identifier tokens
- Literal tokens
- Symbol tokens

### Identifier tokens

Are tokens that make use of a set of unicode characters that can be combined to form text that depends on the language and/or context it was written in, it is used extensively to refer to meaningful constructs either defined by you(the user/programmer) or the language. Below is the ncBNF of a generically defined nc-lang identifier:

**\<generic-identifier-token>** = **'\_' | \<$alphabetic-unicode-scripts> ('\_' | \<$alphabetic-unicode-scripts> | '0' ~ '9')**

#### Classifications Of Identifier Tokens

- Language identifier tokens
- User identifier tokens

#### Language Identifier Token

Defines a set combination of generic identifier tokens that are reserved for use by the language. Below are language identifier tokens:

- **userscope**
- **typescope**
- **mixin**
- **interface**
- **struct**
- **union**
- **valueDef**
- **unique**
- **obj**
- **type**
- **fn**
- **give**
- **break**
- **continue**
- **true**
- **false**
- **addressof**
- **and**
- **or**
- **not**
- **xor**
- **try**
- **prop**
- **copy**
- **mut**
- **if**
- **else**
- **while**
- **for**
- **match**
- **import**

#### User Identifier Token

Are identifier tokens that make use of any generic identifier tokens that do not equal language identifier tokens. To eliminate the restrictions imposed on the user of allowing only a small set of scripts to be used in writing identifiers and also the restriction of reserving a set combinations of those scripts for the language, nc-lang has something called escaped user identifier tokens that are valid user identifiers.

#### Escaped User Identifier Tokens

Are user identifier tokens that defines special identifiers that make it possible for the user to make use language identifier tokens and also use any unicode character to form text.

#### Types Of Escaped Identifier Tokens

- Escaped language identifier tokens
- Escaped raw identifier tokens

#### Escaped Language Identifier Tokens

Are the type of escaped identifier tokens that make using language identifier tokens possible for the user. Below is the ncBNF:

**\<escaped-language-identifier>** = **'\\' \<language-identifier-token>**

#### Escaped Language Identifier Tokens

Are the type of escaped identifier tokens that make using any unicode character possible for the user. Below is the ncBNF:

**\<escaped-raw-identifier-token>** = **'\\' '"' \<$unicode-character>+  '"'**

#### Warning

The character contents of an escaped raw identifier token is not allowed to be identical to the character contents of a generic identifier token.

Explanation:

Meaning that what can be written as a generic identifier token should be not also be written as an escaped raw identifier token, this is done to eliminate confusion amongst users/programmers. An example of this confusion would be:

```
dexter    `*This is a generic identifier token*`
\"dexter"  `*while this is an escaped raw identifier token*`
```

Escaped raw identifier token was introduced to write all the things a user/programmer could not normal write with the very limited generic identifier token, anything abnormal identifier goes in escaped raw identifier token entry, a user can start identifiers with numbers `\"1Egg"`, include whitespace unicode characters `\"1 Egg"`  and have fun with emojis `\"Happy Birthday 🎂 and gorilla day 🦍. 😁"`. The sky is literally your limit with escaped raw identifier tokens.



### Literal Tokens

Are tokens that make use of a set of unicode characters that can be combined to form constant values(more in the semantic section).

#### Types Of Literal Tokens

- Number literal tokens
- Text literal tokens

#### Number literal tokens

Are tokens that allow the entry of numbers in nc-lang. Number literal tokens in nc-lang are positional numeral system numbers that support bases **2 ~ 36** and digit separators(**'**). Due to the fact the number literal tokens in nc-lang support bases **2 ~ 36**, unicode characters **A ~ Z** or **a ~ z** are considered valid digits. Detailed explanation of positional numeral system can be found in [***TODO: create a document to explain positional numeral systems***]. Below is the ncBNF entry of unicode characters that are valid number literal token digits:

**\<digits>** = **'0' ~ '9' | 'a' ~ 'z' | 'A' ~ 'Z'**

Also for the digit separator:

**\<digit-separator>** = **_**

The BNF entry for number literal tokens would be shown below after the explanation of the two number literal token types.

#### Types Of Number literal tokens

- Integer number literal tokens
- Real number literal tokens

#### Integer Number Literal Tokens

Are numbers that allow the entry of natural numbers(all digits expect zero) whole numbers(all digits including zero) and negative numbers, they are often simplified as allowing the entry of positive numbers, negative numbers and zero. They are further broken down in high level computing context into two types which are called signed and unsigned integer number literal tokens, but that explanation is saved for the semantic section, for now, they are just simply known as integer number literal tokens. Below is the ncBNF:

**\<integer-number-literal-token>** = **'-'? '0' ~ '9'  (\<digit-separator> \<digits>)\***

Because of the need to disambiguate between identifiers and numbers, digits **'0' ~ '9'** are expected as the first digit in an integer number literal token.

#### Real Number Literal Tokens

Are numbers that allow the entry of rational numbers(fractional numbers - *numbers that are represented as a ratio of two integers* - and integers) and irrational numbers(non fractional numbers - *numbers that cannot be represented as a ratio of two integers*). Real number literal tokens can either be written in either normal fractional number form(***0.0003***) or scientific notation form(***3.0@-4*** or ***3@-4***), because number literal tokens in nc-lang support bases **2 ~ 36**, the exponent base indicator is the base of the number literal token and is denoted by the **@** unicode character - *entry of the base would be shown later*. Below is the ncBNF:

**\<real-number-literal-token>** = **\<integer-number-literal-token>+ ('.' \<integer-number-literal-token>+)? ('@' ('+'|'-' )? ('0' ~ '9')+)?**



#### Text Literals Tokens

Are literal tokens that allow the entry of any unicode character. Apart from allowing unicode character entry, text literal tokens also allow the entry of text literal commands called escaped text literal actions. Escaped text literal actions are a set combination of unicode characters starting with unicode character **\\** that instruct the language to perform a particular action on the text literal. Below are the list of escaped text literal actions all written in ncBNF.

- **'\\n'** : inserts the newline unicode character

- **'\\t'** : inserts the horizontal tab unicode character

- **'\\e'** : inserts the escape unicode character

- **'\\r'** : inserts the carriage return unicode character

- **'\\''** : inserts the apostrophe unicode character

- **'\\"'** : inserts the quotation mark unicode character

- **'\\' \<$whitespace-char>+** : discards any whitespace character till a non whitespace character is found

- **'\\'\<unicode-character-code-point>**

  - **\<unicode-character-code-point>** = **'['\<integer-number-literal-token>']'**

    > **NOTE**: when no base(radix) is specified for the integer number literal token, instead of defaulting to base 10, the language defaults to base 16

- **'\\'\<unicode-character-name>**

  - **\<unicode-character-name>** = **'{'\<generic-identifier-token> (' '|'-') \<generic-identifier-token>'}'**

    > It is like this to allow something like this: "\\{face with crossed-out eyes}"

- **'\\'\<pure-text-literal>** : allows the entry of unicode characters without escaped text literal actions

  - **\<pure-text-literal>** = **'#'{1, n} '{' <$any-unicode-character>+ '}' '#'{1, n}**

    > Remember, the curly brace operator in the above ncBNF asks for at least one or exactly ***n*** iterations of the unicode character ***#***, meaning the number of both ***#*** must be the same

#### Types of Text Literal Tokens

- Character text literal token
- String text literal token

#### Character Text Literal Tokens

Are text literal tokens that allow the entry of exactly one unicode character or escaped text literal action. Below is the ncBNF:

**\<character-text-literal-token>** = **'\\'' (<$any-unicode-character>|\<escaped-text-literal-actions>) '\\''**

#### String Text Literal Tokens

Are text literal tokens that allow the entry of zero or more unicode character or escaped text literal action. Below is the ncBNF:

**\<string-text-literal-token>** = **'"' (\<any-unicode-character>|\<escaped-text-literal-actions>)\* '"'**



---

Because literal token types have a special association with data types in nc-lang, they support specification of literal token data type entry which would be further explained in the semantic section, but the ncBNF shown here:

**\<number-literal-token>** = **(\<integer-number-literal-token>|\<real-number-literal-token>) ('₀' ~ '₉')\* '\'' \<identifier-token>**

> **NOTE**: when no base(radix) is specified for a number literal token, the language defaults to base 10

**\<text-literal-token>** = **\<identifier-token>(\<character-text-literal-token>|\<string-text-literal-token>)**



\<literal-token> = \<number-literal-token>|\<text-literal-token>

----



### Symbol Tokens

Are the remaining set of unicode characters that are used in the language. They are called symbols because the language uses them as such and unlike identifier and literal tokens, individual symbol tokens cannot be combined to form user defined symbols tokens, meaning they are fixed and not extendable. Below are the     unicode characters that form the list of symbol tokens in nc-lang with their unicode names and code-points:

- **+**  Plus sign `U+002B`
- **-**  Hyphen-minus `U+002D`
- **%**  Percent sign `U+0025`
- **×**  Multiplication sign `U+00D7`
- **÷**  Division sign `U+00F7`
- **^**  Circumflex accent `U+005E`
- **=**  Equals sign `U+003D`
- **:=**  Colon and Equals sign `U+003A and U+003D`
- **+=**  
- **-=**
- **=-**
- **×=**
- **÷=**
- **=÷**
- **%=**
- **=%**
- **^=**
- **==**
- **≠**
- **<**
- **>**
- **≤** Less than or equal to `U+2264`
- **≥** Greater than or equal to `U+2265`
- **!**  Exclamation mark `U+0021`
- **~**
- **~.**
- **∈**  Element of unicode character `U+2208`
- **∉**  Not an element of unicode character `U+2209`
- **:**  Colon `U+002C`
- **::**  Colon and Colon `U+002C and U+002C`
- **(**
- **)**
- **{**  Left curly bracket `U+007B`
- **}**  Right curly bracket `U+007D`
- **[**  Left square bracket `U+005B`
- **]**  Right square bracket `U+005D`
- **\*** Asterisk `U+0026`
- **&**  Ampersand `U+0026`
- **#**  Number sign `U+0023`
- **@**
- **=>**
- **->**
- **,** Comma `U+002C`
- **.** Full stop `U+002E`
- **;**  Semicolon `U+003B`
- **...**
- **?**

---

> **Each of these token types would be expanded upon more in the semantic section**.



## Items and LCI Syntax

Items are collection of tokens that are used to define high level constructs whose primary objective is to be used in working with data.

### Types Of Items

- Data type items
- Declaration items
- Expression items
- Statement items
- Unit item
- Language communication interface (LCI) items

#### NOTE

There are some ncBNF replacement placeholders used in some items ncBNF that would not be defined under the Item and LCI syntax section, but would be defined later in this document, due to that, those ncBNF replacement placeholders would be coloured red to indicate it's missing definition.

### Data Type Items

Are items that are used to define the types of data in nc-lang. This is an item because nc-lang is statically typed -*meaning the types of data must be known before run time*- programming language.

### Types Of Data Type Items

- Variadic data type item
- Non variadic data type item

### Variadic Data Type Item

Are data type items that make the variadic syntax which is not defined in under this section. Below is the ncBNF:

**\<data-type-item>** = <font color=red>**\<at-LCI>**</font> **'type' \<data-type>** <font color=red>**\<variadic-symbol-token>**</font>

**\<data-type>** would be defined in the semantic section, but for now just know it includes user identifier tokens.

### Declaration Items

Are items that allow users/programmers to attach user identifier tokens to high level constructs that are necessary for organising and making sense of data in the language.

### Types Of Declaration Items

- Data type creator declaration items
- Typescope declaration item
- Object declaration item
- Function declaration item
- Userscope declaration item
- Mixin declaration item
- Interface declaration item

### Data Type Creator Declaration Items

Are items that allow users/programmers to define blueprints that specifies the layout and behaviour of data types in nc-lang.

### Types of Data Type Creator Declaration Items

- Struct data type creator declaration item
- Union data type creator declaration item
- Set data type creator declaration item
- Unique type creator declaration item

### Struct Data Type Creator Declaration Item

Is a data type creator declaration item that allows a user/programmer to create a type that represents a combination of zero or more types(technically object declarations). Because of this, it is also referred to as the **and data type creator** in nc-lang where **and** describes the relationship of the combination of types(technically object declarations) in the data type creator declaration item, for example when describing a data type creator declaration item containing two types(technically object declarations), one should say this data type creator declaration item consists of ***type-1 and type-2***. Below is the incomplete ncBNF:

<font color=red> **\<struct-data-type-creator-declaration-item>** </font> = <font color=red> **'struct' \<user-identifier-token> '{' (\<struct-object-declaration> (',' \<struct-object-declaration>)* )+ '}'** </font>

### Union Data Type Creator Declaration Item

Is a data type creator declaration item that allows a user/programmer to create a type that represents a combination of two or more types where one those types is used at a time, meaning of all the types present in a union data type creator declaration item, only one is allowed to be used or active at a given point in time. Because of this, it is also referred to as the **or data type creator** in nc-lang where **or** describes the relationship of the combination of types in the data type creator declaration item, for example when describing a data type creator declaration item containing two types, one should say this data type creator declaration item consists of ***type-1 or type-2***. Below is the incomplete ncBNF:

<font color=red>**\<union-data-type-creator-declaration-item>**</font> = <font color=red>**'union' \<user-identifier-token> '{' \<data-type-item> (',' \<data-type-item>)+  '}'**</font>

### Set Data Type Creator Declaration Item

Is a data type creator declaration item that allows a user/programmer to create a type that allows them to define the zero or more set of values that are then treated as valid values for a type of set data type creator declaration item origin, meaning a set data type creator declaration item basically allows a user/programmer to define the values of it's type. Below is the incomplete ncBNF:

<font color=red>**\<set-data-type-creator-declaration-item>**</font> = <font color=red>**'set' \<user-identifier-token> '{' (\<user-identifier-token> (',' \<user-identifier-token>)*)+ '}'**</font>

### Unique Data Type Creator Declaration Item

Is a data type creator declaration item that allows a user/programmer to create a type from an existing data type creation declaration item that is not of unique data type creator declaration item origin. Below is the incomplete ncBNF:

<font color=red>**\<unique-data-type-creator-declaration-item>**</font> = <font color=red>**'unique' \<user-identifier-token> '=' \<data-type-item>**</font>

### Classification Of Declaration Items

- Disclosure declaration items
- Definition declaration items

### Expression Items

Are items that allow the entry of expressions which are high level constructs used in the manipulation of data. Expressions in nc-lang ***can only*** evaluate to a ***single value***, which is in contrast to some programming languages that allow expressions to evaluate to multiple values at the higher level, they do this by optionally making use of entities called operators. The definition would be expanded on more in the semantic section.

#### All Expression Related Technical Definitions

- **Evaluation**: is the process where by an expression is computed.

- **Value**: is simply the result of an evaluated expression.
- **Operators**: are a subset combination of symbol tokens and language identifier tokens that are used to operate on expressions to evaluate to a value.
- **Operands**: are simply expressions that make use of operators. In other words, the expression that an operator operates on is called an operand.

**TODO** make a side by side comparison with an example

Below are the subset of symbol tokens and language identifier tokens that form operators and their formal names as used in the operator context other than that they retain their unicode character names:

- **+**  Addition or plus operator
- **-**  Subtraction or minus operator
- **%**  Remainder operator
- **×**  Multiplication or product operator
- **÷**  Division operator
- **^**  Exponentiation operator
- **=**  Assignment operator
- **:=**  Delay assignment operator
- **+=**  Addition assignment operator
- **-=**  Subtraction assignment operator
- **=-**  Reverse subtraction assignment operator
- **×=**  Multiplication assignment operator
- **÷=**  Division assignment operator
- **=÷**  Reverse division assignment operator
- **%=**  Remainder assignment operator
- **=%**  Reverse remainder assignment operator
- **^=**  Exponentiation assignment operator
- **==**  Equal operator
- **≠**  Not equal operator
- **<**  Ordered before operator
- **>**  Ordered after operator
- **not**  Logical negation operator
- **and**  Logical and operator
- **or**  Logical or operator
- **xor**  Logical exclusive or operator
- **~**  Inclusive range operator
- **~.**  Exclusive range operator
- **∈**  Contains operator
- **∉** Not contains operator
- **()**  Function call operator
- **\***  Get internal type representation operator
- **&**  Value-reference operator
- **.**  Type member item operator
- **try**  Try-fail error/optional handling operator
- **prop**  Propagate-down-call-stack error handling operator
- **=>**  Start error/optional handling operator
- **copy**  Copy value operator
- **addressof**  Get address of value operator
- **memoryof**  Get memory of value operator

### Types Of Operators

- Unary operators
- Binary operators

### Unary Operators

Are operators that operate only on a single operand.

### Types Of Unary Operators

- Prefix unary operators
- Postfix unary operators

### Prefix Unary Operators

Are unary operators that appear before the operand. Below is the ncBNF:

**\<prefix-unary-operator>** = **'&'|'!'|'addressof'|'memoryof'|'copy'|'try'|'prop'**

### Postfix Unary Operators

Are unary operators that appear after the operand. Below is the ncBNF:

**\<postfix-unary-operator>** = **'()'|'=>'**

### Binary Operators

Are operators that operate on two operands. Below is the ncBNF:

**\<binary-operator>** = **'+'|'-'|'×'|'÷'|'%'|'^'|'='|':='|'+='|'-='|'×='|'÷='|'%='|'^='|'=-'|'=÷'|'=%'|'=='|'≠'|'<'|'>'|'not'|'and'|'or'|'xor'|'~'|'~.'|'∈'|'∉'|'\*'|'.'**

### Classification Of Operators

- Arithmetic operators
- Assignment operators
- Logical operators
- Relational operators
- Error/optional handling operators
- Miscellaneous operators

#### [INCOMPLETE]

### Types of Expressions

- Primary expressions
- Unary expressions
- Binary expressions

### Primary Expressions

Are the fundamental expressions in nc-lang, it is from them that the other types of expression are formed.

### Types Of Primary Expressions

- Literal token
- Identifier token
- Block primary expression
- Object declaration primary expression
- Function declaration primary expression
- Enclosed expression

### Block Primary Expression

Is a type of primary expression that allows the entry of certain items that can be used in the process of defining expressions. A block primary expression has odd behaviours depending on the context it is used in, these odd behaviours would be explained in the semantic section.

Below is the ncBNF:

**\<block-expression>** = **'{' (\<declaration-item>|\<expression-item>|\<statement-item>|\<unit-item>)* '}'**

### Object Declaration Primary Expression

Is a type of primary expression that allows the entry of object declarations as expressions, meaning they allow object declarations to be treated by the user/programmer as expressions. But there is a catch to do this, because the object declarations are treated as expressions, they are defined as anonymous to distinguish them from object declarations - *a more comprehensive explanation would be given in the semantic section*. Below is the ncBNF:

**\<object-declaration-primary-expression>** = **\<data-type-item> ( '(' \<expression> ')' )?**

Because the ncBNF query expression after data type item is optional, an object declaration primary expression can end up looking exactly like a data type item, but do not worry as the syntax is not ambiguously defined by the language as data type items aren't whole items so it should be always be clear to a user/programmer that a lone data type item in a block primary expression for example is always an object declaration primary expression - *the reason for this syntax and the omit-ion of the assignment syntax would be explained in the semantic section*.

### Function Declaration Primary Expression

Is a type of primary expression that allows the entry of function declarations as expressions, meaning they allow function declarations to be treated by the user/programmer as expressions. Similar to object declaration primary expression, but for functions and they also share the same catch of them having to be defined as anonymous - *a more comprehensive explanation would be given in the semantic section*. Below is the ncBNF:

**\<function-declaration-primary-expression>** = 'fn' '(' \<parameter> ')'

**\<primary-expression>** = **\<literal-token>|\<user-identifier-token>|\<block-primary-expression>|\<object-declaration-primary-expression>|\<function-declaration-primary-expression>|\<enclosed-expression>**

Remember to add @Identifier for type getting

Classification of operators and their correct usage of them in text like for relational operators

### Expression Precedence And Association

