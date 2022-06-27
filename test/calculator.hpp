/*******************************************************************************
 * Four function calculator built using a parser generator.  The parser reads
 * the grammar for a calculator and generates action tables.  These action
 * tables are compiled along with the user defined functions to build the
 * calculator.
 */
#ifndef calculator_hpp
#define calculator_hpp

#include <string>
#include <vector>

/*******************************************************************************
 * Base class of all arguments passed to the user defined functions of the
 * grammar.
 */
class Value {
  public:
    virtual ~Value() = default;
};

/*******************************************************************************
 * Custom classes for the values of nonterminals of the calculator's grammar.
 */
class Expr : public Value {
  public:
    Expr(int value);
    int value;
};

/*******************************************************************************
 * Symbol table passed to the user defined actions of the grammar.
 */
class Table {
};

/*******************************************************************************
 * This parser generator builds tables for a shift-reduce parser.  To include
 * the shift-reduce parser in another program, this parser class maintains a
 * stack of parse states, symbols and values.  This stack is updated based on
 * the the symbol just read from the input and the actions defined in the parse
 * table.
 */
class Parser {
  public:
    static std::unique_ptr<Parser> build();
    virtual ~Parser() = default;
    
    enum Result {
        Ready,              /// Ready for another character.
        Unexpected_Char,    /// The character resulted in no matching terminal.
        Unexpected_Symbol,  /// The next terminal did not match any rule.
        Done                /// Successfully matched an accepting rule.
    };
    
    virtual Result scan(Table* table, int c) = 0;
    virtual Result scan_end(Table* table) = 0;
};

#endif
