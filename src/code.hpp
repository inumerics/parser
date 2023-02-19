/*******************************************************************************
 * Writes the source code defining the tables of a shift reduce parser.  This
 * source code is then compiled along with custom functions to parse and reduce
 * input strings according to the rules of a grammar.
 */

#ifndef code_hpp
#define code_hpp

#include "lexer.hpp"
#include "solver.hpp"

#include <iostream>
using std::ostream;

/******************************************************************************/
class Code
{
public:
    
    /** After solving, write the source code for the scanner. */
    static void write(const Grammar& grammar,
                      const Lexer& lexer,
                      ostream& out);
    
    /** After solving, write the source code for the parse table. */
    static bool write(const Grammar& grammar,
                      const Solver& solver,
                      ostream& out);
    
    /**
     * Writes the structures and functions that implement a lexer.  The lexer
     * matches patterns by defining nodes that determine the next node based on
     * an input character.  When there is no next node for a given input
     * character, check for a matching terminal at the current node.
     */
    static void write_terms(Term* term, ostream& out);
    static void write_eval( Term* term, ostream& out);
    static void write_scan( Node* node, std::map<Node*, int>& ids, ostream& out);
    static void write_node( Node* node, std::map<Node*, int>& ids, ostream& out);
    
    /**
     * Writes the functions that calls the user defined action for a given rule.
     * These functions get values from the top of the stack and cast those
     * values to their user define classes.  After casting the values, the
     * functions call the user define action with those objects.
     */
    static bool write_reduce(const Grammar& grammar, ostream& out);
    static void write_rule_action(Nonterm::Rule* rule, ostream& out);
    static void write_call_action(Nonterm::Rule* rule, ostream& out);
    
    /**
     * Writes the rules that define which action to call when a sequence of
     * symbols is reduced to a nonterminal.  These written rules provide the
     * number of symbols in a production so that the parser knows the number of
     * values to remove from the stack after reducing the symbols to the rule's
     * nonterminal.
     */
    static void write_nonterm(Nonterm* nonterm, ostream& out);
    static void write_rules(const Grammar& grammar, ostream& out);
    
    /**
     * Writes the actions for each state.  The actions determines if the parser
     * should shift the next terminal onto its stack or reduce the stack by a
     * matched production rule.
     */
    static void write_actions(State::Actions* actions, ostream& out);
    static void write_gotos(const std::vector<std::unique_ptr<State>>& states, ostream& out);
    static void write_states(const std::vector<std::unique_ptr<State>>& states, ostream& out);
};

#endif
