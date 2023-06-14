/**
 * JSON parser built using a parser generator.  The generator reads the grammar
 * for json format and writes action tables.  These action tables are compiled
 * along with the user defined functions to build the JSON reader.
 */
#ifndef json_hpp
#define json_hpp

#include <string>
#include <vector>
#include <memory>
#include <map>

/**
 * Base class for values built during parsing.  The class provides a virtual
 * destructor to release memory during parsing.
 */
class Value {
  public:
    virtual ~Value() = default;
};

class Table {
};

/**
 * Base class for all JSON values built during parsing.  The JSON array and
 * object types use pointers to this class for the JSON values they contain.
 */
class JsonValue : public Value {
};

/**
 * Class for each JSON data type.
 */
class JsonNumber : public JsonValue {
  public:
    JsonNumber(double value);
    double value;
};

class JsonBool : public JsonValue {
  public:
    JsonBool(bool value);
    bool value;
};

class JsonNull : public JsonValue {
  public:
};

class JsonString : public JsonValue {
  public:
    JsonString(const std::string& value);
    std::string value;
};

class JsonArray : public JsonValue {
  public:
    std::vector<std::unique_ptr<JsonValue>> values;
};

class JsonObject : public JsonValue {
  public:
    std::map<std::string, std::unique_ptr<JsonValue>> values;
};

/**
 * Classes defined by the state machine to parse a user defined grammar.  The
 * rules of the grammar are written as a nonterminal followed by zero or more
 * symbols.  The parser generates a set of nodes and states for identifing
 * symbols in a string and reducing the input symbols by rules of the grammar.
 */
struct Rule;
struct Symbol;
extern Symbol endmark;

struct Node;
extern Node node0;

struct State;
extern State state0;

/**
 * Function defined by the lexer for identifing terminals in an input string.
 * Node next determines the next node for a given input character.  If no node
 * is returned, check the current node to determine the accepted symbol for the
 * input string.  With the accepted symbol, call scan to get the value for the
 * symbol.
 */
Node*   node_next(Node* node, int c);
Symbol* node_accept(Node* node);
Value*  node_scan(Node* node, Table*, const std::string&);

/**
 * Function defined by the parser.  For each new symbol, the parser determines
 * if the symbol should be pushed onto the stack, or reduced by a rule of the
 * grammar.  If the top symbols are reduced by a rule, call the reduce function
 * to determine the new value for the top of the stack.  After reducing the
 * rule, call goto to determine the next state of the parser.
 */
State* find_shift(State* state, Symbol* sym);
Rule*  find_reduce(State* state, Symbol* sym, bool* accept);

Symbol* rule_nonterm(Rule* rule, size_t* length);
Value*  rule_reduce(Rule* rule, Table*, Value**);

State* find_goto(State* state, Symbol* sym);

/**
 * With the grammar and the custom functions defined, the parser generator is
 * run to build the table of actions.
 */
class Parser {
  public:
    void start();
    
    /** Advance the parser with the next input character. */
    bool scan(Table* table, int c);
    
    /** Advance the parser at the end of the input. */
    bool scan_end(Table* table);
    
  private:
    
    /** State of the lexer and input characters of the current symbol. */
    Node* node = &node0;
    std::string text;
    
    /** Stack of the shift reduce parser. */
    std::vector<State*>  states;
    std::vector<Symbol*> symbols;
    std::vector<Value*>  values;
    
    /** Utility methods for modifying to the stack. */
    void push(State* state, Symbol* sym, Value* val);
    void pop(size_t count);
    
    /** Either shift the symbol onto the stack, or reduce by a rule. */
    bool advance(Table* table, Symbol* sym, Value* val);
};

#endif
