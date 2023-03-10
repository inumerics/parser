#include "calculator.hpp"

#include <iostream>
#include <sstream>
#include <memory>
using std::unique_ptr;

/**
 * Types and functions of the calculator.  Each terminal can specify an
 * associated class and the name of a function that takes the matched string
 * from the input and returns a value of that class.
 */
unique_ptr<Value>
scan_num(Table* table, const std::string& text)
{
    return std::make_unique<Value>(std::stoi(text));
}

unique_ptr<Value>
scan_hex(Table* table, const std::string& text)
{
    std::stringstream stream;
    stream << std::hex << text;
    int num = 0;
    stream >> num;
    return std::make_unique<Value>(num);
}

/**
 * Functions listed for the nonterminals of the grammar.  Our program will call
 * these functions when its associated rule is seen in the input.  Each of these
 * function is passed arguments for each terminal in the rule that has an
 * associated type.  Once again, if any function is not implemented, including
 * with the exact argument types, then link errors will occur when building the
 * final calculator program.
 */
unique_ptr<Value>
reduce_add_mul(Table* table, unique_ptr<Value>& E1,
               unique_ptr<Value>& E2)
{
    unique_ptr<Value> result = std::move(E1);
    result->value += E2->value;
    return result;
}

unique_ptr<Value>
reduce_sub_mul(Table* table, unique_ptr<Value>& E1,
               unique_ptr<Value>& E2)
{
    unique_ptr<Value> result = std::move(E1);
    result->value -= E2->value;
    return result;
}

unique_ptr<Value>
reduce_mul_int(Table* table, unique_ptr<Value>& E1,
               unique_ptr<Value>& E2)
{
    unique_ptr<Value> result = std::move(E1);
    result->value *= E2->value;
    return result;
}

unique_ptr<Value>
reduce_div_int(Table* table, unique_ptr<Value>& E1,
               unique_ptr<Value>& E2)
{
    unique_ptr<Value> result = std::move(E1);
    result->value /= E2->value;
    return result;
}

unique_ptr<Value>
reduce_paren(Table* table, unique_ptr<Value>& E1)
{
    return std::move(E1);
}

/**
 * By definition, the first rule specified in the grammar is the rule that is
 * followed by the end of the input string and therefore the last nonterminal
 * left on the stack after reading the entire input.  Upon reducing the entire
 * input string to this terminal, the final program calls its associated
 * function. For our calculator example, the last function prints the calculated
 * result of the input expression.
 */

unique_ptr<Value>
reduce_total(Table* table, unique_ptr<Value>& E1)
{
    unique_ptr<Value> result = std::move(E1);
    std::cout << result->value << "\n";
    return result;
}

/******************************************************************************/
void
Calculator::start()
{
    this->node = &node0;;
    this->text.clear();
    
    states.clear();
    symbols.clear();
    values.clear();
    
    states.push_back(&state0);
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

bool
Calculator::scan_end(Table* table)
{
    Symbol* accept = node_accept(node);
    
    if (accept) {
        Value* value = node_scan(node, table, text);
        if (!advance(table, accept, value)) {
            return false;
        }
    } else if (node != &node0) {
        return false;
    }
    
    // TODO Check for accept.
    if (!advance(table, &endmark, nullptr)) {
        return false;
    }
    return true;
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

/******************************************************************************/
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

/******************************************************************************/
int
main(int argc, const char * argv[])
{
    if (argc != 2) {
        std::cerr << "Expected a single input string.\n";
        return 1;
    }
    
    // TODO Just build on the stack.
    auto parser = std::make_unique<Calculator>();
    parser->start();
    if (!parser) {
        std::cerr << "Error while building parser.\n";
        return 1;
    }
    
    Table table;
    
    std::stringstream in(argv[1]);
    
    while (true) {
        int c = in.get();
        if (c == EOF) {
            bool ok = parser->scan_end(&table);
            if (!ok) {
                std::cerr << "Unexpected end of the input.\n";
                return 1;
            }
            break;
        } else {
            bool ok = parser->scan(&table, c);
            if (!ok) {
                std::cerr << "Unexpected character.\n";
                return 1;
            }
        }
    }
    
    return 0;
}
