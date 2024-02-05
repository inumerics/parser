#include "calculator.hpp"
#include <memory>
using std::unique_ptr;
using std::vector;



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

Symbol term0 = {"num"};
Symbol term1 = {"hex"};
Symbol term2 = {"exit"};
Symbol term3 = {"ident"};
Symbol term4 = {"="};
Symbol term5 = {"+"};
Symbol term6 = {"-"};
Symbol term7 = {"*"};
Symbol term8 = {"/"};
Symbol term9 = {"("};
Symbol term10 = {")"};

class Ident;
class Num;

unique_ptr<Num>
scan_num(Table*, const std::string&);

Value*
scan0(Table* t, const std::string& s) {
    unique_ptr<Num> value = scan_num(t, s);
    return value.release();
}

unique_ptr<Num>
scan_hex(Table*, const std::string&);

Value*
scan1(Table* t, const std::string& s) {
    unique_ptr<Num> value = scan_hex(t, s);
    return value.release();
}

unique_ptr<Ident>
scan_ident(Table*, const std::string&);

Value*
scan3(Table* t, const std::string& s) {
    unique_ptr<Ident> value = scan_ident(t, s);
    return value.release();
}

extern Node node0;
extern Node node1;
extern Node node2;
extern Node node3;
extern Node node4;
extern Node node5;
extern Node node6;
extern Node node7;
extern Node node8;
extern Node node9;
extern Node node10;
extern Node node11;
extern Node node12;
extern Node node13;
extern Node node14;
extern Node node15;
extern Node node16;

Node*
next0(int c) {
    if (c == '(') { return &node1; }
    if (c == ')') { return &node2; }
    if (c == '*') { return &node3; }
    if (c == '+') { return &node4; }
    if (c == '-') { return &node5; }
    if (c == '/') { return &node6; }
    if (c == '0') { return &node7; }
    if ((c >= '1') && (c <= '9')) { return &node8; }
    if (c == '=') { return &node9; }
    if ((c >= 'A') && (c <= 'Z')) { return &node10; }
    if ((c >= 'a') && (c <= 'd')) { return &node10; }
    if (c == 'e') { return &node11; }
    if ((c >= 'f') && (c <= 'z')) { return &node10; }
    return nullptr;
}

Node*
next7(int c) {
    if ((c >= '0') && (c <= '9')) { return &node8; }
    if (c == 'x') { return &node15; }
    return nullptr;
}

Node*
next8(int c) {
    if ((c >= '0') && (c <= '9')) { return &node8; }
    return nullptr;
}

Node*
next10(int c) {
    if ((c >= 'A') && (c <= 'Z')) { return &node10; }
    if ((c >= 'a') && (c <= 'z')) { return &node10; }
    return nullptr;
}

Node*
next11(int c) {
    if ((c >= 'A') && (c <= 'Z')) { return &node10; }
    if ((c >= 'a') && (c <= 'w')) { return &node10; }
    if (c == 'x') { return &node12; }
    if ((c >= 'y') && (c <= 'z')) { return &node10; }
    return nullptr;
}

Node*
next12(int c) {
    if ((c >= 'A') && (c <= 'Z')) { return &node10; }
    if ((c >= 'a') && (c <= 'h')) { return &node10; }
    if (c == 'i') { return &node13; }
    if ((c >= 'j') && (c <= 'z')) { return &node10; }
    return nullptr;
}

Node*
next13(int c) {
    if ((c >= 'A') && (c <= 'Z')) { return &node10; }
    if ((c >= 'a') && (c <= 's')) { return &node10; }
    if (c == 't') { return &node14; }
    if ((c >= 'u') && (c <= 'z')) { return &node10; }
    return nullptr;
}

Node*
next14(int c) {
    if ((c >= 'A') && (c <= 'Z')) { return &node10; }
    if ((c >= 'a') && (c <= 'z')) { return &node10; }
    return nullptr;
}

Node*
next15(int c) {
    if ((c >= '0') && (c <= '9')) { return &node16; }
    if ((c >= 'A') && (c <= 'F')) { return &node16; }
    if ((c >= 'a') && (c <= 'f')) { return &node16; }
    return nullptr;
}

Node*
next16(int c) {
    if ((c >= '0') && (c <= '9')) { return &node16; }
    if ((c >= 'A') && (c <= 'F')) { return &node16; }
    if ((c >= 'a') && (c <= 'f')) { return &node16; }
    return nullptr;
}

Node node0 = {&next0, nullptr, nullptr};
Node node1 = {nullptr, &term9, nullptr};
Node node2 = {nullptr, &term10, nullptr};
Node node3 = {nullptr, &term7, nullptr};
Node node4 = {nullptr, &term5, nullptr};
Node node5 = {nullptr, &term6, nullptr};
Node node6 = {nullptr, &term8, nullptr};
Node node7 = {&next7, &term0, &scan0};
Node node8 = {&next8, &term0, &scan0};
Node node9 = {nullptr, &term4, nullptr};
Node node10 = {&next10, &term3, &scan3};
Node node11 = {&next11, &term3, &scan3};
Node node12 = {&next12, &term3, &scan3};
Node node13 = {&next13, &term3, &scan3};
Node node14 = {&next14, &term2, nullptr};
Node node15 = {&next15, nullptr, nullptr};
Node node16 = {&next16, &term1, &scan1};



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


Symbol endmark;
Symbol nonterm0 = {"total"};
Symbol nonterm1 = {"line"};
Symbol nonterm2 = {"add"};
Symbol nonterm3 = {"mul"};
Symbol nonterm4 = {"int"};

unique_ptr<Num>
reduce_total(Table*, unique_ptr<Num>&);

Value*
reduce_total(Table* table, Value** values) {
    unique_ptr<Num> E0(dynamic_cast<Num*>(values[-1]));
    unique_ptr<Num> R = reduce_total(table, E0);
    return R.release();
}

unique_ptr<Num>
reduce_line(Table*, unique_ptr<Num>&);

Value*
reduce_line(Table* table, Value** values) {
    unique_ptr<Num> E0(dynamic_cast<Num*>(values[-1]));
    unique_ptr<Num> R = reduce_line(table, E0);
    return R.release();
}

unique_ptr<Num>
reduce_assign(Table*, unique_ptr<Ident>&, unique_ptr<Num>&);

Value*
reduce_assign(Table* table, Value** values) {
    unique_ptr<Ident> E0(dynamic_cast<Ident*>(values[-3]));
    unique_ptr<Num> E2(dynamic_cast<Num*>(values[-1]));
    unique_ptr<Num> R = reduce_assign(table, E0, E2);
    return R.release();
}

unique_ptr<Num>
reduce_exit(Table*);

Value*
reduce_exit(Table* table, Value** values) {
    unique_ptr<Num> R = reduce_exit(table);
    return R.release();
}

unique_ptr<Num>
reduce_add_mul(Table*, unique_ptr<Num>&, unique_ptr<Num>&);

Value*
reduce_add_mul(Table* table, Value** values) {
    unique_ptr<Num> E0(dynamic_cast<Num*>(values[-3]));
    unique_ptr<Num> E2(dynamic_cast<Num*>(values[-1]));
    unique_ptr<Num> R = reduce_add_mul(table, E0, E2);
    return R.release();
}

unique_ptr<Num>
reduce_sub_mul(Table*, unique_ptr<Num>&, unique_ptr<Num>&);

Value*
reduce_sub_mul(Table* table, Value** values) {
    unique_ptr<Num> E0(dynamic_cast<Num*>(values[-3]));
    unique_ptr<Num> E2(dynamic_cast<Num*>(values[-1]));
    unique_ptr<Num> R = reduce_sub_mul(table, E0, E2);
    return R.release();
}

unique_ptr<Num>
reduce_mul_int(Table*, unique_ptr<Num>&, unique_ptr<Num>&);

Value*
reduce_mul_int(Table* table, Value** values) {
    unique_ptr<Num> E0(dynamic_cast<Num*>(values[-3]));
    unique_ptr<Num> E2(dynamic_cast<Num*>(values[-1]));
    unique_ptr<Num> R = reduce_mul_int(table, E0, E2);
    return R.release();
}

unique_ptr<Num>
reduce_div_int(Table*, unique_ptr<Num>&, unique_ptr<Num>&);

Value*
reduce_div_int(Table* table, Value** values) {
    unique_ptr<Num> E0(dynamic_cast<Num*>(values[-3]));
    unique_ptr<Num> E2(dynamic_cast<Num*>(values[-1]));
    unique_ptr<Num> R = reduce_div_int(table, E0, E2);
    return R.release();
}

unique_ptr<Num>
reduce_paren(Table*, unique_ptr<Num>&);

Value*
reduce_paren(Table* table, Value** values) {
    unique_ptr<Num> E1(dynamic_cast<Num*>(values[-2]));
    unique_ptr<Num> R = reduce_paren(table, E1);
    return R.release();
}

unique_ptr<Num>
reduce_lookup(Table*, unique_ptr<Ident>&);

Value*
reduce_lookup(Table* table, Value** values) {
    unique_ptr<Ident> E0(dynamic_cast<Ident*>(values[-1]));
    unique_ptr<Num> R = reduce_lookup(table, E0);
    return R.release();
}

Rule rule0 = {&nonterm0, 1, &reduce_total};
Rule rule1 = {&nonterm1, 1, &reduce_line};
Rule rule2 = {&nonterm1, 3, &reduce_assign};
Rule rule3 = {&nonterm1, 1, &reduce_exit};
Rule rule4 = {&nonterm2, 1, nullptr};
Rule rule5 = {&nonterm2, 3, &reduce_add_mul};
Rule rule6 = {&nonterm2, 3, &reduce_sub_mul};
Rule rule7 = {&nonterm3, 1, nullptr};
Rule rule8 = {&nonterm3, 3, &reduce_mul_int};
Rule rule9 = {&nonterm3, 3, &reduce_div_int};
Rule rule10 = {&nonterm3, 3, &reduce_paren};
Rule rule11 = {&nonterm4, 1, &reduce_lookup};
Rule rule12 = {&nonterm4, 1, nullptr};
Rule rule13 = {&nonterm4, 1, nullptr};

extern State state0;
extern State state1;
extern State state2;
extern State state3;
extern State state4;
extern State state5;
extern State state6;
extern State state7;
extern State state8;
extern State state9;
extern State state10;
extern State state11;
extern State state12;
extern State state13;
extern State state14;
extern State state15;
extern State state16;
extern State state17;
extern State state18;
extern State state19;
extern State state20;
extern State state21;
extern State state22;
extern State state23;
extern State state24;
extern State state25;
extern State state26;
extern State state27;
extern State state28;
extern State state29;
extern State state30;
extern State state31;
extern State state32;
extern State state33;
extern State state34;
extern State state35;
extern State state36;
extern State state37;
extern State state38;

std::vector<Shift> shift0 = {{&term0, &state1}, {&term1, &state2}, {&term2, &state3}, {&term3, &state4}, {&term9, &state5}};
std::vector<Reduce> reduce0 = {};
std::vector<Shift> shift1 = {};
std::vector<Reduce> reduce1 = {{nullptr, &rule12, false}};
std::vector<Shift> shift2 = {};
std::vector<Reduce> reduce2 = {{nullptr, &rule13, false}};
std::vector<Shift> shift3 = {};
std::vector<Reduce> reduce3 = {{nullptr, &rule3, false}};
std::vector<Shift> shift4 = {{&term4, &state37}};
std::vector<Reduce> reduce4 = {{nullptr, &rule11, false}};
std::vector<Shift> shift5 = {{&term0, &state19}, {&term1, &state20}, {&term3, &state21}, {&term9, &state22}};
std::vector<Reduce> reduce5 = {};
std::vector<Shift> shift6 = {};
std::vector<Reduce> reduce6 = {{&endmark, &rule0, true}};
std::vector<Shift> shift7 = {{&term5, &state15}, {&term6, &state16}};
std::vector<Reduce> reduce7 = {{nullptr, &rule1, false}};
std::vector<Shift> shift8 = {{&term7, &state10}, {&term8, &state11}};
std::vector<Reduce> reduce8 = {{nullptr, &rule4, false}};
std::vector<Shift> shift9 = {};
std::vector<Reduce> reduce9 = {{nullptr, &rule7, false}};
std::vector<Shift> shift10 = {{&term0, &state1}, {&term1, &state2}, {&term3, &state12}};
std::vector<Reduce> reduce10 = {};
std::vector<Shift> shift12 = {};
std::vector<Reduce> reduce12 = {{nullptr, &rule11, false}};
std::vector<Shift> shift13 = {};
std::vector<Reduce> reduce13 = {{nullptr, &rule9, false}};
std::vector<Shift> shift14 = {};
std::vector<Reduce> reduce14 = {{nullptr, &rule8, false}};
std::vector<Shift> shift15 = {{&term0, &state1}, {&term1, &state2}, {&term3, &state12}, {&term9, &state5}};
std::vector<Reduce> reduce15 = {};
std::vector<Shift> shift17 = {{&term7, &state10}, {&term8, &state11}};
std::vector<Reduce> reduce17 = {{nullptr, &rule6, false}};
std::vector<Shift> shift18 = {{&term7, &state10}, {&term8, &state11}};
std::vector<Reduce> reduce18 = {{nullptr, &rule5, false}};
std::vector<Shift> shift23 = {{&term5, &state30}, {&term6, &state31}, {&term10, &state32}};
std::vector<Reduce> reduce23 = {};
std::vector<Shift> shift24 = {{&term7, &state26}, {&term8, &state27}};
std::vector<Reduce> reduce24 = {{nullptr, &rule4, false}};
std::vector<Shift> shift26 = {{&term0, &state19}, {&term1, &state20}, {&term3, &state21}};
std::vector<Reduce> reduce26 = {};
std::vector<Shift> shift32 = {};
std::vector<Reduce> reduce32 = {{nullptr, &rule10, false}};
std::vector<Shift> shift33 = {{&term7, &state26}, {&term8, &state27}};
std::vector<Reduce> reduce33 = {{nullptr, &rule6, false}};
std::vector<Shift> shift34 = {{&term7, &state26}, {&term8, &state27}};
std::vector<Reduce> reduce34 = {{nullptr, &rule5, false}};
std::vector<Shift> shift35 = {{&term5, &state30}, {&term6, &state31}, {&term10, &state36}};
std::vector<Reduce> reduce35 = {};
std::vector<Shift> shift38 = {{&term5, &state15}, {&term6, &state16}};
std::vector<Reduce> reduce38 = {{nullptr, &rule2, false}};

std::vector<Go> go0 = {{&nonterm1, &state6}, {&nonterm2, &state7}, {&nonterm3, &state8}, {&nonterm4, &state9}};
std::vector<Go> go5 = {{&nonterm2, &state23}, {&nonterm3, &state24}, {&nonterm4, &state25}};
std::vector<Go> go10 = {{&nonterm4, &state14}};
std::vector<Go> go11 = {{&nonterm4, &state13}};
std::vector<Go> go15 = {{&nonterm3, &state18}, {&nonterm4, &state9}};
std::vector<Go> go16 = {{&nonterm3, &state17}, {&nonterm4, &state9}};
std::vector<Go> go22 = {{&nonterm2, &state35}, {&nonterm3, &state24}, {&nonterm4, &state25}};
std::vector<Go> go26 = {{&nonterm4, &state29}};
std::vector<Go> go27 = {{&nonterm4, &state28}};
std::vector<Go> go30 = {{&nonterm3, &state34}, {&nonterm4, &state25}};
std::vector<Go> go31 = {{&nonterm3, &state33}, {&nonterm4, &state25}};
std::vector<Go> go37 = {{&nonterm2, &state38}, {&nonterm3, &state8}, {&nonterm4, &state9}};

State state0 = {&shift0, &reduce0, &go0};
State state1 = {&shift1, &reduce1, nullptr};
State state2 = {&shift2, &reduce2, nullptr};
State state3 = {&shift3, &reduce3, nullptr};
State state4 = {&shift4, &reduce4, nullptr};
State state5 = {&shift5, &reduce5, &go5};
State state6 = {&shift6, &reduce6, nullptr};
State state7 = {&shift7, &reduce7, nullptr};
State state8 = {&shift8, &reduce8, nullptr};
State state9 = {&shift9, &reduce9, nullptr};
State state10 = {&shift10, &reduce10, &go10};
State state11 = {&shift10, &reduce10, &go11};
State state12 = {&shift12, &reduce12, nullptr};
State state13 = {&shift13, &reduce13, nullptr};
State state14 = {&shift14, &reduce14, nullptr};
State state15 = {&shift15, &reduce15, &go15};
State state16 = {&shift15, &reduce15, &go16};
State state17 = {&shift17, &reduce17, nullptr};
State state18 = {&shift18, &reduce18, nullptr};
State state19 = {&shift1, &reduce1, nullptr};
State state20 = {&shift2, &reduce2, nullptr};
State state21 = {&shift12, &reduce12, nullptr};
State state22 = {&shift5, &reduce5, &go22};
State state23 = {&shift23, &reduce23, nullptr};
State state24 = {&shift24, &reduce24, nullptr};
State state25 = {&shift9, &reduce9, nullptr};
State state26 = {&shift26, &reduce26, &go26};
State state27 = {&shift26, &reduce26, &go27};
State state28 = {&shift13, &reduce13, nullptr};
State state29 = {&shift14, &reduce14, nullptr};
State state30 = {&shift5, &reduce5, &go30};
State state31 = {&shift5, &reduce5, &go31};
State state32 = {&shift32, &reduce32, nullptr};
State state33 = {&shift33, &reduce33, nullptr};
State state34 = {&shift34, &reduce34, nullptr};
State state35 = {&shift35, &reduce35, nullptr};
State state36 = {&shift32, &reduce32, nullptr};
State state37 = {&shift15, &reduce15, &go37};
State state38 = {&shift38, &reduce38, nullptr};

Node* lexer_start = &node0;

State* parser_start = &state0;

Symbol* symbol_endmark = &endmark;

