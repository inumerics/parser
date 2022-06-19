#include "literal.hpp"

#include <sstream>

/**
 * Builds the state machine by connecting a series of states, one for each
 * character in the pattern.
 */
bool
Literal::parse(const std::string& pattern, Term* accept)
{
    /** Build a single state to start */
    states.clear();
    start = add_state();
    Finite* state = start;
    
    std::istringstream in(pattern);
    
    /** Add a state for each character in the string */
    while (in.peek() != EOF)
    {
        int c = in.get();
        if (!isprint(c)) {
            std::cerr << "Expected a printable character.\n";
            return false;
        }
        
        /** Use escape sequences for non-printable characters */
        if (c == '\\') {
            c = in.get();
            switch (c) {
                case 'n': c = '\n'; break;
                case 'r': c = '\r'; break;
                case 't': c = '\t'; break;
                case 'a': c = '\a'; break;
                case 'b': c = '\b'; break;
                case 'e': c = '\e'; break;
                case 'f': c = '\f'; break;
                case 'v': c = '\v'; break;
                case '\\': c = '\\'; break;
                case '\'': c = '\''; break;
                case '"':  c = '"' ; break;
                case '?':  c = '?' ; break;
                default: {
                    std::cerr << "Unknown escape sequence.\n";
                    return false;
                }
            }
        }
        
        /** Each state has only a single output with its character. */
        Finite* next = add_state();
        state->add_out(c, next);
        state = next;
    }
    
    /** Set the term of the last state to indicate a match. */
    state->term = accept;
    return true;
}

/**
 * Builds a new state and retains ownership.  No memory leaks occur if any
 * exceptions or errors occur during subset construction, as the states vector
 * contains all fragments of the automaton.
 */
Finite*
Literal::add_state() {
    states.emplace_back(std::make_unique<Finite>());
    return states.back().get();
}

