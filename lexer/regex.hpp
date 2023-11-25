/**
 * Converts regular expressions into non-deterministic finite automata (NFA) for
 * matching patterns in strings.
 */
#ifndef regex_hpp
#define regex_hpp

#include "finite.hpp"

/**
 * Matches complex patterns defined with the regular expression syntax.  The
 * class implements the Thompson's construction algorithm to construct the
 * finite state machines.
 */
class Regex
{
  public:
    /** Builds the NFA to match the pattern. */
    bool parse(const std::string& in, Term* accept);
    
    /** After parsing, call scan of the start state to look for a match. */
    Finite* start = nullptr;
    
  private:
    std::vector<std::unique_ptr<Finite>> states;
    Finite* add_state();
    Finite* add_state(Term* accept);
    
    /**
     * Recursively reads the user defined expression and return subsets of
     * connected states for each part of the expression.  Each method takes the
     * input expression stream and returns the first state of the set that
     * matches the next part of the expression.
     *
     * These methods also return a vector all of the unconnected outputs from
     * the newly created states.  These outputs are connected later as the
     * recursive decent parser returns and assembles the complete finite
     * state machine.
     */
    Finite* parse_expr(std::istream& in, std::vector<Finite::Out*>* outs);
    Finite* parse_term(std::istream& in, std::vector<Finite::Out*>* outs);
    Finite* parse_fact(std::istream& in, std::vector<Finite::Out*>* outs);
    Finite* parse_atom(std::istream& in, std::vector<Finite::Out*>* outs);
    
    /** Additional method to find characters in a range. */
    Finite* parse_atom_range(std::istream& in, std::vector<Finite::Out*>* outs);
    
    /** Returns the next printable character or escape sequence. */
    int parse_char(std::istream& in);
    int parse_escape(std::istream& in);
    int parse_unicode(std::istream& in);
};

#endif
