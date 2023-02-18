/**
 * Lexer for identifying terminals in an input string.  The lexer solve method
 * combines multiple regular expressions into a single deterministic finite
 * automaton (DFA).  This DFA can then be written to source code and later
 * compiled into another program to identify terminals in an input string.
 */

#ifndef lexer_hpp
#define lexer_hpp

#include "literal.hpp"
#include "regex.hpp"
#include "node.hpp"

/**
 * The Lexer provides two ways to add patterns for matching, either a literal
 * string of characters or a regular expression.  After adding all the of
 * expressions, call solve to find the nodes the DFA.  Each node maps an input
 * character to a single next node, creating a finite state machine.  If the
 * next character is not found in any output range of a node, then an accept
 * pointer indicates a match for that terminal.
 */
class Lexer
{
public:
    
    /** Adding expressions, return true if the provided expression is valid. */
    bool add_regex(Term* accept, const std::string& regex);
    bool add_literal(Term* accept, const std::string& series);
    
    /** After adding all expressions, call solve to build to DFA. */
    void solve();
    
    /** After building the DFA, call reduce to minimize the nodes. */
    void reduce();
    
public:
    std::vector<std::unique_ptr<Node>> nodes;
    
private:
    std::vector<std::unique_ptr<Regex>> exprs;
    std::vector<std::unique_ptr<Literal>> literals;
};

#endif
