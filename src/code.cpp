#include "code.hpp"

#include <algorithm>
#include <fstream>

/******************************************************************************/
const char* header = R"""(

struct Symbol {
    const char* name;
};

struct Node {
    Node* (*next)(int c);
    Symbol* accept;
    Value* (*scan)(Table*, const std::string&);
};


struct Shift {
    Symbol* symbol;
    State*  next;
};

struct Rule {
    Symbol* nonterm;
    int length;
    Value* (*reduce)(Table*, Value**);
};

struct Reduce {
    Symbol* symbol;
    Rule*   rule;
    bool    accept;
};

struct Go {
    Symbol* symbol;
    State*  state;
};

struct State {
    std::vector<Shift>*  shift;
    std::vector<Reduce>* reduce;
    std::vector<Go>*     go;
};

)""";

/******************************************************************************/
const char* source = R"""(

Node*
node_next(Node* node, int c) {
    Node* next = nullptr;
    if (node->next && node->next(c)) {
        next = node->next(c);
    }
    return next;
}

Symbol*
node_accept(Node* node) {
    return node->accept;
}

Value*
node_scan(Node* node, Table* table, const std::string& text) {
    Value* value = nullptr;
    if (node->scan) {
        value = node->scan(table, text);
    }
    return value;
}

Rule*
find_reduce(State* state, Symbol* sym, bool* accept) {
    for (auto& s : *state->reduce) {
        if (s.symbol == nullptr || s.symbol == sym) {
            *accept = s.accept;
            return s.rule;
        }
    }
    *accept = false;
    return nullptr;
}

Symbol*
rule_nonterm(Rule* rule, size_t* length) {
    *length = rule->length;
    return rule->nonterm;
}

Value*
rule_reduce(Rule* rule, Table* table, Value** values) {
    if (rule->reduce) {
        return rule->reduce(table, values);
    } else {
        if (rule->length == 1) {
            return *(values - 1);
        } else {
            return nullptr;
        }
    }
}

State*
find_shift(State* state, Symbol* sym) {
    for (auto& s : *state->shift) {
        if (s.symbol == sym) {
            return s.next;
        }
    }
    return nullptr;
}
State*
find_goto(State* state, Symbol* sym) {
   if (!state->go)
        return nullptr;
   for (auto& g : *state->go) {
       if (g.symbol == sym) {
           return g.state;
       }
   }
   return nullptr;
};


)""";

/*******************************************************************************
 * Writes the source code for a lexer.  The source code will define a structure
 * for each state in DFA. This structure contains a method that takes a
 * character and returns either a new state in the DFA or a null pointer.  The
 * null indicates that the pattern matching is complete and the term value, if
 * any, for the current state is the type of token identified.
 */
void
Code::write(const Grammar& grammar, const Lexer& lexer, std::ostream& out)
{
    int id = 0;
    std::map<Node*, int> ids;
    for (auto& state : lexer.nodes) {
        ids[state.get()] = id++;
    }
    
    for (auto include : grammar.includes) {
        out << include << std::endl;
    }
    out << "#include <memory>\n";
    out << "using std::unique_ptr;\n";
    out << "using std::vector;\n\n";
    
    out << header;
    
    std::set<std::string> types;
    for (auto& term : grammar.terms) {
        if (!term->type.empty()) {
            types.insert(term->type);
        }
    }
    
    for (auto& nonterm : grammar.nonterms) {
        if (!nonterm->type.empty()) {
            types.insert(nonterm->type);
        }
    }
    
    for (auto& term : grammar.terms) {
        write_terms(term.get(), out);
    }
    out << "\n";
    
    for (auto type : types) {
        out << "class " << type << ";\n";
    }
    out << "\n";
    
    for (auto& term : grammar.terms) {
        write_eval(term.get(), out);
    }
    
    for (auto& node : lexer.nodes) {
        out << "extern Node node" << ids[node.get()] << ";\n";
    }
    out << "\n";
    
    for (auto& node : lexer.nodes) {
        write_scan(node.get(), ids, out);
    }
    
    for (auto& node : lexer.nodes) {
        out << "Node node" << ids[node.get()] << " = ";
        write_node(node.get(), ids, out);
    }
    out << "\n";
    
    out << source;
}

/******************************************************************************/
bool
Code::write(const Grammar& grammar, const Solver& solver, std::ostream& out)
{
    std::vector<State*> states_sorted;
    
    out << "Symbol endmark;\n";
    
    for (auto& nonterm : grammar.nonterms) {
        write_nonterm(nonterm.get(), out);
    }
    out << std::endl;
    
    bool ok = write_reduce(grammar, out);
    if (!ok) {
        return false;
    }
    write_rules(grammar, out);
    
    for (auto& s : solver.states) {
        out << "extern State state" << s->id << ";\n";
    }
    out << std::endl;
    
    for (auto& a : solver.actions) {
        write_actions(a.get(), out);
    }
    out << std::endl;
    
    write_gotos(solver.states, out);
    write_states(solver.states, out);
    
    out << "Node* lexer_start = &node0;\n\n";
    out << "State* parser_start = &state0;\n\n";
    out << "Symbol* symbol_endmark = &endmark;\n\n";
    
    //out << parser_source;
    
    return true;
}

/******************************************************************************/
void
Code::write_terms(Term* term, std::ostream& out)
{
    out << "Symbol term" << term->rank;
    out << " = {\"" << term->name << "\"};\n";
}

void
Code::write_eval(Term* term, std::ostream& out)
{
    if (term->action.empty())
        return;
    
    out << "unique_ptr<" << term->type << ">\n";
    out << term->action << "(Table*, const std::string&);\n\n";
    
    out << "Value*\n";
    out << "scan" << term->rank << "(Table* t, const std::string& s) {\n";
    out << "    unique_ptr<" << term->type << "> value = " << term->action << "(t, s);\n";
    out << "    return value.release();\n";
    out << "}\n\n";
}

/**
 * Writes the source code for a single state of the lexer.  The source code
 * defines a structure and a function for each node.  The function takes an
 * input character and returns the next node in the DFA or a null pointer.
 */
void
Code::write_scan(Node* node, std::map<Node*, int>& ids, std::ostream& out)
{
    if (node->nexts.size() == 0) {
        return;
    }
    
    out << "Node*\n";
    out << "next" << ids[node] << "(int c) {\n";
    for (auto next : node->nexts) {
        out << "    if (";
        write_range(&next.first, out);
        out << ") { return &node" << ids[next.second] << "; }\n";
    }
    out << "    return nullptr;\n";
    out << "}\n\n";
}

void
Code::write_node(Node* node, std::map<Node*, int>& ids, std::ostream& out)
{
    if (node->nexts.size() > 0) {
        out << "{&next" << ids[node];
    } else {
        out << "{nullptr";
    }
    
    if (node->accept) {
        out << ", &term" << node->accept->rank;
        if (node->accept->action.size() > 0) {
            out << ", &scan" << node->accept->rank << "";
        } else {
            out << ", nullptr";
        }
    } else {
        out << ", nullptr";
        out << ", nullptr";
    }
    out << "};\n";
}

void
Code::write_range(const Node::Range* range, std::ostream& out)
{
    int first = range->first;
    int last  = range->last;
    
    if (first == last) {
        if ((first <= 127) && isprint(first) && first != '\'' && first != '\\') {
            out << "c == '" << (char)first << "'";
        } else {
            out << "c == " << first << "";
        }
    } else {
        if ((first <= 127) && (last <= 127)
            && isprint(first) && isprint(last)
            && first != '\'' && last != '\''
            && first != '\\' && last != '\\') {
            out << "(c >= '" << (char)first << "')";
            out << " && ";
            out << "(c <= '" << (char)last << "')";
        } else {
            out << "(c >= " << first << ")";
            out << " && ";
            out << "(c <= " << last << ")";
        }
    }
}

/******************************************************************************/
bool
Code::write_reduce(const Grammar& grammar, ostream& out)
{
    for (auto& nonterm : grammar.nonterms) {
        for (auto& rule : nonterm->rules) {
            if (!rule->action.empty()) {
                write_rule_action(rule.get(), out);
                write_call_action(rule.get(), out);
            }
            else {
                size_t count = 0;
                std::string type;
                for (auto sym : rule->product) {
                    if (!sym->type.empty()) {
                        count++;
                        type = sym->type;
                    }
                }
                if (count != 1) {
                    std::cerr << "Production rules without a method must have only "
                    "one symbol with a type.";
                    return false;
                }
                if (type.compare(rule->nonterm->type) != 0) {
                    std::cerr << "Production rules without a method must have only "
                    "one symbol with the same type as the result.";
                    return false;
                }
            }
        }
    }
    return true;
}

void
Code::write_rule_action(Nonterm::Rule* rule, std::ostream& out)
{
    if (!rule->nonterm->type.empty()) {
        out << "unique_ptr<" << rule->nonterm->type << ">\n";
    } else {
        out << "void\n";
    }
    
    out << rule->action << "(";
    out << "Table*";
    
    bool comma = true;
    for (auto sym : rule->product) {
        if (!sym->type.empty()) {
            if (comma) {
                out << ", ";
            } else {
                comma = true;
            }
            out << "unique_ptr<" << sym->type << ">&";
        }
    }
    out << ");\n\n";
}

void
Code::write_call_action(Nonterm::Rule* rule, std::ostream& out)
{
    out << "Value*\n";
    out << rule->action << "(Table* table, Value** values) {\n";
    
    int i = 0;
    for (Symbol* sym : rule->product) {
        int index = i - (int)rule->product.size();
        if (!sym->type.empty()) {
            out << "    unique_ptr<" << sym->type << "> ";
            out << "E" << i;
            out << "(dynamic_cast<" << sym->type << "*>";
            out << "(values[" << index << "]));\n";
        }
        i++;
    }
    
    out << "    unique_ptr<" << rule->nonterm->type << "> ";
    out << "R = "<< rule->action << "(";
    out << "table";
    
    i = 0;
    bool comma = true;
    for (Symbol* sym : rule->product) {
        if (!sym->type.empty()) {
            if (comma) {
                out << ", ";
            } else {
                comma = true;
            }
            out << "E" << i;
        }
        i++;
    }
    out << ");\n";
    
    out << "    return R.release();\n";
    out << "}\n\n";
}

/******************************************************************************/
void
Code::write_nonterm(Nonterm* nonterm, std::ostream& out)
{
    out << "Symbol nonterm" << nonterm->id;
    out << " = {\"" << nonterm->name << "\"};\n";
}

void
Code::write_rules(const Grammar& grammar, std::ostream& out)
{
    for (auto& nonterm : grammar.nonterms) {
        for (auto& rule : nonterm->rules) {
            out << "Rule rule" << rule->id << " = ";
            out << "{&nonterm" << rule->nonterm->id << ", ";
            out << rule->product.size() << ", ";
            if (!rule->action.empty()) {
                out << "&" << rule->action;
            } else {
                out << "nullptr";
            }
            out << "};\n";
        }
    }
    out << "\n";
}

/******************************************************************************/
void
Code::write_actions(State::Actions* actions, ostream& out)
{
    bool comma = false;
    out << "std::vector<Shift> shift" << actions->id << " = {";
    for (auto& act : actions->shift) {
        if (comma) { out << ", "; } else { comma = true; }
        out << "{&"; act.first->write(out);
        out << ", &state" << act.second->id; out << "}";
    }
    out << "};\n";
    
    comma = false;
    out << "std::vector<Reduce> reduce" << actions->id << " = {";
    for (auto& act : actions->reduce) {
        if (comma) { out << ", "; } else { comma = true; }
        out << "{&"; act.first->write(out);
        out << ", &rule" << act.second->id << ", false}";
    }
    for (auto& act : actions->accept) {
        if (comma) { out << ", "; } else { comma = true; }
        out << "{&"; act.first->write(out);
        out << ", &rule" << act.second->id << ", true}";
    }
    if (actions->any) {
        if (comma) { out << ", "; } else { comma = true; }
        out << "{nullptr";
        out << ", &rule" << actions->any->id << ", false}";
    }
    out << "};\n";
}

void
Code::write_gotos(const std::vector<std::unique_ptr<State>>& states, std::ostream& out)
{
    for (auto& s : states) {
        if (s->gotos.size() == 0) {
            continue;
        }
        bool comma = false;
        out << "std::vector<Go> go" << s->id << " = {";
        for (auto& g : s->gotos) {
            if (comma) { out << ", "; } else { comma = true; }
            out << "{&";
            g.first->write(out);
            out << ", &state" << g.second->id << "}";
        }
        out << "};\n";
    }
    out << "\n";
}

void
Code::write_states(const std::vector<std::unique_ptr<State>>& states, std::ostream& out)
{
    for (auto& s : states) {
        out << "State state" << s->id << " = ";
        out << "{&shift" << s->actions->id << ", ";
        out << "&reduce" << s->actions->id << ", ";
        if (s->gotos.size() > 0) {
            out << "&go" << s->id;
        } else {
            out << "nullptr";
        }
        out << "};\n";
    }
    out << "\n";
}
