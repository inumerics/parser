/*******************************************************************************
 * Solves for the parse table of a grammar.
 */

#include "grammar.hpp"
#include "display.hpp"
#include "code.hpp"
#include "options.hpp"

#include <iostream>
#include <fstream>
#include <cstring>

/*******************************************************************************
 * Reads the user defined grammar from the standard input or file and then
 * solves for all of the parse states.  After solving for all possible states,
 * the program writes the source code for the table.
 */
int
main(int argc, char* argv[])
{
    Options opt;
    opt.parse(argc, argv);
    
    if (opt.show_help) {
        opt.display_help();
        return 0;
    } else if (opt.show_version) {
        opt.display_license();
        return 0;
    }

    std::istream* in  = &std::cin;
    std::ostream* out = &std::cout;
    
    std::fstream file_in;
    if (!opt.inpath.empty()) {
        file_in.open(opt.inpath);
        if (file_in) {
            in = &file_in;
        } else {
            std::cerr << "Unable to open input file.\n";
            return 1;
        }
    }
    
    std::ofstream file_out;
    if (!opt.outpath.empty()) {
        file_out.open(opt.outpath);
        if (file_out) {
            out = &file_out;
        } else {
            std::cerr << "Unable to open output file.\n";
            return 1;
        }
    }
    
    Grammar grammar;
    
    bool ok = grammar.read_grammar(*in);
    if (!ok) {
        std::cerr << "Unable to read grammar.\n";
        return 1;
    }
    
    Lexer lexer;
    
    for (auto& term : grammar.terms) {
        if (!term->regex.empty()) {
            lexer.add_regex(term.get(), term->regex);
        } else {
            lexer.add_literal(term.get(), term->name);
        }
    }
    
    lexer.solve();
    lexer.reduce();
    
    Solver parser;
    
    ok = parser.solve(grammar);
    if (!ok) {
        std::cerr << "Unable to solve states of the grammar.\n";
        return 1;
    }
    
    if (opt.show_lexer) {
        Display::print_lexer(lexer, *out);
    } else if (opt.show_parser) {
        Display::print_parser(grammar, parser, std::cout);
    } else if (opt.show_states) {
        Display::print_states(grammar, parser, std::cout);
    } else {
        Code::write(grammar, lexer, *out);
        ok = Code::write(grammar, parser, *out);
        if (!ok) {
            std::cerr << "Error while writing the parse tables.\n";
            return 1;
        }
    }
    
    return 0;
}

