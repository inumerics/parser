/*******************************************************************************
 * Solves for the parse table of a grammar.
 */

#include "grammar.hpp"
#include "display.hpp"
#include "code.hpp"

#include <iostream>
#include <fstream>
#include <cstring>

/*******************************************************************************
 * Parses and stores the command line options.
 */
class Options {
  public:
    Options();
    bool parse(int argc, char *argv[]);
    void display_help();
    
    std::string inpath;
    std::string outpath;
    bool show_help;
    bool show_lexer;
    bool show_parser;
    
  private:
    bool parse_option(char c, int argc, char *argv[], int* idx);
};

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
        if (!term.second->regex.empty()) {
            lexer.add_regex(term.second.get(), term.second->regex);
        } else {
            lexer.add_literal(term.second.get(), term.second->name);
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

/******************************************************************************/
Options::Options():
    show_help(false),
    show_lexer(false),
    show_parser(false){}

/******************************************************************************/
void
Options::display_help()
{
    std::cout <<
    "Usage: parser [options] [file]\n"
    "  -f   specify input filename\n"
    "  -o   specify output filename\n"
    "\n"
    "  -l   display only the lexer states\n"
    "  -p   display only the parse table\n"
    "\n"
    "DESCRIPTION\n"
    "  This program generates parse tables for languages that are defined by a\n"
    "  context free grammar."
    "\n";
}

bool
Options::parse(int argc, char *argv[])
{
    int idx = 1;
    while (idx < argc) {
        if (strlen(argv[idx]) == 2 && argv[idx][0] == '-') {
            char c = argv[idx++][1];
            parse_option(c, argc, argv, &idx);
        }
        else if ((idx + 1) == argc) {
            inpath = argv[idx++];
        }
        else {
            return false;
        }
    }
    return true;
}

bool
Options::parse_option(char c, int argc, char *argv[], int* idx)
{
    switch (c) {
        case 'o': {
            if (*idx < argc) {
                outpath = argv[(*idx)++];
                return true;
            }
            break;
        }
        case 'h': {
            show_help = true;
            return true;
        }
        case 'l': {
            show_lexer = true;
            return true;
        }
        case 'p': {
            show_parser = true;
            return true;
        }
    }
    return false;
}

