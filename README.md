# LearningCpp
Cpp cloud repository for vs code.

This repository contains code, sometimes unrelated, that are products of my C++ learning journey - I wish i chose another languague though.

References to sites like:
[link](Programiz.com)
[link](codecademy.com)
[link](tutorialspoint.com)
[link](learncpp.com)
and many others.

[01/01/2024] INTRODUCTION TO MY PROGRAMMING LANGUAGE NC
 Hello to the parties interested, i am currently working on a compiler for my programming language called NC - short for not C;
 I also have a partially-implemented pl0 compiler that compiles down to the C programming language.

[07/01/2024] UPDATE: UTF8 FINALLY
 I have wriiten a custom utf8 chracter class and also made a utf8 string class from std::basic_string<CharT, CharTraits> enable unicode support in my language.
 It can be found in file myfunctions1.cpp. Fuck icu and boost, i made my own😎.

[05/03/2024] MORE DETAILS ON MY LANGUAGE
 My language contians or is going to contian various existing programming features and more: sumTypes, productypes, classes, function as objects, compileTime    ability to run code, generics(in form of templates), match expressions, attributes and so much more.
 It is going to be as expressive as Rust/Lisp(without marcos) and as fuild and intuitive as C++(cough...cough...👀)

 The files invloved in my programming language building journey
 (1). header/NcLog.h                                     │: contains class Nc::Log, responsible for error logging
 (2). header/NcLexer.h and NcLexer.cpp                   │: contains class Nc::Lexer, responsible for lexing nc files
 (3). header/NcParser.h and NcParser.cpp                 │: contians class Nc::Parser, responsible for parsing nc files(also contains BNF for my language)
 (4). header/NcNonTerminal.h and header/NcTerminal.h     │: contians classes that represent Ast nodes in my language
 (5). header/NcAst.h and header/NcAst.cpp                │: contains class Nc::Ast, responsible for building Nc's Ast
 (6). header/NcSemantics.h and NcSemantics.cpp           │: currently empty, but would contain class Nc::Semantics that would be responsible for semantic checking
 (7). header/NcCodeGen.h and NcCodeGen.cpp               │: currently deprecated for now(but i am using LLVM for codegen)
 (8). functionality independent files:
        header/readFile.h,
        header/drawboxaroundtext.h,
        header/myfunctions.h and myfunctions.cpp

 Below shows an example error produce from my language, NC
 ![NC_Parser_Error](image.png)

 Thanks to ([07/01/2024] unicode update), my language is unicode agnostic, but some unicode characters are still
 not allowed as identifiers - i haven't solidified acceptable unicode characters yet, but i will when i have time, or
 if you wish to help in that field, pls contact me through my email: ![my gmail](emekacryil@gmail.com)

 [NOTE] i rarely do commit, but if you badly want to see my local changes, you can always just ask, so i would commit the new changes.
 

LICENSE: none for now. But pls in your good consicience, when you need to distribute the code add the source from where it came from, thank you.