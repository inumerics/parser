#include "symbols.hpp"

Nonterm::Nonterm(const std::string& name):
    name    (name),
    id      (0),
    firsts  (),
    empty_first(false){}

void Nonterm::print(std::ostream& out) const { out << name; }
void Nonterm::write(std::ostream& out) const { out << "nonterm" << id; }

/**
 * Methods called by solve for adding symbols to the set of firsts and
 * follows for each nonterminal.  To solve for the firsts and follows of the
 * grammar, the program will keep calling solve as long as a new symbols are
 * being found and inserted into the set.
 */
void
Nonterm::solve_first(bool *found)
{
    for (auto& rule : rules) {
        insert_firsts(rule.get(), found);
    }
}

void
Nonterm::solve_firsts(const std::vector<Symbol*>& symbols,
                      std::set<Symbol*>* firsts)
{
    for (Symbol* sym : symbols) {
        Nonterm* nonterm = dynamic_cast<Nonterm*>(sym);
        if (nonterm) {
            firsts->insert(nonterm->firsts.begin(), nonterm->firsts.end());
            if (!nonterm->empty_first) {
                return;
            }
        } else {
            firsts->insert(sym);
            return;
        }
    }
}

void
Nonterm::insert_firsts(Rule* rule, bool* found)
{
    for (auto sym : rule->product) {
        Nonterm* nonterm = dynamic_cast<Nonterm*>(sym);
        if (nonterm) {
            for (auto first : nonterm->firsts) {
                auto inserted = firsts.insert(first);
                if (inserted.second) {
                    *found = true;
                }
            }
            if (!nonterm->empty_first) {
                return;
            }
        } else {
            auto inserted = firsts.insert(sym);
            if (inserted.second) {
                *found = true;
            }
            return;
        }
    }
    if (!empty_first) {
        empty_first = true;
        *found = true;
    }
}

void
Nonterm::solve_follows(bool *found)
{
    for (auto& rule : rules) {
        auto sym = rule->product.begin();
        auto end = rule->product.end();
        
        for (; sym < end; sym++) {
            Nonterm* nonterm = dynamic_cast<Nonterm*>(*sym);
            if (nonterm) {
                bool empty = false;
                nonterm->insert_follows(sym + 1, end, &empty, found);
                if (empty) {
                    nonterm->insert_follows(rule->nonterm->follows, found);
                }
            }
        }
    }
}

void
Nonterm::insert_follows(const std::set<Symbol*>& syms,
                        bool* found)
{
    for (Symbol* sym : syms) {
        auto in = follows.insert(sym);
        if (in.second) {
            *found = true;
        }
    }
}

void
Nonterm::insert_follows(std::vector<Symbol*>::iterator sym,
                        std::vector<Symbol*>::iterator end,
                        bool* epsilon,
                        bool* found)
{
    for (; sym < end; sym++) {
        Nonterm* nonterm = dynamic_cast<Nonterm*>(*sym);
        if (nonterm) {
            insert_follows(nonterm->firsts, found);
            if (!nonterm->empty_first) {
                *epsilon = false;
                return;
            }
        } else {
            auto in = follows.insert(*sym);
            if (in.second) {
                *found = true;
            }
            *epsilon = false;
            return;
        }
    }
    *epsilon = true;
}

void
Nonterm::print_rules(std::ostream& out) const
{
    bool bar = false;
    for (auto& rule : rules) {
        if (!bar) {
            out << name << ": ";
            bar = true;
        } else {
            out << "  | ";
        }
        
        bool space = false;
        for (auto sym : rule->product) {
            if (space) {
                out << " ";
            } else {
                space = true;
            }
            sym->print(out);
        }
        out << "\n";
    }
}

void
Nonterm::print_firsts(std::ostream& out) const
{
    out << "  ";
    print(out);
    out << ": ";
    
    bool space = false;
    for (auto sym : firsts) {
        if (space) {
            out << " ";
        } else {
            space = true;
        }
        sym->print(out);
    }
    if (empty_first) {
        if (space) {
            out << " ";
        }
        out << "eps";
    }
}

void
Nonterm::print_follows(std::ostream& out) const
{
    out << "  ";
    print(out);
    out << ": ";
    
    bool space = false;
    for (auto sym : follows) {
        if (space) {
            out << " ";
        } else {
            space = true;
        }
        sym->print(out);
    }
}

Nonterm::Rule::Rule(Nonterm* nonterm):
    nonterm (nonterm),
    product (),
    action  (),
    id      (0){}

Nonterm::Rule::Rule(Nonterm* nonterm, const std::string& action):
    nonterm (nonterm),
    product (),
    action  (action),
    id      (0){}

void
Nonterm::Rule::print(std::ostream& out) const
{
    out << nonterm->name << " : ";
    
    bool space = false;
    for (auto sym : product) {
        if (space) {
            out << " ";
        } else {
            space = true;
        }
        sym->print(out);
    }
}

void
Nonterm::Rule::write(std::ostream& out) const {
    out << "rule" << id;
}

void
Nonterm::solve_first(std::vector<std::unique_ptr<Nonterm>>& nonterms)
{
    bool found = false;
    do {
        found = false;
        for (auto& nonterm : nonterms) {
            nonterm->solve_first(&found);
        }
    } while (found);
}

void
Nonterm::solve_follows(std::vector<std::unique_ptr<Nonterm>>& all, Symbol* endmark)
{
    if (all.size() == 0 || all.front()->rules.size() == 0) {
        return;
    }
    
    Nonterm::Rule* rule = all.front()->rules.front().get();
    rule->nonterm->follows.insert(endmark);
    
    bool found = false;
    do {
        found = false;
        for (auto& nonterm : all) {
            nonterm->solve_follows(&found);
        }
    } while (found);
}

void Endmark::print(std::ostream& out) const { out << "$"; }
void Endmark::write(std::ostream& out) const { out << "endmark"; }

