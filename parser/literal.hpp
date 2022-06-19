/**
 * Builds a finite state machine for finding a sequence of characters in an
 * input string.
 */
#ifndef literal_hpp
#define literal_hpp

#include "finite.hpp"

/**
 * The parse method takes a character string and builds a state machine to find
 * that sequence in an input.   After parsing the pattern, call the scan method
 * of the start state to look for a match.
 */
class Literal
{
  public:
    Finite* start = nullptr;

    /**
     * Each character in the pattern must be a printable character.  Escape
     * sequences, starting with a forward slash, allow non-printable characters
     * in the matched sequence.
     */
    bool parse(const std::string& pattern, Term* accept);

  private:
    std::vector<std::unique_ptr<Finite>> states;
    Finite* add_state();
};

#endif
