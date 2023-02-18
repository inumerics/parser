# Parser Generator

A parser generator simplifies the development of programs such as calculators
and compilers by defining the source code for reading input text.  The generator
takes a user defined grammar and develops tables that describe which functions
to call when parsing text.  These tables and the user provided functions are
compiled together to build the final program.

A common way to define a context free grammar is the Backus-Naur Form.  The
grammar is defined by two types of symbols: terminals and nonterminals.  The
terminals are shown in quotes and are defined by a regular expression.  The
nonterminals are a sequence of symbols known as a production rule.  The
following text is an example grammar for a four function calculator.

```
'num'<Expr>  [0-9]+             &scan_num;
'hex'<Expr>  0x([A-Z]|[0-9])+   &scan_hex;

total<Expr>: <add        &reduce_total
    ;
add<Expr>: mul
    | add '+' mul       &reduce_add_mul
    | add '-' mul       &reduce_sub_mul
    ;
mul<Expr>: int
    | mul '*' int       &reduce_mul_int
    | mul '/' int       &reduce_div_int
    | '(' add ')'       &reduce_paren
    ;
int<Expr>: 'num'
    | 'hex'
    ;
```

## Building
```
    make
    make calculator
```

## Example Programs

The source code includes an example program with a grammar and functions that
implement a four function calculator.  With the calculator's language as an
input, the parser program generates the action tables.  These action tables are
then compiled along with the user defined functions to build a calculator
program.

- [Calculator Grammar](https://github.com/inumerics/parser/blob/main/calculator/calculator.bnf)
- [Calculator Header ](https://github.com/inumerics/parser/blob/main/calculator/calculator.hpp)
- [Calculator Source ](https://github.com/inumerics/parser/blob/main/calculator/calculator.cpp)

## Example Output

The output of the parser generator is source code.  This source code is then
compiled into another program for parsing an input string.  The following source
is an example of the code written by the generator to implement a parser. The
generated source code defines several classes to store the grammar rules
and actions of the parser.

- [Example Output](https://github.com/inumerics/parser/blob/main/output.md)

