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
    /** After reading the grammar, solve for parse states. */
    bool solve(Grammar& grammar);
    
    /** Unique parse states of the grammar. */
    std::vector<std::unique_ptr<State>> states;
    
    /** Unique actions for the individual states to reference. */
    std::vector<std::unique_ptr<State::Actions>> actions;
};

#endif
