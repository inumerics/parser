#include "regex.hpp"

#include <sstream>

/**
 * Builds the state machine according to the regular expression.  The final
 * unconnected outputs are connected to a new state that contains a pointer to
 * the user defined terminal representing this expression.
 */
bool
Regex::parse(const std::string& in, Term* accept)
{
    states.clear();
    std::istringstream input(in);
    std::vector<Finite::Out*> outs;

    start = parse_expr(input, &outs);

    if (start) {
        Finite* target = add_state(accept);
        for (Finite::Out* out : outs) {
            out->next = target;
        }
        return true;
    }
    else {
        std::cerr << "Unable to parse expression '" << in << "'.\n";
        return false;
    }
}

/**
 * The vertical bar, |, in a regular expression defines a machine that matches
 * either one of patterns on the two sides of the bar.  As an example, the
 * expression ab|cd will look for the string ab or cd.
 */
Finite*
Regex::parse_expr(std::istream& in, std::vector<Finite::Out*>* outs)
{
    Finite* expr = add_state();

    while (in.peek() != EOF)
    {
        Finite* term = parse_term(in, outs);
        if (!term) {
            return nullptr;
        }

        expr->add_epsilon(term);
        if (in.peek() == '|') {
            in.get();
        } else {
            break;
        }
    }

    return expr;
}

/**
 * Connects multiple states to match a series of letters.  The following method
 * tracks the outputs from the previous state.  Previously unconnected outputs
 * are then connected to the state built for the next letter in the expression.
 */
Finite*
Regex::parse_term(std::istream& in, std::vector<Finite::Out*>* outs)
{
    std::vector<Finite::Out*> fact_in;
    std::vector<Finite::Out*> fact_out;
    Finite* term = parse_fact(in, &fact_out);
    if (!term) {
        return nullptr;
    }

    while (true)
    {
        int c = in.peek();
        if (c == EOF || c == ')' || c == '|') {
            break;
        }

        fact_in = fact_out;
        fact_out.clear();

        Finite* fact = parse_fact(in, &fact_out);
        if (!fact) {
            return nullptr;
        }

        for (Finite::Out* input : fact_in) {
            input->next = fact;
        }
    }

    outs->insert(outs->end(), fact_out.begin(), fact_out.end());
    return term;
}

/**
 * The regular expression syntax provides special characters for defining
 * patterns, such as repeated letters or numbers.  The + operator indicates that
 * the match contains one or more of the previous character.  In this case a+
 * would match the strings, a, aa and aaaa among others.  While the * operator
 * defines a machine to match zero or more of the previous letter.  Finally, ?
 * marks that the previous character is optional.  As an example, ab?c will
 * match the strings abc or ac.
 */
Finite*
Regex::parse_fact(std::istream& in, std::vector<Finite::Out*>* outs)
{
    std::vector<Finite::Out*> atom_outs;
    Finite* atom = parse_atom(in, &atom_outs);
    if (!atom) {
        return nullptr;
    }

    int c = in.peek();
    if (c != '+' && c != '*' && c != '?') {
        outs->insert(outs->end(), atom_outs.begin(), atom_outs.end());
        return atom;
    }

    Finite* state = add_state();
    state->add_epsilon(atom);
    Finite::Out* out2 = state->add_epsilon(nullptr);

    outs->push_back(out2);

    switch (in.get()) {
        case '+': {
            for (Finite::Out* out : atom_outs) {
                out->next = state;
            }
            return atom;
        }
        case '*': {
            for (Finite::Out* out : atom_outs) {
                out->next = state;
            }
            return state;
        }
        case '?': {
            outs->insert(outs->end(), atom_outs.begin(), atom_outs.end());
            return state;
        }
        default: {
            return nullptr;
        }
    }
}

/**
 * Typically, this method returns a single state to match the smallest unit of
 * a regular expression: a single character. The addition of parenthesis is the
 * last operator needed to fully utilize the syntax of regular expressions.
 * Parenthesis allows the grouping of an expression to control the order of
 * operations. In this case the pattern ab|c matches ab or c, while a(b|c)
 * matches ab or ac. In another example, ab+ matches ab and abbb, while (ab)+
 * defines ab or ababab.
 */
Finite*
Regex::parse_atom(std::istream& in, std::vector<Finite::Out*>* outs)
{
    int c = in.get();
    if (c == EOF) {
        std::cerr << "Unexpected end of file.\n";
        return nullptr;
    }

    if (c == '(') {
        Finite* expr = parse_expr(in, outs);
        if (!expr) {
            return nullptr;
        }
        if (in.get() != ')') {
            std::cerr << "Expected ')' to end expression.\n";
            return nullptr;
        }
        return expr;
    }
    else if (c == ']' || c == ')' || c == '|') {
        std::cerr << "Unexpected '" << (char)c << "' in expression.\n";
        return nullptr;
    }
    else if (isprint(c)) {
        Finite* state = add_state();
        Finite::Out* out = state->add_out(c, nullptr);
        outs->push_back(out);
        return state;
    }
    else {
        if (c == EOF) {
            std::cerr << "Unexpected end of file.\n";
        } else if (isprint(c)) {
            std::cerr << "Unexpected '" << (char)c << "' in expression.\n";
        } else {
            std::cerr << "Unexpected << c << in expression.\n";
        }
        return nullptr;
    }
}

/**
 * Builds a new state and retains ownership.  No memory leaks occur if any
 * exceptions or errors occur during subset construction, as the states vector
 * contains all fragments of the automaton.
 */
Finite*
Regex::add_state() {
    states.emplace_back(std::make_unique<Finite>());
    return states.back().get();
}

Finite*
Regex::add_state(Term* accept) {
    states.emplace_back(std::make_unique<Finite>(accept));
    return states.back().get();
}
