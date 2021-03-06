#include "code.hpp"

#include <algorithm>
#include <fstream>

extern std::string parser_header;
extern std::string parser_source;
//#include "parser_header.cpp"
//#include "parser_source.cpp"

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
    std::vector<Node*> sorted;
    for (auto& node : lexer.nodes) {
        sorted.push_back(node.get());
    }
    struct {
        bool operator()(Node* a, Node* b) const {
            return a->id < b->id;
        }
    } compare;
    
    std::sort(sorted.begin(), sorted.end(), compare);
    
    for (auto include : grammar.includes) {
        out << include << std::endl;
    }
    out << "#include <memory>\n";
    out << "using std::unique_ptr;\n";
    out << "using std::vector;\n\n";
    
    //out << "#include \"parser.hpp\"\n";
    out << parser_header;

    std::set<std::string> types;
    for (auto& term : grammar.terms) {
        if (!term.second->type.empty()) {
            types.insert(term.second->type);
        }
    }
    for (auto& nonterm : grammar.nonterms) {
        if (!nonterm.second->type.empty()) {
            types.insert(nonterm.second->type);
        }
    }

    for (auto type : types) {
        out << "class " << type << ";\n";
    }
    out << "\n";
    
    for (auto& term : grammar.terms) {
        write_terms(term.second.get(), out);
    }
    out << std::endl;

    for (auto& term : grammar.terms) {
        write_eval(term.second.get(), out);
    }
    
    for (auto node : sorted) {
        out << "extern Node node" << node->id << ";\n";
    }
    out << "\n";

    for (auto node : sorted) {
        write_scan(node, out);
    }
        
    for (auto node : sorted) {
        out << "Node node" << node->id << " = ";
        write_node(node, out);
    }
    out << "\n";
    
    out << parser_source;
}

/******************************************************************************/
bool
Code::write(const Grammar& grammar, const Solver& solver, std::ostream& out)
{
    std::vector<State*> states_sorted;
    
    out << "Symbol endmark;\n";

    for (auto& nonterm : grammar.nonterms) {
        write_nonterm(nonterm.second.get(), out);
    }
    out << std::endl;

    bool ok = write_reduce(grammar, out);
    if (!ok) {
        return false;
    }
    write_rules(grammar, out);

    for (auto& s : solver.sorted) {
        out << "extern State state" << s->id << ";\n";
    }
    out << std::endl;

    for (auto& a : solver.actions) {
        write_actions(a.get(), out);
    }
    out << std::endl;

    write_gotos(solver.sorted, out);
    write_states(solver.sorted, out);

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
Code::write_scan(Node* node, std::ostream& out)
{
    if (node->nexts.size() == 0) {
        return;
    }
    
    out << "Node*\n";
    out << "next" << node->id << "(int c) {\n";
    for (auto next : node->nexts) {
        out << "    if (";
        next.first.write(out);
        out << ") { return &node" << next.second->id << "; }\n";
    }
    out << "    return nullptr;\n";
    out << "}\n\n";
}

void
Code::write_node(Node* node, std::ostream& out)
{
    if (node->nexts.size() > 0) {
        out << "{&next" << node->id;
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

/******************************************************************************/
bool
Code::write_reduce(const Grammar& grammar, ostream& out)
{
    for (auto& rule : grammar.rules) {
        if (!rule->action.empty()) {
            write_rule_action(rule, out);
            write_call_action(rule, out);
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
    for (auto& nonterm : grammar.all) {
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
Code::write_gotos(const std::vector<State*>& states, std::ostream& out)
{
    for (auto s : states) {
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
Code::write_states(const std::vector<State*>& states, std::ostream& out)
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
