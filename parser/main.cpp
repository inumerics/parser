/**
 * Finite state machine for recognizing a character sequence in an input string.
 */
#include "regex.hpp"

#include <sstream>

int
main(int argc, const char* argv[])
{
    Term match("match", 1);
    
    /** Build the finite state machine to match the sequence. */
    Regex regex;
    bool ok = regex.parse("c?(a|b)+", &match);
    if (!ok) {
        std::cerr << "Unable to parse sequence.\n";
        return 1;
    }

    std::stringstream example("aab cbb dab ccab");
    
    /** Scan identifies terminals in the input. */
    while (true) {
        example >> std::ws;
        if (example.peek() == EOF) {
            break;
        }
        
        std::string match;
        Term* term = regex.start->scan(example, &match);

        if (term) {
            std::cout << match << " is a " << term->name << ".\n";
        } else {
            std::string rest;
            example >> rest;
            std::cout << match + rest << " is not a match.\n";
        }
    }
    
    return 0;
}

