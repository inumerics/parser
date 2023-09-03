#include "state.hpp"

using std::vector;
using std::ostream;

/******************************************************************************/
State::State(size_t id):
id(id),
actions(nullptr){}

//void
//State::add(Item item) {
//    items.insert(item);
//}

void
State::closure()
{
    vector<Item> found;
    std::copy(items.begin(), items.end(), std::back_inserter(found));
    
    while (found.size() > 0)
    {
        Item item = found.back();
        found.pop_back();
        
        Nonterm* nonterm = item.next_nonterm();
        if (nonterm) {
            vector<Symbol*> product = item.advance().rest();
            product.push_back(item.ahead);
            
            std::set<Symbol*> terms;
            Nonterm::solve_firsts(product, &terms);
            
            for (auto& rule : nonterm->rules) {
                for (auto term : terms) {
                    Item next = Item(rule.get(), 0, term);
                    auto result = items.insert(next);
                    if (result.second) {
                        found.push_back(next);
                    }
                }
            }
        }
    }
}

/**
 * Solves for the next state for a given input symbol.  Since the goal is
 * to solve for only unique states, solve is called first and the result
 * is compared to previouly seen states.  After comparing to the previously
 * found, a pointer to a unique state is added to the current state for the
 * given symbol.
 */

std::unique_ptr<State>
State::solve_next(Symbol* symbol, size_t id)
{
    std::unique_ptr<State> state = std::make_unique<State>(id);
    for (auto item : items) {
        if (item.next() == symbol)
            state->items.insert(item.advance());
    }
    state->closure();
    if (state->items.size() == 0) {
        state.reset();
    }
    return state;
}

std::unique_ptr<State::Actions>
State::solve_actions(Item accept)
{
    auto actions = std::make_unique<Actions>();
    
    for (auto item : items) {
        Term* term = dynamic_cast<Term*>(item.next());
        if (term) {
            auto found = nexts.find(term);
            if (found != nexts.end()) {
                if (actions->shift.count(term) == 0) {
                    actions->shift[term] = found->second;
                } else if (actions->shift[term] != found->second) {
                    std::cerr << "Error: Shift action already exists.\n";
                    return nullptr;
                }
            }
        }
        else if (!item.next()) {
            if (item == accept) {
                if (actions->accept.count(item.ahead) == 0) {
                    actions->accept[item.ahead] = item.rule;
                } else if (actions->accept[item.ahead] != item.rule) {
                    std::cerr << "Error: Reduce action already exists.\n";
                    std::cerr << "  ";
                    item.rule->nonterm->print(std::cerr);
                    std::cerr << ":";
                    for (auto s : item.rule->product) {
                        std::cerr << " ";
                        s->print(std::cerr);
                    }
                    return nullptr;
                }
            } else {
                if (actions->reduce.count(item.ahead) == 0) {
                    actions->reduce[item.ahead] = item.rule;
                } else if (actions->reduce[item.ahead] != item.rule) {
                    std::cerr << "Error: Reduce action already exists.\n";
                    
                    auto current = actions->reduce[item.ahead];
                    
                    std::cerr << "  ";
                    current->nonterm->print(std::cerr);
                    std::cerr << ":";
                    for (auto s : current->product) {
                        std::cerr << " ";
                        s->print(std::cerr);
                    }
                    
                    
                    
                    std::cerr << "  ";
                    item.rule->nonterm->print(std::cerr);
                    std::cerr << ":";
                    for (auto s : item.rule->product) {
                        std::cerr << " ";
                        s->print(std::cerr);
                    }
                    
                    return nullptr;
                }
            }
        }
    }
    
    actions->combine_reduce();
    return actions;
}

void
State::solve_gotos()
{
    for (auto next : nexts) {
        Nonterm* nonterm = dynamic_cast<Nonterm*>(next.first);
        if (nonterm) {
            gotos[nonterm] = next.second;
        }
    }
}

/******************************************************************************/
void
State::print(ostream& out) const {
    out << "State " << id;
}

void
State::print_items(ostream& out) const
{
    for (auto& item : items) {
        item.print(out);
        out << "\n";
    }
    for (auto next : nexts) {
        next.first->print(out);
        out <<  " -> " << next.second->id << "\n";
    }
}

/******************************************************************************/
Item::Item(Nonterm::Rule* rule, size_t mark, Symbol* ahead):
rule    (rule),
mark    (mark),
ahead   (ahead){}

Item
Item::advance() {
    if (mark < rule->product.size()) {
        return Item(rule, mark + 1, ahead);
    } else {
        return Item(rule, mark, ahead);
    }
}

vector<Symbol*>
Item::rest() {
    vector<Symbol*> result;
    result.insert(result.end(),
                  rule->product.begin() + mark,
                  rule->product.end());
    return result;
}

Symbol*
Item::next() {
    if (mark < rule->product.size()) {
        return rule->product[mark];
    } else {
        return nullptr;
    }
}

Nonterm*
Item::next_nonterm() {
    if (mark < rule->product.size()) {
        return dynamic_cast<Nonterm*>(rule->product[mark]);
    } else {
        return nullptr;
    }
}

bool
Item::operator==(const Item& other) const {
    if (rule != other.rule) {
        return false;
    } else if (ahead != other.ahead) {
        return false;
    } else {
        return mark == other.mark;
    }
}

bool
Item::operator<(const Item& other) const {
    if (rule != other.rule) {
        return rule < other.rule;
    } else if (ahead != other.ahead) {
        return ahead < other.ahead;
    } else {
        return mark < other.mark;
    }
}

void
Item::print(ostream& out) const
{
    rule->nonterm->print(out);
    out << ": ";
    
    bool first = true;
    for (size_t i = 0; i < rule->product.size(); i++) {
        if (i == mark) {
            if (!first) {
                out << " ";
            } else {
                first = false;
            }
            out << ".";
        }
        if (!first) {
            out << " ";
        } else {
            first = false;
        }
        rule->product[i]->print(out);
    }
    
    if (mark == rule->product.size()) {
        if (!first) {
            out << " ";
        } else {
            first = false;
        }
        out << ".";
    }
    
    out << " , ";
    ahead->print(out);
}

/******************************************************************************/
bool
State::Actions::is_same(const std::unique_ptr<Actions>& other) const
{
    if (shift != other->shift)
        return false;
    if (accept != other->accept)
        return false;
    if (reduce != other->reduce)
        return false;
    if (any != other->any)
        return false;
    return true;
}

void
State::Actions::combine_reduce()
{
    int max_count = 0;
    Nonterm::Rule* max_rule = nullptr;
    std::map<Nonterm::Rule*, int> count;
    for (auto r : reduce) {
        auto found = count.find(r.second);
        if (found != count.end()) {
            found->second += 1;
            if (max_count < found->second) {
                max_count = 1;
                max_rule  = found->first;
            }
        } else {
            count[r.second] = 1;
            if (max_count < 1) {
                max_count = 1;
                max_rule  = r.second;
            }
        }
    }
    if (max_count > 0) { 
        std::map<const Symbol*, Nonterm::Rule*> update;
        for (auto r : reduce) {
            if (r.second != max_rule) {
                update[r.first] = r.second;
            }
        }
        reduce = update;
        any = max_rule;
    }
}

