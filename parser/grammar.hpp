/*******************************************************************************
 * A common way to represent a context free grammar is in Backus-Naur Form or
 * BNF.  The grammar is defined by two types of symbols: terminals and non-
 * terminals.  In the language's definition the terminals are shown in quotes
 * and are defined by a regular expression.  The nonterminals are defined as a
 * sequence of symbols known as a production rule.
 */

#ifndef grammar_hpp
#define grammar_hpp

#include "symbols.hpp"

#include <string>
#include <map>
#include <memory>
#include <sstream>

/*******************************************************************************
 * Reads in a user defined grammar in Backus-Naur Form (BNF).  The class keeps a
 * set of all unique terminal and nonterminals found in the language.  The rules
 * are written as a nonterminal followed by zero or more symbols.  If there is
 * more than one rule for a nonterminal, they are separated by a vertical bar.
 */
class Grammar
{
  public:
    /** Reads in the user defined grammar. */
    bool read_grammar(std::istream& in);
    
    void print(std::ostream& out) const;
    
    /** Nonterminals and rules in the same order as in the grammar. */
    std::vector<Nonterm*> all;
    std::vector<Nonterm::Rule*> rules;
    
    /** Unique terminals and nonterminals of the grammar. */
    std::map<std::string, std::unique_ptr<Term>> terms;
    std::map<std::string, std::unique_ptr<Nonterm>> nonterms;
        
    /** Indicates the end of an input string. */
    class Endmark : public Symbol {
      public:
        virtual void print(std::ostream& out) const;
        virtual void write(std::ostream& out) const;
    };

    Endmark endmark;

    /**
     * Names of files included in the generated source code.  Since the this
     * program writes the reduce methods, the user defined types need to be
     * included with the generated source code.
     */
    std::vector<std::string> includes;
        
    /**
     * The first step to solving all parse states is finding all terminals that
     * could be first in a rule, or could follow after a nonterminal.
     */
    void solve_first();
    void solve_follows(Symbol* endmark);
    
  private:

    /** Recursive descent parser for reading the grammar. */
    bool read_include(std::istream& in);
    bool read_term(std::istream& in);
    bool read_nonterm(std::istream& in);
    bool read_rule(std::istream& in, std::vector<Symbol*>* syms);

    /** Reads attributes of the symbols. */
    bool read_ident(std::istream& in, std::string* name);
    bool read_chars(std::istream& in, std::string* name);
    bool read_regex(std::istream& in, std::string* name);

    /** Buffer the current line of the input for reporting errors. */
    std::string line;
    int number;
    
    /** Returns the next character and adds to the line buffer. */
    char next(std::istream& in);
    
    /** Check for a specific next character. */
    bool check(std::istream& in, char c);
    bool find (std::istream& in, char c);

    /** Consumes whitespace and comments in the input. */
    bool space(std::istream& in);
    bool multiline(std::istream& in);
    bool singleline(std::istream& in);
    
};

#endif
