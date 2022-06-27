/*******************************************************************************
 * Solve for all possible parse states of the grammar.
 */

#ifndef solver_hpp
#define solver_hpp

#include "grammar.hpp"
#include "state.hpp"

/*******************************************************************************
 * With all states known, actions such as shifting a symbol onto the stack or
 * reducing the stack to a new nonterminal, are computed.  These actions
 * determine the action taken for every state given the next symbol.
 */
class Solver
{
  public:
    Solver();
    State* start;
    
    /** After reading the grammar, solve for parse states. */
    bool solve(Grammar& grammar);

    /** Unique parse states of the grammar. */
    Unique_States states;
    
    /** Maintain a list of the states as they are found. */
    std::vector<State*> sorted;

    /** Unique actions for the individual states to reference. */
    Unique_Actions actions;
};

#endif
