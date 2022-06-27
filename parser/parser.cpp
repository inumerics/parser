#include "parser.hpp"

/******************************************************************************/
ParserImpl::ParserImpl(Node* node, State* start, Symbol* endmark):
    lexer_start (node),
    parser_start(start),
    endmark     (endmark),
    node        (node){}

void
ParserImpl::start()
{
    this->node = lexer_start;
    this->text.clear();

    states.clear();
    symbols.clear();
    values.clear();

    states.push_back(parser_start);
    symbols.push_back(nullptr);
    values.push_back(nullptr);
};

/**
 * The calculator calls scan with the next input character to match terminals in
 * input.  If a terminal is found,  the calculator updates the stack based on
 * the action in the parse table.  Based on the action in the parse table, the
 * new symbol is either shifted onto the stack, or the stack is reduced by a
 * rule and the user defined action is called with the values on top of the
 * stack as arguments.  This process is continued until the end of the input at
 * which point the end mark symbol is used to reduce the remaining symbols still
 * of the stack into a single value.  The following function reads an input one
 * character at a time until a terminal is found.  When a symbol is found the
 * function calls another function to update the state of the calculator.
 */
Parser::Result
ParserImpl::scan(Table* table, int c)
{
    while (true)
    {
        if (node == lexer_start && isspace(c)) {
            return Ready;
        }
        else {
            Node* next = nullptr;
            if (node->next && node->next(c)) {
                next = node->next(c);
            }

            if (next) {
                text.push_back(c);
                node = next;
                return Ready;
            }
            else {
                if (!node->accept) {
                    return Unexpected_Char;
                }

                Value* value = nullptr;
                if (node->scan) {
                    value = node->scan(table, text);
                }
                if (!advance(table, node->accept, value)) {
                    return Unexpected_Symbol;
                }
                node = lexer_start;
                text.clear();
            }
        }
    }
}

Parser::Result
ParserImpl::scan_end(Table* table)
{
    if (node->accept) {
        Value* value = nullptr;
        if (node->scan) {
            value = node->scan(table, text);
        }
        if (!advance(table, node->accept, value)) {
            return Unexpected_Symbol;
        }
    } else if (node != lexer_start) {
        return Unexpected_Char;
    }

    if (!advance(table, endmark, nullptr)) {
        return Unexpected_Symbol;
    }
    return Done;
};
/**
 * After a terminal is found in the input, it is either shifted onto the top of
 * the stack or the stack is reduced by one of the predefined rules in the
 * grammar.  If the stack is reduced, the corresponding user defined function is
 * called with the values on the top of the stack as arguments.
 */
bool
ParserImpl::advance(Table* table, Symbol* sym, Value* val)
{
    while (true)
    {
        State* top = states.back();
        State* next = find_shift(top, sym);
        if (next) {
            push(next, sym, val);
            return true;
        }

        bool accept = false;
        Rule* rule = find_reduce(top, sym, &accept);

        if (rule) {
            Value* result = nullptr;
            if (rule->reduce) {
                result = rule->reduce(table, values.data() + values.size());
            } else {
                // TODO Check rule length is one.
                result = values.back();
            }
            pop(rule->length);

            top = states.back();
            State* found = find_goto(top, rule->nonterm);
            push(found, rule->nonterm, result);
        } else {
            return false;
        }

        if (accept) {
            return true;
        }
    }
};

/******************************************************************************/
void
ParserImpl::push(State* state, Symbol* sym, Value* val)
{
    states.push_back(state);
    symbols.push_back(sym);
    values.push_back(val);
}

void
ParserImpl::pop(size_t count)
{
    for (size_t i = 0; i < count; i++) {
        states.pop_back();
        symbols.pop_back();
        values.pop_back();
    }
};
/*****************************************************************************
 * Determining the current action, either shift or reduce, based on the current
 * state and next symbol from the input.
 */
State*
find_shift(State* state, Symbol* sym) {
    for (auto& s : *state->shift) {
        if (s.sym == sym) {
            return s.next;
        }
    }
    return nullptr;
}

Rule*
find_reduce(State* state, Symbol* sym, bool* accept) {
    for (auto& s : *state->reduce) {
        if (s.sym == nullptr || s.sym == sym) {
            *accept = s.accept;
            return s.next;
        }
    }
    *accept = false;
    return nullptr;
}

State*
find_goto(State* state, Symbol* sym) {
   if (!state->go)
        return nullptr;
   for (auto& g : *state->go) {
       if (g.sym == sym) {
           return g.state;
       }
   }
   return nullptr;
};


extern Node node0;
extern State state0;
extern Symbol endmark;


std::unique_ptr<Parser> Parser::build()
{
    std::unique_ptr<ParserImpl> result = std::make_unique<ParserImpl>(&node0, &state0, &endmark);
    result->start();
    return result;
};

