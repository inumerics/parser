#include "display.hpp"

#include <iomanip>
#include <climits>

/******************************************************************************/
void
Display::print_lexer(const Lexer& lexer, std::ostream& out)
{
    int id = 0;
    std::map<const Node*, int> ids;
    for (auto& state : lexer.nodes) {
        ids[state.get()] = id++;
    }
    
    for (auto& state : lexer.nodes) {
        print_node(state.get(), ids, out);
    }
}

static void
print_range(const Node::Range& range, std::ostream& out)
{
    // TODO Fix for unicode.
    char a = range.first;
    char b = range.last;
    
    if (a == b) {
        if (isprint(a) && a != '\'') {
            out << (char)a << "    ";
        } else {
            out << a;
        }
    } else {
        if (isprint(a) && isprint(b)
            && a != '\'' && b != '\'') {
            out << (char)a << " - " << (char)b;
        } else {
            out << a << " - " << b;
        }
    }
}

void
Display::print_node(const Node* node, std::map<const Node*, int>& ids,
                    std::ostream& out)
{
    if (node->nexts.size() == 0) {
        return;
    }
    
    out << ids[node] << ":";
    if (node->accept) {
        out << " " << node->accept->name;
    }
    out << "\n";
    for (auto& next : node->nexts) {
        out << "  ";
        print_range(next.first, out);
        out << "  =>  " << ids[next.second];
        if (next.second->nexts.size() == 0) {
            if (next.second->accept) {
                out << ": " << next.second->accept->name;
            }
        }
        out << "\n";
    }
    out << "\n";
}

/******************************************************************************/
void
Display::print_parser(const Grammar& grammar,
                      const Solver& solver,
                      std::ostream& out)
{
    print_actions(grammar, solver, out);
    print_gotos(grammar, solver, out);
    
    for (auto& n : grammar.nonterms) {
        for (auto& rule : n->rules) {
            out << "r" << rule->id << " ";
            rule->print(out);
            out << "\n";
        }
    }
}

/** Get the longest symbol name to align to the columns. */
int
longest_symbol_name(const Grammar& grammar)
{
    size_t result = 0;
    for (auto& term : grammar.terms) {
        if (result < term->name.length()) {
            result = term->name.length();
        }
    }
    
    for (auto& nonterm : grammar.nonterms) {
        if (result < nonterm->name.length()) {
            result = nonterm->name.length();
        }
    }
    
    return (result < INT_MAX) ? (int)result : INT_MAX;
}

static std::string
action_for_symbol(const State::Actions* actions, const Symbol* symbol)
{
    std::stringstream s;
    
    auto found = actions->shift.find(symbol);
    if (found != actions->shift.end()) {
        s << "s" << found->second->id;
        return s.str();
    }
    auto reduce = actions->reduce.find(symbol);
    if (reduce != actions->reduce.end()) {
        s << "r" << reduce->second->id;
        return s.str();
    }
    auto accept = actions->accept.find(symbol);
    if (accept != actions->accept.end()) {
        s << "a" << accept->second->id;
        return s.str();
    }
    if (actions->any) {
        s << "\"";
        return s.str();
    }
    return "";
}

static std::string
action_for_any_symbol(const State::Actions* actions)
{
    std::stringstream s;
    if (actions->any) {
        s << "r" << actions->any->id;
        return s.str();
    }
    return "";
}

void
Display::print_actions(const Grammar& grammar,
                       const Solver& solver,
                       std::ostream& out)
{
    int len = longest_symbol_name(grammar);
    
    out << std::setw(len) << std::left;
    out << "";
    for (auto& state : solver.states) {
        out << std::setw(4) << std::right;
        out << state->id;
    }
    out << "\n";
    
    for (auto& term : grammar.terms) {
        out << std::setw(len) << std::left;
        out << term->name;
        
        for (auto& state : solver.states) {
            out << std::setw(4) << std::right;
            out << action_for_symbol(state->actions, term.get());
        }
        out << "\n";
    }
    
    Endmark endmark;
    
    out << std::setw(len) << std::left;
    out << "$";
    for (auto& state : solver.states) {
        out << std::setw(4) << std::right;
        out << action_for_symbol(state->actions, &endmark);
    }
    out << "\n";
    
    out << std::setw(len) << std::left;
    out << "any";
    for (auto& state : solver.states) {
        out << std::setw(4) << std::right;
        out << action_for_any_symbol(state->actions);
    }
    out << "\n";
    out << "\n";
}

void
Display::print_gotos(const Grammar& grammar,
                     const Solver& parser,
                     std::ostream& out)
{
    int len = longest_symbol_name(grammar);
    
    for (auto& nonterm : grammar.nonterms) {
        out << std::setw(len) << std::left;
        out << nonterm->name;
        
        Symbol* sym = nonterm.get();
        for (auto& state : parser.states)
        {
            out << std::setw(4) << std::right;
            auto found = state->gotos.find(sym);
            if (found != state->gotos.end()) {
                out << found->second->id;
            } else {
                out << "";
            }
        }
        out << "\n";
    }
    out << "\n";
}

