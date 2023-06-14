#include "finite.hpp"

#include <algorithm>

/**
 * Terminals of the grammar.  The rank is required when a strings matches
 * multiple terminals and the one with the lowest rank is accepted.
 */
Term::Term(const std::string& name, size_t rank):
name    (name),
rank    (rank){}

void
Term::print(std::ostream& out) const {
    out << "'" << name << "'";
}

void
Term::write(std::ostream& out) const {
    out << "term" << rank;
}

/**
 * The state machine is constructed by connecting states to each other with
 * outputs.  Each state contains a vector of outputs that defines the next
 * active states for a given input character.  While operating the machine, one
 * or more states is in a set of active states.
 */
Finite::Finite():
term(nullptr){}

Finite::Finite(Term* accept):
term(accept){}

/**
 * Reads from an input stream, following the outputs base on each character,
 * until no new states are found.  Returns the lowest ranked term term of the
 * final set of states.
 */
Term*
Finite::scan(std::istream& in, std::string* match)
{
    /** Current states and the new ones found based on the input character. */
    std::set<Finite*> current;
    std::set<Finite*> found;
    
    current.insert(this);
    closure(&current);
    
    bool done = false;
    
    /** Follow the outputs to new states for each input character. */
    while (!done) {
        int c = in.peek();
        
        for (Finite* state : current) {
            state->find_next(c, &found);
        }
        closure(&found);
        
        if (found.size() > 0) {
            match->push_back(in.get());
            current = std::move(found);
            found.clear();
        } else {
            done = true;
        }
    }
    
    /** Return the terminal with the lowest rank. */
    auto lowest = std::min_element(current.begin(), current.end(), lower_rank);
    if (lowest != current.end()) {
        return (*lowest)->term;
    } else {
        return nullptr;
    }
}

/**
 * Find all of the new active states based on the current set and the next
 * character of the input string.
 */
void
Finite::find_next(int c, std::set<Finite*>* next) const
{
    for (auto& out : outs) {
        if (out->in_range(c)) {
            if (out->next) {
                next->insert(out->next);
            }
        }
    }
}

void
Finite::find_next(int c, std::set<Finite*>* next, int* last) const
{
    for (auto& out : outs)
    {
        if (out->in_range(c)) {
            if (out->next)
                next->insert(out->next);
            if (*last > out->last)
                *last = out->last;
        }
        else if (!out->is_epsilon()) {
            if (out->first > c && out->first <= *last) {
                *last = out->first - 1;
            }
        }
    }
}

/**
 * Closure methods follow the empty transitions of a state to complete the new
 * active set after reading a character.
 */
void
Finite::closure(std::set<Finite*>* states)
{
    std::vector<Finite*> stack;
    stack.insert(stack.end(), states->begin(), states->end());
    
    while (stack.size() > 0) {
        Finite* check = stack.back();
        stack.pop_back();
        check->closure(states, &stack);
    }
}

/**
 * Closure methods follow the empty transitions of a state to complete the new
 * active set after reading a character.  Newly found states are added to a
 * stack to check later.
 */
void
Finite::closure(std::set<Finite*>* states, std::vector<Finite*>* stack) const
{
    for (auto& out: outs) {
        if (out->is_epsilon()) {
            Finite* next = out->next;
            if (next) {
                auto found = states->insert(next);
                if (found.second) {
                    stack->push_back(next);
                }
            }
        }
    }
}

/**
 * Each terminal is given a unique rank to determine which pattern will be
 * the accepted match in cases with multiple final states.
 */
bool
Finite::lower_rank(const Finite* left, const Finite* right)
{
    if (left->term && right->term) {
        return left->term->rank < right->term->rank;
    } else if (left->term) {
        return true;
    } else {
        return false;
    }
}

Finite::Out::Out(int first, int last, Finite* next):
next    (next),
epsilon (false),
first   (first),
last    (last){}

Finite::Out::Out(Finite* next):
next    (next),
epsilon (true),
first   ('\0'),
last    ('\0'){}

bool
Finite::Out::is_epsilon() {
    return epsilon;
}

bool
Finite::Out::in_range(int c) {
    if (epsilon) {
        return false;
    } else {
        return (c >= first && c <= last);
    }
}

/** Builds and returns new outputs, but retains ownership. */
Finite::Out*
Finite::add_out(int c, Finite* next) {
    outs.emplace_back(std::make_unique<Out>(c, c, next));
    return outs.back().get();
}

Finite::Out*
Finite::add_out(int first, int last, Finite* next) {
    outs.emplace_back(std::make_unique<Out>(first, last, next));
    return outs.back().get();
}

Finite::Out*
Finite::add_epsilon(Finite* next) {
    outs.emplace_back(std::make_unique<Out>(next));
    return outs.back().get();
}

