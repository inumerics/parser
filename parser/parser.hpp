#ifndef parser_h
#define parser_h

#include <string>

std::string parser_header = R""""(

#include <string>
#include <vector>

class Value;
class Table;

struct Symbol {
    const char* name;
};

struct Node {
    Node* (*next)(int c);
    Symbol* accept;
    Value* (*scan)(Table*, const std::string&);
};;

/*******************************************************************************
 * The first class defines the production rule of a nonterminal and it stores
 * the reduce action to call when its sequence of symbols is found in the input.
 * The reduce member variable stores a function that is passed a pointer to the
 * top of the stack and returns a value for the resulting nonterminal.
 */
struct State;

struct Shift {
    Symbol* sym;
    State*  next;
};

struct Rule {
    Symbol* nonterm;
    int length;
    Value* (*reduce)(Table*, Value**);
};

struct Reduce {
    Symbol* sym;
    Rule*   next;
    bool    accept;
};

struct Go {
    Symbol* sym;
    State*  state;
};

struct State {
    std::vector<Shift>*  shift;
    std::vector<Reduce>* reduce;
    std::vector<Go>*     go;
};;

State* find_shift( State* state, Symbol* sym);
Rule*  find_reduce(State* state, Symbol* sym, bool* accept);
State* find_goto(  State* state, Symbol* sym);;

/*******************************************************************************
 * With the grammar and the custom functions defined, the parser generator is
 * run to build the table of actions.  However, two additional functions are
 * needed to fully implement the calculator.  The first function reads the input
 * string one character at a time looking for terminals.  The second function is
 * passed the found terminal and based on the action table either adds the
 * terminal to a stack, or reduces the stack by a grammar rule while calling its
 * associated function.
 *
 * TODO Enumerate the errors.
 */
class ParserImpl : public Parser {
  public:
    ParserImpl(Node* node, State* start, Symbol* endmark);
    
    /** Advance the parser with the next input character. */
    void start();
    virtual bool scan(Table* table, int c);
    virtual bool scan_end(Table* table);

  private:
    Node*   lexer_start;
    State*  parser_start;
    Symbol* endmark;

    /** State of the lexer and the characters of the current symbol. */
    Node* node;
    std::string text;

    /** Stack of the shift reduce parser. */
    std::vector<State*>  states;
    std::vector<Symbol*> symbols;
    std::vector<Value*>  values;

    /** Utility methods for modifing to the stack. */
    void push(State* state, Symbol* sym, Value* val);
    void pop(size_t count);

    /** Either shift the symbol onto the stack, or reduce by a rule. */
    bool advance(Table* table, Symbol* sym, Value* val);
};

)"""";

#endif

