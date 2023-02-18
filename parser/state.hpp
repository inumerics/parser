/*******************************************************************************
 * Possible state of a parser that occurs while reading a valid string for the
 * given grammar.  While reading the input, the parser must be within at least
 * one rule while waiting for the next symbols of that rule.
 */

#ifndef state_hpp
#define state_hpp

#include "symbols.hpp"

#include <map>

/*******************************************************************************
 * A state is comprised of the possible rules that could be being read.  The
 * Item class represents one these possibilities.  A mark indicates the location
 * in the possible rule after reading some terminals.  Symbols to the left of
 * the mark have been seen, and symbols to the right could be seen later in the
 * input.
 */
class Item {
public:
    Item(Nonterm::Rule* rule, size_t mark, Symbol* ahead);
    
    Nonterm::Rule* rule;
    size_t mark;
    Symbol* ahead;
    
    Item advance();
    std::vector<Symbol*> rest();
    
    Symbol* next();
    Nonterm* next_nonterm();
    
    bool operator==(const Item& other) const;
    bool operator<(const Item& other) const;
    
    void print(std::ostream& out) const;
};

/*******************************************************************************
 * Possible state of the parser.  Each state contains rules that could be
 * matched after having read the previous input symbols.  At any given time the
 * parser must be within at least one rule while waiting for one of several
 * symbols that could be next based on the grammar.
 */
class State
{
public:
    State(size_t id);
    size_t id;
    
    std::set<Item> items;
    void closure();
    
    std::map<Symbol*, State*> nexts;
    
    std::unique_ptr<State> solve_next(Symbol* symbol, size_t id);
    
    /** Shift or reduce actions for a given state and next symbol. */
    class Actions {
    public:
        size_t id = 0;
        std::map<const Symbol*, State*> shift;
        std::map<const Symbol*, Nonterm::Rule*> accept;
        std::map<const Symbol*, Nonterm::Rule*> reduce;
        Nonterm::Rule* any = nullptr;
        
        bool is_same(const std::unique_ptr<Actions>& other) const;
        
        /** Compress the action size by combining reduce actions. */
        void combine_reduce();
    };
    
    /**
     * Similar to the states, while there is one set of actions for each state,
     * some states have the exact same actions.  The individual parse states
     * refernce this unique set of found actions.
     */
    Actions* actions = nullptr;
    std::unique_ptr<Actions> solve_actions(Item accept);
    
    /** Defines the next parse state after reduction of a rule. */
    std::map<Symbol*, State*> gotos;
    
    void solve_gotos();
    
    void print(std::ostream& out) const;
    void print_items(std::ostream& out) const;
    
private:
    // TODO Move to the nonterm.
    static void firsts(const std::vector<Symbol*>& symbols,
                       std::set<Symbol*>* firsts);
};

#endif
