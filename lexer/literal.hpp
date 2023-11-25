/**
 * Builds a finite state machine for finding a sequence of characters in an
 * input string.
 */
#ifndef literal_hpp
#define literal_hpp

#include "finite.hpp"

/**
 * The parse method takes a character string and builds a state machine to find
 * that sequence in an input.  Only printable characters are used to define the
 * sequence.  Escape sequences, starting with a forward slash, allow
 * non-printable characters in the matched sequence.
 */
class Literal
{
  public:
    /** Builds the NFA to match the sequence. */
    bool parse(const std::string& in, Term* accept);
    
    /** After parsing, call scan of the start state to look for a match. */
    Finite* start = nullptr;
    
  private:
    std::vector<std::unique_ptr<Finite>> states;
};

#endif
