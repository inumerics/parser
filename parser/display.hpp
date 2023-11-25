/**
 * Displays the state machine nodes of the lexer and the actions of the shift
 * reduce parser.
 */

#ifndef display_hpp
#define display_hpp

#include "lexer.hpp"
#include "solver.hpp"
#include <iostream>

/**
 * The lexer matches patterns using a state machine of nodes, where the nodes
 * have next states corresponding to input characters.  The actions determine
 * if the parser should shift the next terminal onto its stack or reduce the
 * stack by a grammar rule.
 */
class Display
{
  public:
    static void print_lexer(const Lexer& lexer,
                            std::ostream& out);
    
    static void print_parser(const Grammar& grammar,
                             const Solver& solver,
                             std::ostream& out);

    static void print_states(const Grammar& grammar,
                             const Solver& solver,
                             std::ostream& out);
    
  private:
    static void print_node(const Node* node,
                           std::map<const Node*, int>& ids,
                           std::ostream& out);
    
    static void print_actions(const Grammar& grammar,
                              const Solver& solver,
                              std::ostream& out);
    
    static void print_gotos(const Grammar& grammar,
                            const Solver& solver,
                            std::ostream& out);

    static void print_state(const State& state, 
                            std::ostream& out);
};

#endif
