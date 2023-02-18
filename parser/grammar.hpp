/**
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

/**
 * Reads in a user defined grammar in Backus-Naur Form (BNF).  The class keeps a
 * set of all unique terminal and nonterminals found in the language.  The rules
 * are written as a nonterminal followed by zero or more symbols.  If there is
 * more than one rule for a nonterminal, they are separated by a vertical bar.
 */
class Grammar
{
public:
    std::vector<std::unique_ptr<Nonterm>> nonterms;
    std::vector<std::unique_ptr<Term>> terms;
    Endmark endmark;
    
    /** Reads in the user defined grammar. */
    bool read_grammar(std::istream& in);
    
    /**
     * Names of files for included directives in the generated source code.
     * Since this program writes the reduce methods, the user defined types need
     * to be provided to the generated source code.
     */
    std::vector<std::string> includes;
    
    void print(std::ostream& out) const;
    
private:
    /** Lookups for already defined sysmbols */
    std::map<std::string, Nonterm*> nonterm_names;
    std::map<std::string, Term*> term_names;
    
    /** Recursive descent parser for reading the grammar. */
    bool read_include(std::istream& in);
    bool read_term(std::istream& in);
    bool read_nonterm(std::istream& in);
    bool read_rule(std::istream& in, std::vector<Symbol*>* syms);
    
    /** Reads attributes of the symbols. */
    bool read_ident(std::istream& in, std::string* name);
    bool read_chars(std::istream& in, std::string* name);
    bool read_regex(std::istream& in, std::string* pattern);
    
    /** Buffer the current line of the input for reporting errors. */
    std::string line;
    int line_number = 1;
    
    /** Returns the next character and adds to the line buffer. */
    char next(std::istream& in);
    
    /** Check for a specific next character. */
    bool check(std::istream& in, char c);
    bool find (std::istream& in, char c);
    
    /** Consumes whitespace and comments in the input. */
    void space(std::istream& in);
    void multiline(std::istream& in);
    void singleline(std::istream& in);
};

#endif
