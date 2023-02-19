#include "options.hpp"

#include <iostream>
#include <fstream>
#include <cstring>

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

