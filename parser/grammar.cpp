#include "grammar.hpp"

#include <algorithm>
#include <iostream>

using std::string;
using std::vector;
using std::istream;
using std::make_unique;

/**
 * Recursive descent parser for reading in a BNF grammar from a file.
 */
bool
Grammar::read_grammar(std::istream& in)
{
    bool done = false;
    bool error = false;
    
    while (!done && !error)
    {
        space(in);
        char c = in.peek();
        
        if (c == '\'') {
            next(in);
            if (!read_term(in)) {
                error = true;
            }
        } else if (isalpha(c) || c == '_') {
            if (!read_nonterm(in)) {
                error = true;
            }
        } else if (c == '#') {
            if (!read_include(in)) {
                error = true;
            }
        } else if (c == EOF) {
            done = true;
        } else {
            std::cerr << "Unexpected '" << c << "' to start definition.\n";
            error = true;
        }
    }
    
    if (error) {
        std::cerr << "Line " << line_number << ": ";
        std::cerr << line << "\n";
        return false;
    }
    
    /** Check that all nonterms have at least one production rule. */
    for (auto& nonterm : nonterms) {
        if (nonterm->rules.size() == 0) {
            std::cerr << "Error: ";
            std::cerr << nonterm->name << " does not have a rule.\n";
            return false;
        }
    }
    
    // TODO Remove this numbering.
    size_t id = 0;
    size_t rule_id = 0;
    for (auto& nonterm : nonterms) {
        nonterm->id = id++;
        for (auto& r : nonterm->rules) {
            r->id = rule_id++;
        }
    }
    
    return done;
}

/**
 * Prints back the grammar read in from an input file.
 */
void
Grammar::print(std::ostream& out) const
{
    for (auto& nonterm : nonterms) {
        nonterm->print_rules(out);
        out << std::endl;
    }
    out << std::endl;
    
    out << "Firsts:\n";
    for (auto& nonterm : nonterms) {
        nonterm->print_firsts(out);
        out << std::endl;
    }
    out << std::endl;
    
    out << "Follows:\n";
    for (auto& nonterm : nonterms) {
        nonterm->print_follows(out);
        out << std::endl;
    }
    out << std::endl;
}

/**
 * The method for reading a terminal looks four attributes: name, associated
 * type, the regular expression that defines the terminal, and an action that
 * converts the matching string into a value of the associate type.  After
 * reading the definition, the new terminal is added to the set in the grammar.
 */
bool
Grammar::read_term(istream& in)
{
    std::string name;
    if (!read_chars(in, &name)) {
        std::cerr << "Invalid terminal name '" << name << "'.\n";
        return false;
    }
    
    auto term = make_unique<Term>(name, term_names.size());
    
    if (check(in, '<')) {
        if (!read_ident(in, &term->type)) {
            std::cerr << "Invalid terminal type '" << term->type << "'.\n";
            return false;
        }
        if (!find(in, '>'))
            return false;
    }
    
    space(in);
    if (in.peek() != '&' && in.peek() != ';') {
        read_regex(in, &term->regex);
    }
    
    if (check(in, '&')) {
        if (!read_ident(in, &term->action)) {
            std::cerr << "Invalid terminal action '" << term->action << "'.\n";
            return false;
        }
    }
    if (!find(in, ';')) {
        return false;
    }
    
    if (term_names.count(name) == 0) {
        term_names[name] = term.get();
        terms.push_back(std::move(term));
    }
    else {
        Term* prev = term_names[name];
        
        if (prev->action.empty()) {
            prev->action = term->action;
        } else if (!term->action.empty()) {
            if (prev->action.compare(term->action) != 0) {
                std::cerr << "Conflicting terminal action '" << term->action << "'.\n";
                return false;
            }
        }
        
        if (prev->regex.empty()) {
            prev->regex = term->regex;
        } else if (!term->regex.empty()) {
            prev->regex = "(" + prev->regex + ")|(" + term->regex + ")";
        }
    }
    
    return true;
}

/**
 * The first part of the definition of a nonterminal is the same as a terminal:
 * a name and an associated type.  However, when defining the grammar, multiple
 * production rules are possible for each nonterminal.  So another method is
 * needed to read each of the production rules, which are separated by a
 * vertical bar.  The method keeps reading the production rules until a
 * semicolon is found which indicates the end of the rules for a nonterminal.
 */
bool
Grammar::read_nonterm(istream& in)
{
    string name;
    if (!read_ident(in, &name)) {
        return false;
    }
    
    if (nonterm_names.count(name) == 0) {
        nonterms.emplace_back(make_unique<Nonterm>(name));
        nonterm_names[name] = nonterms.back().get();
    }
    Nonterm* nonterm = nonterm_names[name];
    
    if (check(in, '<')) {
        if (!read_ident(in, &nonterm->type)) {
            return false;
        }
        if (!find(in, '>'))
            return false;
    }
    
    if (!find(in, ':'))
        return false;
    
    while (true)
    {
        auto rule = std::make_unique<Nonterm::Rule>(nonterm);
        
        if (!read_rule(in, &rule->product)) {
            return false;
        }
        if (check(in, '&')) {
            if (!read_ident(in, &rule->action)) {
                return false;
            }
        }
        
        nonterm->rules.push_back(std::move(rule));
        
        if (!check(in, '|'))
            break;
    }
    
    if (!find(in, ';'))
        return false;
    
    return true;
}

/**
 * To read a production rule the method keeps adding terminals, indicated by a
 * leading quote, and nonterminals to a vector.  The method keeps reading the
 * symbols rules until a semicolon or vertical bar indicates the end of the
 * rule.
 */
bool
Grammar::read_rule(istream& in, vector<Symbol*>* syms)
{
    while (true)
    {
        space(in);
        int c = in.peek();
        
        if (c == '\'') {
            next(in);
            std::string name;
            if (!read_chars(in, &name)) {
                return false;
            }
            
            if (term_names.count(name) == 0) {
                auto sym = make_unique<Term>(name, terms.size());
                term_names[name] = sym.get();
                terms.push_back(std::move(sym));
            }
            
            syms->push_back(term_names[name]);
        }
        else if (isalpha(c) || c == '_') {
            std::string name;
            if (!read_ident(in, &name)) {
                return false;
            }
            
            if (nonterm_names.count(name) == 0) {
                auto sym = make_unique<Nonterm>(name);
                nonterm_names[name] = sym.get();
                nonterms.push_back(std::move(sym));
            }
            
            syms->push_back(nonterm_names[name]);
        }
        else if (c == '&' || c == '|' || c == ';') {
            return true;
        }
        else {
            std::cerr << "Unexpected '" << c << "' in rule.\n";
            return false;
        }
    }
}

/******************************************************************************/
bool
Grammar::read_chars(std::istream& in, std::string* name)
{
    int c = in.peek();
    if (isprint(c) && c != ' ') {
        name->push_back(next(in));
    } else {
        std::cerr << "Unexpected '" << c << "' to start terminal.\n";
        return false;
    }
    
    while (true) {
        int c = in.peek();
        
        if (c == '\'') {
            next(in);
            return true;
        } else if (isprint(c) && c != ' ') {
            name->push_back(next(in));
        } else {
            std::cerr << "Unexpected '" << c << "' in terminal.\n";
            return false;
        }
    }
}

bool
Grammar::read_ident(std::istream& in, std::string* name)
{
    int c = in.peek();
    if (isalpha(c) || c == '_') {
        name->push_back(next(in));
    } else {
        std::cerr << "Unexpected '" << c << "' to start identifier.\n";
        return false;
    }
    
    while (true) {
        int c = in.peek();
        
        if (isalnum(c) || c == '_') {
            name->push_back(next(in));
        } else {
            return true;
        }
    }
}

bool
Grammar::read_regex(std::istream& in, std::string* pattern)
{
    int c = in.peek();
    if (isprint(c) && c != ' ') {
        pattern->push_back(next(in));
    } else {
        std::cerr << "Unexpected '" << c << "' to start regex.\n";
        return false;
    }
    
    while (true) {
        int c = in.peek();
        
        if (isprint(c) && c != ' ') {
            pattern->push_back(next(in));
        } else {
            return true;
        }
    }
}

bool
Grammar::read_include(istream& in)
{
    // TODO Change to read quote.
    std::string text;
    
    while (in.peek() != EOF) {
        int c = in.peek();
        
        if (c == '\n') {
            // TODO Move to after the loop.
            in.get();
            line.clear();
            line_number++;
            break;
        } else if (isprint(c)) {
            text.push_back(next(in));
        } else {
            return false;
        }
    }
    
    includes.push_back(text);
    return true;
}

/******************************************************************************/
char
Grammar::next(std::istream& in)
{
    char c = in.get();
    line.push_back(c);
    return c;
}

void
Grammar::space(std::istream& in)
{
    while (true)
    {
        char c = in.peek();
        
        if (c == '/') {
            c = in.get();
            if (in.peek() == '*') {
                in.get();
                line.push_back('/');
                line.push_back('*');
                multiline(in);
            }
            else if (in.peek() == '/') {
                in.get();
                line.push_back('/');
                line.push_back('/');
                singleline(in);
            }
            else {
                in.putback(c);
                break;
            }
        }
        else if (c == '\n') {
            in.get();
            line.clear();
            line_number++;
        }
        else if (isspace(c)) {
            in.get();
            line.push_back(c);
        }
        else {
            break;
        }
    }
}

/******************************************************************************/
void
Grammar::multiline(std::istream& in)
{
    bool star = false;
    bool matched = false;
    
    while (!matched && !in.eof()) {
        int c = next(in);
        if (star) {
            if (c == '/') {
                matched = true;
            } else if (c != '*') {
                star = false;
            }
        }
        else if (c == '*') {
            star = true;
        }
    }
    
    if (!matched) {
        std::cerr << "Unexpected end of file in comment.\n";
    }
}

void
Grammar::singleline(std::istream& in)
{
    bool matched = false;
    while (!matched && !in.eof()) {
        if (next(in) != '\n') {
            matched = true;
        }
    }
    
    if (!matched) {
        std::cerr << "Unexpected end of file in comment.\n";
    }
}

/******************************************************************************/
bool
Grammar::check(std::istream& in, char c)
{
    space(in);
    if (in.peek() == c) {
        next(in);
        return true;
    } else {
        return false;
    }
}

bool
Grammar::find(std::istream& in, char c)
{
    space(in);
    if (in.peek() == c) {
        next(in);
        return true;
    } else {
        std::cerr << "Expected '" << c << "'.\n";
        return false;
    }
}
