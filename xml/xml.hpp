/*******************************************************************************
 * An XML (Extensible Markup Language) document is a text-based data file that
 * follows specific rules and syntax for structuring and encoding data. XML
 * documents are designed to represent and store structured data in a way that
 * is both human-readable and easily processed by computers.
 */

#ifndef calculator_hpp
#define calculator_hpp

#include "elements.hpp"

#include <string>
#include <vector>
#include <memory>
#include <map>

/*******************************************************************************
 * An XML document typically starts with an optional XML declaration, which
 * specifies the version of XML being used and may include information about the
 * character encoding used in the document.  Every XML document must have a
 * single root element that encapsulates all other elements within the document.
 * The root element defines the overall structure of the document and serves as
 * the starting point for navigating the data hierarchy.
 */
class Document : public Value
{
  public:
    std::unique_ptr<Element> root;
    
    virtual void print(std::ostream& out);
};

/*******************************************************************************
 * Classes defined by the parser generator to read the text input accourding to
 * the rules of the grammar.  The parser generates a set of nodes for
 * identifying symbols in the input and a state machine for reducing the input
 * symbols by rules of the grammar.
 */

struct Symbol;
extern Symbol endmark;

struct Node;
extern Node node0;

struct State;
extern State state0;

/*******************************************************************************
 * XML parser built with an action table from with a parser generator.
 */
class XMLParser 
{
  public:
    
    /** Clear the stack and place the first state on top. */
    void init();
    
    /** Advances the parser with the next input character */
    bool scan(Table* table, int c);
    
    /** Advances the parser at the end of the input */
    std::unique_ptr<Value> scan_end(Table* table);
    
  private:
    
    Node* node = &node0; /// state of the lexer
    std::string text;    /// input characters of the current symbol
    
    /** Stack of the shift reduce parser */
    std::vector<State*>  states;
    std::vector<Symbol*> symbols;
    std::vector<Value*>  values;
    
    /** Either shift the symbol onto the stack, or reduce by a rule */
    bool advance(Table* table, Symbol* sym, Value* val);
    
    /** Utility methods for modifying to the stack */
    void push(State* state, Symbol* sym, Value* val);
    void pop(size_t count);
};

#endif
