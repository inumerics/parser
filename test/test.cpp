#include "finite.hpp"
#include "literal.hpp"
#include "regex.hpp"
#include "lexer.hpp"
#include "display.hpp"

#include <sstream>
#include <fstream>

/**
 * Finding numbers and words in a string.
 */
int test_finite()
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

int test_literal()
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
        }
    }
    
    return 0;
}

int test_regex()
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

/******************************************************************************/
int test_lexer()
{
    Term num("number", 1);
    Term hex("hex", 2);
    
    Lexer lexer;
    lexer.add_regex(&num, "[0-9]+");
    lexer.add_regex(&hex, "0x([a-f]|[A-F]|[0-9])+");
    lexer.add_regex(&hex, "0h([a-f]|[A-F]|[0-9])+");
    
    
    lexer.solve();
    lexer.reduce();
    
    Display::print_lexer(lexer, std::cout);
    
    return 0;
}

int
main(int argc, char* argv[])
{
    test_finite();
    test_literal();
    test_regex();
    test_lexer();
}
