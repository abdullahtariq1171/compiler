The purpose of this project to develop a basic level compiler for C--.

The project has four major parts:
1. Parsing
1. Lexical Analysis
1. Generating Machine Code
1. Virtual Machine (To Execute Machine Code)


The Actual code in written in file `code1.cm` which is passed as argument to `main.cpp` file at the time of execution of compiler.


`words.txt` contains the output of Lexical Analyser i.e. pairs of Lexemes and Tokens  

`identifiers_lex.txt` list of all the variables used in input file (`code1.cm`)

`machineCode.txt` as its name suggest if contains the generated machine code.

`instructionCodes.txt` contains the mapping of instruction code to machine code.

