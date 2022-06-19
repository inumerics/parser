/**
 * Finite state machine for recognizing a character sequence in an input string.
 */
#include "finite.hpp"
#include "literal.hpp"

#include <sstream>

int
main(int argc, const char* argv[])
{
    Term match("match", 1);
    
    /** Build the finite state machine to match the sequence. */
    Literal literal;
    bool ok = literal.parse("abc", &match);
    if (!ok) {
        std::cerr << "Unable to parse sequence.\n";
        return 1;
    }
    
    std::stringstream example("hello abc 123");
    
    /** Scan identifies terminals in the input. */
    while (true) {
        example >> std::ws;
        if (example.peek() == EOF) {
            break;
        }
        
        std::string match;
        Term* term = literal.start->scan(example, &match);

        if (term) {
            std::cout << match << " is a " << term->name << ".\n";
        } else {
            example >> match;
            std::cout << match << " is not a match.\n";
        }
    }
    
    return 0;
}

