#include "json.hpp"

#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>
using std::unique_ptr;

/******************************************************************************/
JsonBool::JsonBool(bool value): value(value) {}

JsonNumber::JsonNumber(double value): value(value) {}

JsonString::JsonString(const std::string& value): value(value) {}

/******************************************************************************/
bool
JsonObject::has_key(const std::string& name) const {
    return (values.count(name) > 0);
}

JsonObject*
JsonObject::get_object(const std::string& name) const{
    if (values.count(name) > 0) {
        return dynamic_cast<JsonObject*>(values.at(name).get());
    } else {
        return nullptr;
    }
}

JsonArray*
JsonObject::get_array(const std::string& name) const {
    if (values.count(name) > 0) {
        return dynamic_cast<JsonArray*>(values.at(name).get());
    } else {
        return nullptr;
    }
}

std::string
JsonObject::get_string(const std::string& name) const
{
    if (values.count(name) > 0) {
        JsonString* str = dynamic_cast<JsonString*>(values.at(name).get());
        if (str) {
            return str->value;
        }
    }
    return "";
}

bool
JsonObject::get_bool(const std::string& name) const
{
    if (values.count(name) > 0) {
        JsonBool* value = dynamic_cast<JsonBool*>(values.at(name).get());
        if (value) {
            return value->value;
        }
    }
    return false;
}

/******************************************************************************/
unique_ptr<JsonBool>
read_true(Table* table, const std::string& text) {
    return std::make_unique<JsonBool>(true);
}

unique_ptr<JsonBool>
read_false(Table* table, const std::string& text) {
    return std::make_unique<JsonBool>(false);
}

unique_ptr<JsonNull>
read_null(Table* table, const std::string& text) {
    return std::make_unique<JsonNull>();
}

unique_ptr<JsonValue>
read_num(Table* table, const std::string& text) {
    return std::make_unique<JsonNumber>(std::stoi(text));
}

unique_ptr<JsonString>
read_string(Table* table, const std::string& text)
{
    if (text.size() > 2) {
        return std::make_unique<JsonString>(text.substr(1, text.size() - 2));
    } else {
        return std::make_unique<JsonString>("");
    }
}

/******************************************************************************/
unique_ptr<JsonValue>
reduce_json(Table* table, unique_ptr<JsonValue>& E1) {
    table->value = std::move(E1);
    return std::make_unique<JsonValue>();
}

unique_ptr<JsonValue>
value_object(Table* table, unique_ptr<JsonObject>& E1) {
    return std::move(E1);
}

unique_ptr<JsonValue>
value_array(Table* table, unique_ptr<JsonArray>& E1) {
    return std::move(E1);
}

unique_ptr<JsonValue>
value_string(Table* table, unique_ptr<JsonString>& E1) {
    return std::move(E1);
}

unique_ptr<JsonObject>
object_empty(Table* table) {
    return std::make_unique<JsonObject>();
}

unique_ptr<JsonObject>
object_members(Table* table, unique_ptr<JsonObject>& E1) {
    return std::move(E1);
}

unique_ptr<JsonObject>
members_reduce(Table* table, unique_ptr<JsonObject>& E1) {
    return std::move(E1);
}

unique_ptr<JsonObject>
members_member(Table* table, unique_ptr<JsonObject>& E1,
               unique_ptr<JsonObject>& E2) {
    for (auto& member : E2->values) {
        E1->values[member.first] = std::move(member.second);
    }
    return std::move(E1);
}

unique_ptr<JsonObject>
member_reduce(Table* table, unique_ptr<JsonString>& E1,
              unique_ptr<JsonValue>& E2) {
    auto result = std::make_unique<JsonObject>();
    result->values[E1->value] = std::move(E2);
    return result;
}

unique_ptr<JsonArray>
array_empty(Table* table) {
    return std::make_unique<JsonArray>();
}

unique_ptr<JsonArray>
array_values(Table* table, unique_ptr<JsonArray>& E1) {
    return std::move(E1);
}

unique_ptr<JsonArray>
values_reduce(Table* table, unique_ptr<JsonValue>& E1) {
    auto result = std::make_unique<JsonArray>();
    result->values.push_back(std::move(E1));
    return result;
}

unique_ptr<JsonArray>
values_value(Table* table, unique_ptr<JsonArray>& E1,
             unique_ptr<JsonValue>& E2) {
    E1->values.push_back(std::move(E2));
    return std::move(E1);
}

/******************************************************************************/
void
Parser::start()
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
Parser::scan(Table* table, int c)
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
Parser::scan_end(Table* table)
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
Parser::advance(Table* table, Symbol* sym, Value* val)
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
Parser::push(State* state, Symbol* sym, Value* val)
{
    states.push_back(state);
    symbols.push_back(sym);
    values.push_back(val);
}

void
Parser::pop(size_t count)
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
    auto parser = std::make_unique<Parser>();
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
