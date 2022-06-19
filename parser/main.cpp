/**
 * Finite state machine for recognizing words and numbers in an input string.
 */

#include "finite.hpp"

#include <sstream>

int
main(int argc, const char* argv[])
{
    /** Finding words and numbers. */
    Term word("word", 0);
    Term number("number", 1);
    
    /** States for each terminal and the negative sign. */
    Finite start;
    Finite state_word(&word);
    Finite state_number(&number);
    Finite state_sign;
    
    /** Add outputs to construct the state machine. */
    start.add_out('a', 'z', &state_word);
    start.add_out('-', &state_sign);

    /** An empty output is added to bypass the optional negative sign. */
    start.add_epsilon(&state_sign);

    /**
     * Circle back to the previous state so that continuing to see a letter
     * while in the word state, or a digit while in the number state, keeps
     * those states in the active set.
     */
    state_word.add_out('a', 'z', &state_word);
    state_sign.add_out('0', '9', &state_number);
    state_number.add_out('0', '9', &state_number);
    
    std::stringstream example("hello test 123");
    
    /** Scan identifies terminals in the input. */
    while (true) {
        example >> std::ws;
        if (example.peek() == EOF) {
            break;
        }
        
        std::string match;
        Term* term = start.scan(example, &match);

        if (term) {
            std::cout << match << " is a " << term->name << ".\n";
        } else {
            std::cin >> match;
        }
    }

    return 0;
}
