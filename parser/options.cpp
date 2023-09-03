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
    "  -p   display only the parser table\n"
    "  -s   display only the parser states\n"
    "\n"
    "  -v   display version and license\n"
    "\n"
    "DESCRIPTION\n"
    "  This program generates parse tables for languages that are defined by a\n"
    "  context free grammar."
    "\n";
}

void 
Options::display_license()
{
    std::cout <<
        "IslandParser version 0.8.0\n"
        "\n"
        "The software is provided \"as is\", without warranty of any kind, express or\n"
        "implied, including but not limited to the warranties of merchantability, \n"
        "fitness for a particular purpose and noninfringement. In no event shall the\n"
        "authors or copyright holders be liable for any claim, damages or other\n"
        "liability, whether in an action of contract, tort or otherwise, arising from, \n"
        "out of or in connection with the software or the use or other dealings in the\n"
        "software.\n"
        "\n"
        "Copyright(c) 2023 Island Numerics\n";
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
    case 'v': {
        show_version = true;
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
    case 's': {
        show_states = true;
        return true;
    }
    }
    return false;
}

