/**
 * Lexer for recognizing a character sequence in an input string.
 */
#include "lexer.hpp"
#include "display.hpp"

#include <sstream>

int
main(int argc, const char* argv[])
{
    Term num("number", 1);
    Term hex("hexadecimal", 2);
    
    Lexer lexer;
    lexer.add_regex(&num,   "[0-9]+");
    lexer.add_regex(&hex,   "0x([A-Z]|[0-9])+");
    
    lexer.solve();
    lexer.reduce();
    
    Display::print_lexer(lexer, std::cout);

    return 0;
}

