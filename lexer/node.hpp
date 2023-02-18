/**
 * The Node class combines multiple NFA states into a single state for building
 * a deterministic finite automaton (DFA).  Unlike a NFA, in the DFA only one
 * state is ever active and each input character leads to only one other state
 * while matching an input.  While the NFA is simpler to construct, keeping
 * track of multiple possible states results in more computation during runtime.
 */
#ifndef node_hpp
#define node_hpp

#include "finite.hpp"

#include <vector>
#include <map>

/**
 * A node of a DFA is defined by the set of finite states it contains.  For each
 * DFA node, each character will appear in at most one output.  As a result,
 * there is at most one next state for each input character.  If there is no
 * output with that input character, then the term pointer of the current
 * node indicates the matched terminal.
 */
class Node
{
public:
    std::set<Finite*> items;
    
    /** Accepted term when no new node is next. */
    Term* accept = nullptr;
    
    /** Map a range of characters to the next DFA node. */
    Node* get_next(int c, int* last);
    
    /** Solving for the next states in the DFA from this state. */
    void find_next(int c, std::set<Finite*>* next, int* last);
    void solve_closure();
    void solve_accept();
    
    /** Character range for connecting states. */
    struct Range {
        Range(int first, int last);
        int first;
        int last;
        bool operator<(const Range& other) const;
        void write(std::ostream& out) const;
    };
    
    std::map<Range, Node*> nexts;
    
    /**
     * After minimizing the number of nodes in a DFA, replaces the target nodes
     * of the ranges with the new nodes given in the map.
     */
    void replace_targets(const std::map<Node*, Node*>& prime);
    
    /** After minimizing the number of nodes, merge ranges if possible. */
    void merge_ranges();
    
    static bool lower(Node* left, Node* right);
};

/**
 * Group of nodes used in minimizing the number of states of a DFA.
 */
class Group
{
public:
    std::vector<Node*> nodes;
    
    std::vector<std::unique_ptr<Group>> divide(const std::map<Node*, Group*>& lookup);
    
private:
    bool belongs(Node* node, const std::map<Node*, Group*>& lookup);
};

#endif
