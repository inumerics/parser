/**
 * A grammar for specifying a regular language can be defined using two types of
 * symbols: terminals and nonterminals.  The terminals are the smallest unit of
 * the grammar and often represent a specific pattern of characters, such as an
 * integer.  The nonterminals are defined by production rules, which are a
 * sequences comprised of both terminals and nonterminals.
 */

#ifndef symbols_hpp
#define symbols_hpp

#include "finite.hpp"
#include <string>
#include <vector>
#include <set>

/**
 * Non-terminals of a grammar.  Building a parser requires knowing all terminals
 * that could be the first terminal in a rule.  This class determines the
 * terminals that could start a rule and all those that could follow a non-
 * terminal.
 */
class Nonterm : public Symbol
{
  public:
    Nonterm(const std::string& name);
    std::string name;
    size_t id = 0;
    
    /**
     * All nonterminals have one or more production rules, vectors of symbols,
     * that list the symbol sequence that defines a given nonterminal.  Each
     * rule has an action called when the parser matches it in the input and
     * reduces its sequence of symbols to a nonterminal.  These custom actions
     * implement the functionality of the final program being built.
     */
    class Rule {
      public:
        Rule(Nonterm* nonterm);
        Rule(Nonterm* nonterm, const std::string& action);
        
        Nonterm* nonterm;
        std::vector<Symbol*> product;
        std::string action;
        size_t id = 0;
        
        void print(std::ostream& out) const;
        void write(std::ostream& out) const;
    };
    
    std::vector<std::unique_ptr<Rule>> rules;
    
    /**
     * To find all possible parse states, the first step is to solve for all
     * terminals that could be the first in the productions for each non-
     * terminal. A nonterminal can also have an empty production rule of no
     * symbols.
     */
    std::set<Symbol*> firsts;
    bool empty_first;
    void solve_first(bool* found);
    
    /**
     * After finding the firsts, solve for all terminals that could follow each
     * nonterminal.
     */
    std::set<Symbol*> follows;
    void solve_follows(bool* found);
    
    /**
     * The first step to solving all parse states is finding all terminals that
     * could be first in a rule, or could follow after a nonterminal.
     */
    static void
    solve_first(std::vector<std::unique_ptr<Nonterm>>& nonterms);
    
    static void
    solve_follows(std::vector<std::unique_ptr<Nonterm>>& nonterms,
                  Symbol* endmark);
    
    /** Prints the input grammar in BNF form. */
    virtual void print(std::ostream& out) const;
    virtual void write(std::ostream& out) const;
    
    void print_rules(std::ostream& out) const;
    void print_firsts(std::ostream& out) const;
    void print_follows(std::ostream& out) const;
    
    static void solve_firsts(const std::vector<Symbol*>& symbols,
                             std::set<Symbol*>* firsts);
    
private:
    /** Called by solve for finding the set of firsts and follows. */
    void insert_firsts(Rule* rule,
                       bool* found);
    
    void insert_follows(const std::set<Symbol*>& syms,
                        bool* found);
    
    void insert_follows(std::vector<Symbol*>::iterator symbol,
                        std::vector<Symbol*>::iterator end,
                        bool* epsilon,
                        bool* found);
};

/** Indicates the end of an input string. */
class Endmark : public Symbol {
public:
    virtual void print(std::ostream& out) const;
    virtual void write(std::ostream& out) const;
};


#endif
