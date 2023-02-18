#include "literal.hpp"

#include <sstream>

/**
 * Builds the state machine by connecting a series of states, one for each
 * character in the pattern.
 */
bool
Literal::parse(const std::string& pattern, Term* accept)
{
    std::istringstream in(pattern);
    
    /** Build a single state to start */
    states.clear();
    states.emplace_back(std::make_unique<Finite>());
    start = states.back().get();
    
    Finite* state = start;
    
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
        states.emplace_back(std::make_unique<Finite>());
        Finite* next = states.back().get();
        state->add_out(c, next);
        state = next;
    }
    
    /** Set the term of the last state to indicate a match. */
    state->term = accept;
    return true;
}
