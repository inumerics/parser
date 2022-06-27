#include "grammar.hpp"

#include <algorithm>
#include <iostream>

using std::string;
using std::vector;
using std::istream;
using std::make_unique;

/******************************************************************************/
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
            if (!read_term(in)) {
                error = true;
            };
        } else if (isalpha(c) || c == '_') {
            if (!read_nonterm(in)) {
                error = true;
            };
        } else if (c == '#') {
            if (!read_include(in)) {
                error = true;
            }
        } else if (c == EOF) {
            done = true;
        } else {
            std::cerr << "Unexpected '" << c << "'.\n";
            error = true;
        }
    }

    if (error) {
        std::cerr << "Line " << number << ": ";
        std::cerr << line << "\n";
        return false;
    }

    /** Check that all nonterms have at least one production rule. */
    for (auto& nonterm : nonterms) {
        auto found = std::find(all.begin(), all.end(), nonterm.second.get());
        if (found == all.end()) {
            std::cerr << "Error: ";
            std::cerr << nonterm.second->name << " does not have a rule.\n";
            return false;
        }
    }

    size_t id = 0;
    for (auto nonterm : all) {
        nonterm->id = id++;
    }
    
    return done;
}

/******************************************************************************/
void
Grammar::print(std::ostream& out) const
{
    for (auto nonterm : all) {
        nonterm->print_rules(out);
        out << std::endl;
    }
    out << std::endl;
    
    out << "Firsts:\n";
    for (auto nonterm : all) {
        nonterm->print_firsts(out);
        out << std::endl;
    }
    out << std::endl;
    
    out << "Follows:\n";
    for (auto nonterm : all) {
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
    if (!find(in, '\''))
        return false;
    
    std::string name;
    if (!read_chars(in, &name))
        return false;
    
    if (terms.count(name) == 0)
        terms[name] = make_unique<Term>(name, terms.size());
    Term* term = terms[name].get();
    
    if (check(in, '<')) {
        if (!read_ident(in, &term->type)) {
            return false;
        }
        if (!find(in, '>')) {
            return false;
        }
    }
    
    space(in);
    if (in.peek() != '&' && in.peek() != ';') {
        read_regex(in, &term->regex);
    }
    if (check(in, '&')) {
        if (!read_ident(in, &term->action))
            return false;
    }
    if (!find(in, ';'))
        return false;
    
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
        std::cerr << "Rule must start with a nonterminal.\n";
        return false;
    }
    
    if (nonterms.count(name) == 0) {
        nonterms[name] = make_unique<Nonterm>(name);
    }
    Nonterm* nonterm = nonterms[name].get();

    if (check(in, '<')) {
        if (!read_ident(in, &nonterm->type)) {
            return false;
        }
        if (!find(in, '>')) {
            return false;
        }
    }
        
    if (!find(in, ':'))
        return false;
    
    while (true)
    {
        auto rule = std::make_unique<Nonterm::Rule>(nonterm);
        
        if (!read_rule(in, &rule->product)) {
            return false;
        }
        string action;
        if (check(in, '&')) {
            if (!read_ident(in, &rule->action)) {
                return false;
            }
        }
        
        rule->id = rules.size();
        rules.push_back(rule.get());
        nonterm->rules.push_back(std::move(rule));
        
        if (!check(in, '|'))
            break;
    }
    
    if (!find(in, ';'))
        return false;
    
    all.push_back(nonterm);

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
        if (check(in, '\'')) {
            std::string name;
            if (!read_chars(in, &name))
                return false;
            if (terms.count(name) == 0) {
                terms[name] = std::make_unique<Term>(name, terms.size());
            }
            syms->push_back(terms[name].get());
        }
        else if (isalpha(in.peek()) || in.peek() == '_') {
            std::string name;
            if (!read_ident(in, &name))
                return false;
            if (nonterms.count(name) == 0) {
                nonterms[name] = make_unique<Nonterm>(name);
            }
            syms->push_back(nonterms[name].get());
        }
        else {
            return true;
        }
    }
}

/******************************************************************************/
bool
Grammar::read_chars(std::istream& in, std::string* name)
{
    // TODO Allow for escape sequences.
    char c = next(in);
    if (c == '\'') {
        return false;
    } else if (isprint(c)) {
        name->push_back(c);
    } else {
        std::cerr << "Invalid terminal name.\n";
        return false;
    }
    
    while (true) {
        char c = next(in);
        if (c == '\'') {
            return true;
        } else if (isprint(c)) {
            name->push_back(c);
        } else {
            return false;
        }
    }
}

bool
Grammar::read_ident(std::istream& in, std::string* name)
{
    char c = next(in);
    if (isalpha(c) || c == '_') {
        name->push_back(c);
    } else {
        std::cerr << "Invalid identifier name.\n";
        return false;
    }

    while (true) {
        char c = in.peek();
        if (isalnum(c) || c == '_') {
            name->push_back(next(in));
        } else {
            return true;
        }
    }
}

bool
Grammar::read_regex(std::istream& in, std::string* name)
{
    while (true) {
        char c = in.peek();
        if (isprint(c) && !isspace(c)) {
            name->push_back(next(in));
        } else {
            return true;
        }
    }
}

bool
Grammar::read_include(istream& in)
{
    std::string text;
    while (in.peek() != EOF) {
        int c = in.get();
        if (c == '\n') {
            break;
        } else if (isprint(c)) {
            text.push_back(c);
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

bool
Grammar::space(std::istream& in)
{
    while (true)
    {
        char c = in.peek();
        
        if (c == '/') {
            c = in.get();
            if (in.peek() == '*') {
                multiline(in);
            } else if (c == '/') {
                singleline(in);
            } else {
                in.putback(c);
                break;
            }
        }
        else if (c == '\n') {
            in.get();
            number++;
            line.clear();
        }
        else if (isspace(c)) {
            line.push_back(in.get());
        }
        else {
            break;
        }
    }
    
    return true;
}

bool
Grammar::multiline(std::istream& in)
{
    bool star = false;
    while (true) {
        if (in.peek() == EOF) {
            std::cerr << "Unexpected end of file in comment.\n";
            return false;
        }
        if (star) {
            int c = in.get();
            if (c == '/') {
                break;
            } else if (c != '*') {
                star = false;
            }
        } else {
            if (in.get() == '*') {
                star = true;
            }
        }
    }
    
    return true;
}

bool
Grammar::singleline(std::istream& in)
{
    while (next(in) != '\n' && !in.eof()) {
    }
    return true;
}

bool
Grammar::check(std::istream& in, char c)
{
    if (!space(in))
        return false;

    if (in.peek() == c) {
        line.push_back(next(in));
        return true;
    } else {
        return false;
    }
}

bool
Grammar::find(std::istream& in, char c)
{
    if (!space(in))
        return false;
    
    if (next(in) == c) {
        return true;
    } else {
        std::cerr << "Expected '" << c << "'.\n";
        return false;
    }
}

/******************************************************************************/
void
Grammar::solve_first()
{
    bool found = false;
    do {
        found = false;
        for (auto& nonterm : nonterms) {
            nonterm.second->solve_first(&found);
        }
    } while (found);
}

void
Grammar::solve_follows(Symbol* endmark)
{
    if (all.size() == 0 || all.front()->rules.size() == 0) {
        return;
    }
        
    Nonterm::Rule* rule = all.front()->rules.front().get();
    rule->nonterm->follows.insert(endmark);

    bool found = false;
    do {
        found = false;
        for (auto& nonterm : nonterms) {
            nonterm.second->solve_follows(&found);
        }
    } while (found);
}

/******************************************************************************/
void Grammar::Endmark::print(std::ostream& out) const { out << "$"; }
void Grammar::Endmark::write(std::ostream& out) const { out << "endmark"; }
