# Generated Lexer Source Code 

Solving for the nodes of the lexer provides the states needed to build a finite 
state machine that matches patterns in an input string.  To incorporate this
state machine in another program, this parser generator writes the source code
for each state to a source file.  The generator writes an instance of a structure 
for each node in the DFA.  Each of these nodes contains a single function that 
returns the next node in the running state machine for a given input character.  
This source code is later compiled together, along with a program to scan the 
input, to separate user input into terminals as part of a larger program.

```
struct Symbol {
    const char* name;
};

struct Node {
    Node* (*next)(int c);
    Symbol* accept;
    Value* (*scan)(Table*, const std::string&);
};
```

```
extern Node node0;
extern Node node1;
extern Node node2;
extern Node node3;
extern Node node4;
extern Node node5;
extern Node node6;
extern Node node7;

Node*
next0(int c) {
    if (c == '*') { return &node1; }
    if (c == '+') { return &node2; }
    if ((c >= '0') && (c <= '9')) { return &node3; }
    if (c == 't') { return &node4; }
    return nullptr;
}

Node*
next3(int c) {
    if ((c >= '0') && (c <= '9')) { return &node3; }
    return nullptr;
}

Node*
next4(int c) {
    if (c == 'e') { return &node5; }
    return nullptr;
}

Node*
next5(int c) {
    if (c == 's') { return &node6; }
    return nullptr;
}

Node*
next6(int c) {
    if (c == 't') { return &node7; }
    return nullptr;
}

Node node0 = {&next0, nullptr, nullptr};
Node node1 = {nullptr, &term3, nullptr};
Node node2 = {nullptr, &term2, nullptr};
Node node3 = {&next3, &term1, &scan1};
Node node4 = {&next4, nullptr, nullptr};
Node node5 = {&next5, nullptr, nullptr};
Node node6 = {&next6, nullptr, nullptr};
Node node7 = {nullptr, &term0, nullptr};
```

# Generated Parser Source Code  

## Grammar Rules

The first class defines the production rule of a nonterminal and it stores the 
reduce action to call when its sequence of symbols is found in the input.  The 
reduce member variable stores a function that is passed a pointer to the top of 
the stack and returns a value for the resulting nonterminal.

```
struct Rule {
    Symbol* nonterm;
    int length;
    Value* (*reduce)(Table*, Value**);
};
```

The stack stored by the parser during operation just contains pointers to a 
generic base class.  However, any complex program would have different classes 
associated with the each of the nonterminals.  To enable the user defined reduce 
functions to work with their custom types, the generator down casts the values 
to the correct type for each symbol argument passed to the reduce function.  
These helper function are automatically written for each user action and linked 
by the parser generator.  

In addition to casting to the correct type, these function also store the 
pointer in a unique pointer before passing to the functions.  Using unique 
pointers allows the end user function to take advantage of the memory management 
features provided by these pointers.  One advantage with unique pointers is
allowing the memory from an input argument to be moved to the resulting value
and save on copying cost.  This moving is very useful for compilers as most of 
the input data is combined into one large structure before processing.

```
unique_ptr<Expr>
reduce_total(Table*, unique_ptr<Expr>&);

Value*
reduce_total(Table* table, Value** values) {
    unique_ptr<Expr> E0(dynamic_cast<Expr*>(values[-1]));
    unique_ptr<Expr> R = reduce_total(table, E0);
    return R.release();
}

unique_ptr<Expr>
reduce_add_mul(Table*, unique_ptr<Expr>&, unique_ptr<Expr>&);

Value*
reduce_add_mul(Table* table, Value** values) {
    unique_ptr<Expr> E0(dynamic_cast<Expr*>(values[-3]));
    unique_ptr<Expr> E2(dynamic_cast<Expr*>(values[-1]));
    unique_ptr<Expr> R = reduce_add_mul(table, E0, E2);
    return R.release();
}

unique_ptr<Expr>
reduce_mul_int(Table*, unique_ptr<Expr>&, unique_ptr<Expr>&);

Value*
reduce_mul_int(Table* table, Value** values) {
    unique_ptr<Expr> E0(dynamic_cast<Expr*>(values[-3]));
    unique_ptr<Expr> E2(dynamic_cast<Expr*>(values[-1]));
    unique_ptr<Expr> R = reduce_mul_int(table, E0, E2);
    return R.release();
}
```

After defining the functions, the rule structures are allocated with a pointer 
to its reduce function and the associated nonterminal.  Each rule also contains 
the number of symbols passed to the reduce function.  This number is needed so 
that the final program removes the correct number of values from the top of the 
stack when reducing symbols to a new nonterminal.

```
Rule rule0 = {&nonterm0, 1, &reduce_total};
Rule rule1 = {&nonterm1, 1, nullptr};
Rule rule2 = {&nonterm1, 3, &reduce_add_mul};
Rule rule3 = {&nonterm2, 1, nullptr};
Rule rule4 = {&nonterm2, 3, &reduce_mul_int};
```

## Parser Actions

The next structures defined are for the parse table.  The parse table stores the
actions needed to read in input terminals and determine when those symbols are 
reduced to a nonterminal according to the grammar of the input language.  The 
two main actions are shift and reduce.  For shift, the program will shift the 
current terminal onto its stack.  For reduce, the program calls the reduce 
function with values from the top of the stack as arguments, and then replaces 
those values with the result returned by the reduce function.  Some reduce 
functions are marked as accepting, meaning that reading of the the input 
string of terminals is complete as the entire input was reduced to the first 
rule of the grammar.  Finally, a goto structure is also needed, as this value 
indicates the next parser state after reducing symbol to a nonterminal.  

```
struct Shift {
    Symbol* symbol;
    State*  next;
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
```

A parse state contains vectors for all of the possible actions for a given 
state.  After the program reads a new symbol, it looks through all of the 
actions of the current state to find an action for the next symbol read from
the input.  

```
struct State {
    std::vector<Shift>*  shift;
    std::vector<Reduce>* reduce;
    std::vector<Go>*     go;
};
```

All of the possible states or predefined for the compiler since the shift and 
reduce structures reference other states as targets.  With states defined, the
generator writes the shift and reduce actions for each of the states.

```
extern State state0;
extern State state1;
extern State state2;
extern State state3;
extern State state4;
extern State state5;
extern State state6;
extern State state7;

std::vector<Shift> shift0 = {{&term1, &state1}};
std::vector<Reduce> reduce0 = {};
std::vector<Shift> shift1 = {};
std::vector<Reduce> reduce1 = {{nullptr, &rule3, false}};
std::vector<Shift> shift2 = {{&term2, &state6}};
std::vector<Reduce> reduce2 = {{&endmark, &rule0, true}};
std::vector<Shift> shift3 = {{&term3, &state4}};
std::vector<Reduce> reduce3 = {{nullptr, &rule1, false}};
std::vector<Shift> shift4 = {{&term1, &state5}};
std::vector<Reduce> reduce4 = {};
std::vector<Shift> shift5 = {};
std::vector<Reduce> reduce5 = {{nullptr, &rule4, false}};
std::vector<Shift> shift7 = {{&term3, &state4}};
std::vector<Reduce> reduce7 = {{nullptr, &rule2, false}};

std::vector<Go> go0 = {{&nonterm1, &state2}, {&nonterm2, &state3}};
std::vector<Go> go6 = {{&nonterm2, &state7}};
```

## Parser States

With the actions for each state allocated, the state themselves are now defined 
with pointers to their respective actions.

```
State state0 = {&shift0, &reduce0, &go0};
State state1 = {&shift1, &reduce1, nullptr};
State state2 = {&shift2, &reduce2, nullptr};
State state3 = {&shift3, &reduce3, nullptr};
State state4 = {&shift4, &reduce4, nullptr};
State state5 = {&shift5, &reduce5, nullptr};
State state6 = {&shift0, &reduce0, &go6};
State state7 = {&shift7, &reduce7, nullptr};
```

The generator simplifies development by automatically defining parts of the 
source code required for parsing the input text.  The generator takes a user 
defined grammar and develops a table of actions that describe which functions 
to call when specific patterns are found in the input.  This source code for the 
tables and user provided functions are compiled together, along with a program 
to scan the input, to build the final program.
