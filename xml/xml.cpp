#include "xml.hpp"

#include <iostream>
#include <sstream>
#include <memory>
using std::unique_ptr;

/**
 * For terminals that specify an associated class, these functions takes the
 * matched string from the input and returns a value of that class.
 */
unique_ptr<Name>
scan_name(Table* table, const std::string& text) {
    return std::make_unique<Name>(text);
}

/**
 * Functions called when its associated rule is matched.  Every function is
 * passed arguments for each terminal in the rule that has an associated type.
 * If any function is not implemented, including with the exact argument types,
 * then link errors will occur when building the final program.
 */
unique_ptr<Document>
reduce_document(Table* table, unique_ptr<Elements>& E1) {
    auto doc = std::make_unique<Document>();
    return doc;
}

unique_ptr<Elements>
reduce_elements(Table* table, unique_ptr<Element>& E1) {
    auto elememts = std::make_unique<Elements>();
    return elememts;
}

unique_ptr<Elements>
append_elements(Table* table,
                unique_ptr<Elements>& E1,
                unique_ptr<Element>& E2) {
    return std::move(E1);
}

unique_ptr<Element>
reduce_element(Table* table,
               unique_ptr<Tag>& E1,
               unique_ptr<Contents>& E2,
               unique_ptr<Tag>& E3) {
    auto elememt = std::make_unique<Element>();
    return elememt;
}

unique_ptr<Element>
reduce_empty(Table* table,
               unique_ptr<Tag>& E1,
               unique_ptr<Tag>& E2) {
    auto elememt = std::make_unique<Element>();
    return elememt;
}

unique_ptr<Attrs>
reduce_attrs(Table* table,
               unique_ptr<Attr>& E1) {
    auto attrs = std::make_unique<Attrs>();
    return attrs;
}

unique_ptr<Attrs>
append_attrs(Table* table,
               unique_ptr<Attrs>& E1,
               unique_ptr<Attr>& E2) {
    auto attrs = std::make_unique<Attrs>();
    return attrs;
}

unique_ptr<Attr>
reduce_attr(Table* table,
               unique_ptr<Name>& E1,
               unique_ptr<Name>& E2) {
    auto attr = std::make_unique<Attr>();
    return attr;
}


unique_ptr<Contents>
reduce_contents(Table* table, unique_ptr<Content>& E1) {
    auto content = std::make_unique<Contents>();
    return content;
}

unique_ptr<Contents>
append_contents(Table* table,
                unique_ptr<Contents>& E1,
                unique_ptr<Content>& E2) {
    auto content = std::make_unique<Contents>();
    return content;
}

unique_ptr<Content>
content_name(Table* table, unique_ptr<Name>& E1) {
    auto content = std::make_unique<Content>();
    return content;
}

unique_ptr<Content>
content_element(Table* table, unique_ptr<Element>& E1) {
    auto content = std::make_unique<Content>();
    return content;
}

unique_ptr<Content>
content_element_name(Table* table,
                     unique_ptr<Element>& E1,
                     unique_ptr<Name>& E2) {
    auto content = std::make_unique<Content>();
    return content;
}

unique_ptr<Tag>
reduce_stag(Table* table, unique_ptr<Name>& E1) {
    auto tag = std::make_unique<Tag>();
    return tag;
}

unique_ptr<Tag>
reduce_stag_attrs(Table* table, unique_ptr<Name>& E1, unique_ptr<Attrs>& E2) {
    auto tag = std::make_unique<Tag>();
    return tag;
}

unique_ptr<Tag>
reduce_etag(Table* table, unique_ptr<Name>& E1) {
    auto tag = std::make_unique<Tag>();
    return tag;
}

/**
 * Functions provided by the lexer for identifing terminals given the input
 * characters.  If no node is returned, check the current node for a matching
 * symbol and call scan to get its value.
 */

Node*   node_next(Node* node, int c);
Symbol* node_accept(Node* node);
Value*  node_scan(Node* node, Table*, const std::string&);

/**
 * Functions provided by the parser.  For each new symbol the parser determines
 * if the symbol should be pushed onto the stack, or reduced by a rule of the
 * grammar.  If the top symbols are reduced by a rule, call the reduce function
 * to determine the new value for the top of the stack.  After reducing the
 * rule, call goto to determine the next state of the parser.
 */

struct Rule;

State* find_shift(State* state, Symbol* sym);
Rule*  find_reduce(State* state, Symbol* sym, bool* accept);

Symbol* rule_nonterm(Rule* rule, size_t* length);
Value*  rule_reduce(Rule* rule, Table*, Value**);

State* find_goto(State* state, Symbol* sym);

/**
 * At startup, the lexer for finding terminals is in its initial node.  For the
 * parser, the stack is cleared and the initial state is placed onto the top of
 * the stack.
 */
void
Calculator::start()
{
    states.clear();
    symbols.clear();
    values.clear();

    node = &node0;
    text.clear();

    states.push_back(&state0);
};

/**
 * The calculator calls scan with the next input character to match terminals in
 * input.  If a terminal is found,  the calculator updates the stack based on
 * the action in the parse table.  Based on the action in the parse table, the
 * new symbol is either shifted onto the stack, or the stack is reduced by a
 * rule and the user defined action is called with the values on top of the
 * stack as arguments.
 */
bool
Calculator::scan(Table* table, int c)
{
    while (true)
    {
        if (node == &node0 && isspace(c)) {
            return true;
        }
        else {
            Node* next = node_next(node, c);
            
            if (next) {
                text.push_back(c);
                node = next;
                return true;
            }
            else {
                if (!node_accept(node)) {
                    return false;
                }
                
                Value* value = node_scan(node, table, text);
                if (!advance(table, node_accept(node), value)) {
                    return false;
                }
                node = &node0;
                text.clear();
            }
        }
    }
}

/**
 * The scanning process is continued until the end of the input.  At the end
 * of the input the end mark symbol reduces the remaining symbols still on the
 * stack into a single value.
 */
std::unique_ptr<Value>
Calculator::scan_end(Table* table)
{
    Symbol* accept = node_accept(node);
    
    if (accept) {
        Value* value = node_scan(node, table, text);
        if (!advance(table, accept, value)) {
            return nullptr;
        }
    } else if (node != &node0) {
        return nullptr;
    }
    
    if (!advance(table, &endmark, nullptr)) {
        return nullptr;
    }
    return std::unique_ptr<Value>(values.front());
};

/**
 * After a terminal is found in the input, it is either shifted onto the top of
 * the stack or the stack is reduced by one of the predefined rules in the
 * grammar.  If the stack is reduced, the corresponding user defined function is
 * called with the values on the top of the stack as arguments.
 */
bool
Calculator::advance(Table* table, Symbol* sym, Value* val)
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
            size_t length = 0;
            Symbol* nonterm = rule_nonterm(rule, &length);
            
            Value* result = rule_reduce(rule, table, values.data() + values.size());
            pop(length);
            
            top = states.back();
            State* found = find_goto(top, nonterm);
            push(found, nonterm, result);
        } else {
            return false;
        }
        
        if (accept) {
            return true;
        }
    }
};

void
Calculator::push(State* state, Symbol* sym, Value* val)
{
    states.push_back(state);
    symbols.push_back(sym);
    values.push_back(val);
}

void
Calculator::pop(size_t count)
{
    for (size_t i = 0; i < count; i++) {
        states.pop_back();
        symbols.pop_back();
        values.pop_back();
    }
};
