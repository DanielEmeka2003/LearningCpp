# LearningCpp

Cpp cloud repository for vs code.

This repository contains code, sometimes unrelated, that are products of my C++ learning journey.
Reference to [learncpp.com](www.learncpp.com) <sub>amazing for learning c++<sub>

Ubuntu-linux is the type of operating system that tends to make easy things hard and hard things easy, while windows is the opposite.

## 01/01/2024 Introduction to my Programming Language NC

Hello to the parties interested, i am currently working on a compiler for my programming language called **NC** - short for not C;
I also have a partially-implemented pl0 compiler that compiles down to the **C** programming language.

### 07/01/2024 Utf8 Finally

I have wriiten a custom utf8 character class and also made a utf8 string class from std::basic_string<CharT, CharTraits> to enable unicode support in my language.
It can be found in file [myfunctions1.cpp](myfunctions1.cpp).

### Future Nc-lang projects

- nc_lang_fmt: nc language formatter
- nc_c: nc compiler
- nc_i : nc interpreter
- nc_lnk: nc linker
- nc_dbg: nc debugger
- nc_terminal: nc terminal
- nc_pkg: nc package manager
- nc_browser: nc browser which only purpose of building it is to natively support nc lang via the interpreter and compiler
- nc_os: nc operating system which combines my windows and ubuntu experience into one

### Other Projects - Animations

1. True sight **series**. Below are prequel/sequel movies of the show
    - True sight: memories **movie**
    - True sight: predecessors **movie**
    - True sight: creation **movie**
    - True sight: retribution(a true sight what-if movie) **movie**
2. Disobedience **series**
3. The All father **series**
4. Brotherhood **series**
5. Our saviour **series**
6. The real story **series**
7. Laws of the universe **movie**
8. Chosen **series**
9. Evil: among us **series**
10. Evil: within us **series**

### 05/03/2024 MORE DETAILS ON MY LANGUAGE

My language contains or is going to contain various existing programming features and more:

- Produces useful error messages called logs
- Support for named sum and product types
- Support for un-named sum and product types
- Support for match expressions
- Object-orientated programming
- Procedure programming
- Functional programming
- Concurrent programming
- Meta programming taken to a whole new level
- CompileTime ability to run code
- Powerful implementation of compiler attributes
- Generics support
- **My very own Decimal floating-point format** implementation devoid of the inherent problems of the **IEEE-binary floating-point format**
- Rich type system
- Static and strong typing, with no implicit conversions
- When the time comes, **concurrent, asynchronus and event-driven** programming
- Low enough level language that supports safe memory referencing
- Memory safety(done with no Gargbe collector)
- Bounds checking
- No explicit memory allocation and no explicit memory deallocation inherent to the language
- Error handling scheme that involves forcibly handling of errors through the following ways:
  - Throwing of exceptions that must either be handled by the most recent recipent of the exception or be marked un-handled then propagated
  - Returning errors as shared values in an **expected object of sumType origin**, similar to **rust's Result sumType**
  - Panicing with the panic and panic_fmt functions for unrecoverable errors
- Array indexing starting from **1**
- Unicode agnostic language, with capability of using ***valid multi-language-text and emoji as identifiers***
- Minimalist array of language keywords
- Pseudo-expressive centered language
- A variation of UFC(Unified function call) support
- Ability to imbed other languages to **nc** like markup-language(Html) or programming-language(C)  *a bit optimistic for now*
- Language built-in support for piping output values as input values
- Wide range of operators to be listed soon
- It's very own regex implementation, different from the standard
- It's own implementation of a list structure that supports random access, different from an array list

The files invloved in my programming language building journey

- [NcLog.h](header/NcLog.h) _: contains class Nc::Log, responsible for error logging_
- [NcLexer.h](header/NcLexer.h), [NcLexer.cpp](NcLexer.cpp) _: contains class Nc::Lexer, responsible for lexing nc files_
- [NcParser.h](header/NcParser.h), [NcParser.cpp](NcParser.cpp) _: contians class Nc::Parser, responsible for parsing nc files(also contains BNF for my language)_
- [NcNonTerminal.h](header/NcNonTerminal.h), [NcTerminal.h](header/NcTerminal.h) _: contians classes that represent Ast nodes in my language_
- [NcAst.h](header/NcAst.h), [NcAst.cpp](NcAst.cpp) _: contains class Nc::Ast, responsible for building Nc's Ast. The Ast is really beautiful, will show later_
- [NcSemantics.h](header/NcSemantics.h) and [NcSemantics.cpp](NcSemantics.cpp) _: currently empty, but would contain class Nc::Semantics that would be responsible for semantic checking_
- [NcCodeGen.h](header/NcCodeGen.h) and [NcCodeGen.cpp](NcCodeGen.cpp) _: currently deprecated for now(but i am using LLVM for codegen)_
- functionality only independent files: [readfile.h](header/readfile.h), [drawboxaroundtext.h](header/drawboxaroundtext.h), 
[myfunctions1.h](header/myfunctions.h)  [myfunctions1.cpp](myfunctions1.cpp)

Below is an example error produce from my language, NC:
![Example of a parser error in NC](image.png)

Thanks to ***07/01/2024 unicode update***, my language is now unicode agnostic, but some unicode characters are still not allowed as identifiers.
I haven't solidified acceptable unicode characters yet, but i will when i have time, or if you wish to help in that field, pls contact me through my email
[emekacryil@gmail.com](emekacryil@gmail.com)

> [!NOTE]
> **i rarely do commit, but if you want to view my local changes, you can always just ask, so that i would commit the new changes**
 

## LICENSE

none for now, but pls in good conscience, when you need to distribute my code add the source from where it came from, thank you.


Note:
Regarding the file extention(.nc) of my language, it bares no affiliation whatsoever with the programming language nesC(Networked Embedded Systems C), it only shares the same file extention. Although nesC does have other file extentions like: .nesC and .nsc, .nc is its most prominent.
I do not intend to change the file extention of my language but i do admit it introduces conflict with exsiting systems programmed to recognize programming languages by their file-extention alone, example: github, text-editors and various other technologies, so i plan on introducing a compromise soon.

[^1]: ***A more better readme file is coming soon, explaining all the various file meaning in this repo***