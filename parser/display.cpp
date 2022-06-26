#include "display.hpp"

#include <iomanip>
#include <climits>

/******************************************************************************/
void
Display::print_lexer(const Lexer& lexer, std::ostream& out)
{
    for (auto& state : lexer.primes) {
        print_node(*state, out);
    }
}

static void
print_range(const Node::Range& range, std::ostream& out)
{
    char first = range.first;
    char last  = range.last;
    
    if (first == last) {
        if (isprint(first) && first != '\'') {
            out << (char)first;
        } else {
            out << first;
        }
    } else {
        if (isprint(first) && isprint(last)
                && first != '\'' && last != '\'') {
            out << (char)first << " - " << (char)last;
        } else {
            out << first << " - " << last;
        }
    }
}

void
Display::print_node(const Node& node, std::ostream& out)
{
    out << "Node " << node.id;
    if (node.accept) {
        out << " '" << node.accept->name << "'";
    }
    out << "\n";
    for (auto& next : node.nexts) {
        out << "  ";
        print_range(next.first, out);
        out << " >> " << next.second->id << "\n";
    }
    out << "\n";
}

