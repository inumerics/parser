/**
 * An XML (Extensible Markup Language) document is a text-based data file that
 * follows specific rules and syntax for structuring and encoding data in a
 * machine-readable format. XML documents are designed to represent and store
 * structured data in a way that is both human-readable and easily processed by
 * computers.
 */

#ifndef calculator_hpp
#define calculator_hpp

#include <string>
#include <vector>
#include <memory>
#include <map>

/**
 * Value is the base class of all arguments passed to the user defined functions
 * of the parser.  The class provides a virtual destructor for releasing
 * memory during parsing.
 */
class Value {
  public:
    virtual ~Value() = default;
};

/**
 * The symbol table is also passed to each user defined function associated with
 * a grammar rule.
 */
class Table {
  public:
    std::map<std::string, int> vars;
    bool done = false;
};

/**
 *
 */
class Name : public Value {
  public:
    Name(const std::string& name) : name(name) {}
    std::string name;
};

/**
 * An XML element consists of an opening tag, content or data, and a closing
 * tag. The opening tag defines the element's name and may also include
 * attributes that provide additional information about the element. The content
 * or data between the opening and closing tags represents the actual
 * information or data associated with that element.
 */
class Tag : public Value
{
  public:
    Tag(const std::string& name) : name(name) {}
    std::string name;
};

/**
 * The contents between the opening and closing tags of an XML element contain
 * the actual data or information associated with that element.  This is the
 * base class for the different contents of an xml element.  This content can
 * vary depending on the context and purpose of the document. The context may
 * include text, numbers, other elements, or a combination of these.
 */
class Content : public Value
{
  public:
    virtual void print(std::ostream& out, const std::string& ident) {}
};

class Contents : public Value
{
  public:
    std::vector<std::unique_ptr<Content>> contents;
};

/**
 * Attributes are additional pieces of information that can be associated with
 * an XML element. They provide metadata or characteristics about the element
 * itself and are typically used to convey information that is not part of the
 * element's content. Attributes are defined within the opening tag of an XML
 * element and consist of a name-value pair.
 */
class Attr : public Value
{
  public:
};

class Attrs : public Value
{
  public:
    std::vector<Attr> items;
};

/**
 * An XML (Extensible Markup Language) element is a fundamental building block
 * of an XML document, used to define and structure data or content within the
 * document. An XML element consists of three main parts:
 *
 *  Opening Tag: The opening tag encloses the element's name within angle
 *      brackets, and it is the starting point for defining the element.
 *  Closing Tag: The closing tag is similar to the opening tag but includes a
 *      forward slash before the element name.
 *  Content: The content is the data or information enclosed between the opening
 *      and closing tags of the element.  This content can vary widely and can
 *      include text, numbers, other elements, attributes, or any structured
 *      data that is meaningful within the context of the XML document.
 */
class Element : public Content
{
  public:
    std::unique_ptr<Tag> open;
    std::unique_ptr<Tag> close;
    std::vector<std::unique_ptr<Content>> contents;
    std::string name;
    
    virtual void print(std::ostream& out, const std::string& ident);
};

class Elements : public Value
{
  public:
    std::vector<Element> items;
};

/**
 * An XML (Extensible Markup Language) document is a text-based data file that
 * follows specific rules and syntax for structuring and encoding data in a
 * machine-readable format.  Declaration: An XML document typically starts with
 * an optional XML declaration, which specifies the version of XML being used
 * and may include information about the character encoding used in the
 * document.
 * Root Element: Every XML document must have a single root element that
 * encapsulates all other elements within the document. The root element defines
 * the overall structure of the document and serves as the starting point for
 * navigating the data hierarchy.
 */
class Document : public Value
{
  public:
    std::unique_ptr<Element> root;
    
    virtual void print(std::ostream& out);
};

/**
 * Classes defined by the parser generator to read the text input accourding to
 * the rules of the grammar.  The grammar rules are written as a nonterminal
 * followed by zero or more symbols.  The parser generates a set of nodes for
 * identifying symbols in the input and a state machine for reducing the input
 * symbols by rules of the grammar.
 */

struct Symbol;
extern Symbol endmark;

struct Node;
extern Node node0;

struct State;
extern State state0;

/**
 * Calculator built with an action table from with a parser generator.
 */
class Calculator {
  public:
    void start();
    
    // advance the parser with the next input character
    bool scan(Table* table, int c);
    
    // advance the parser at the end of the input
    std::unique_ptr<Value> scan_end(Table* table);
    
  private:
    
    // state of the lexer and input characters of the current symbol
    Node* node = &node0;
    std::string text;
    
    // stack of the shift reduce parser
    std::vector<State*>  states;
    std::vector<Symbol*> symbols;
    std::vector<Value*>  values;
    
    // utility methods for modifying to the stack
    void push(State* state, Symbol* sym, Value* val);
    void pop(size_t count);
    
    // either shift the symbol onto the stack, or reduce by a rule
    bool advance(Table* table, Symbol* sym, Value* val);
};

#endif
