/**
 * Lexer for recognizing a character sequence in an input string.
 */
#include "lexer.hpp"
#include "display.hpp"

#include <sstream>
#include <fstream>

int
main(int argc, const char* argv[])
{
//    Term num("number", 1);
//    Term hex("hexadecimal", 2);
//
//    Lexer lexer;
//    lexer.add_regex(&num,   "[0-9]+");
//    lexer.add_regex(&hex,   "0x([A-Z]|[0-9])+");
//
//    lexer.solve();
//    lexer.reduce();
//
//    Display::print_lexer(lexer, std::cout);

    std::ifstream in;
    in.open("test.bnf");
    if (!in) {
        return 1;
    }
    
    Grammar grammar;
    grammar.read_grammar(in);
    
    Lexer lexer;
    
    for (auto& term : grammar.terms) {
        if (!term.second->regex.empty()) {
            lexer.add_regex(term.second.get(), term.second->regex);
        } else {
            lexer.add_literal(term.second.get(), term.second->name);
        }
    }
    
    lexer.solve();
    lexer.reduce();

    Solver parser;
    parser.solve(grammar);

    Display::print_parser(grammar, parser, std::cout);
    //Code::write(grammar, lexer,  std::cout);
    //Code::write(grammar, parser,  std::cout);
    
    return 0;
}

