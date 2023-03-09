# Parser Generator

A parser generator simplifies the development of programs such as calculators
and compilers by defining the source code for reading input text.  The generator
takes a user defined grammar and develops tables that describe which functions
to call when parsing the input.  These tables along with the user provided 
functions are compiled together to build the final program.

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

## Defining the Terminals
Terminals represent the smallest unit of the language, such as a number or an 
operator. This parser generator uses the regular expression syntax to provide an 
easy way to define terminals. As an example, the following line defines a 
number terminal whos string consists of one or more digits.

```
    'num' [0-9]+;
```
    
The regular expression syntax provides operators for matching patterns, such as 
repeated letters or numbers. The + operator indicates that the match contains 
one or more of the previous character and the \* operator matches zero or more.
Finally, the ? marks that the previous character is optional.

As shown with the previous definition of a number, the syntax also allows 
matching a range of characters by using brackets and a dash between the first 
and last characters in the range. To enable more complex patterns, the vertical 
bar, |, defines an expression that matches either one of patterns on the two 
sides of the bar. This operator is seen in the previous definition of the 
hexadecimal number, which is comprised of a digit or the letters A through F.

```
    'hex'<Value>  0x([A-Z]|[0-9])+   &scan_hex;
```
    
The addition of parenthesis is the last operator needed to fully utilize the 
syntax of regular expressions. Since there is a defined operator precedence, the 
parenthesis allows the grouping of an expression to control the order of 
operations when reading the pattern. This is similar to their use making an 
addition operation evaluated before a multiplication. The use of the parenthesis 
is also shown by the hexadecimal number which matches one or more of either a 
digit or a letter from A to F.

For convenience, terminals can also be defined within a production rule of the 
nonterminal. This is shown in the previous grammar when defining the rule for 
addition or multiplication using the + and \* infix operators. When written 
within a rule the terminal is not defined using a regular expression, but a 
series of printable characters. This simple definition allows characters which 
are normally regular expression operators to act as a terminal in the grammar. 
Also, when only defined within a production rule there is no type associated 
with the terminal.

```
    add<Value>: add '+' mul;
``` 

## Defining the Nonterminals
A common way to represent a context free grammar is in Backus-Naur Form or BNF. 
The grammar is defined by two types of symbols: terminals and nonterminals.  The 
nonterminals are defined as a sequence of symbols known as a production rule. 
These rules are written as a nonterminal followed by zero or more symbols. If 
there is more than one possible rule associated the same nonterminal, they are 
separated by a vertical bar. A semicolon indicates the end of the rules for a 
given nonterminal. The following statement defines a multiplication product to 
be either a number, or a previous product times a number.

```
    mult: 'num' | mult '*' 'num';
```
        
The basic listing for the rules of our simple calculator can now be written. 
This provides the definition of a number and defines the operators for addition 
and multiplication. Defining the addition nonterminal to be comprised of a 
multiplication nonterminal results in the multiplication operator having a 
higher precedence than addition. For this generator, the first rule specified is 
the rule that is followed by the end of the input string and therefore the last 
nonterminal left after reading the entire input.
    
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

