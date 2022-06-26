/**
 * The Finite class is for non-deterministic finite automaton (NFA).  The
 * non-determinism is from the fact that multiple states might be active given
 * the input character.  While simpler to construct, keeping track of the set of
 * states results in more computation during runtime.  The Node class is
 * combines multiple NFAs into a single deterministic finite automaton (DFA).
 * Ranges within each node map an input character to an another node in the DFA.
 */
#ifndef node_hpp
#define node_hpp

#include "finite.hpp"

#include <vector>
#include <set>
#include <map>
#include <iostream>

/**
 * For the DFA, each character will appear in at most one output.  As a result,
 * there is at most one next state for each input character.  If there is no
 * output with that input character, then the term pointer of the current
 * node indicates the matched terminal.
 */
class Node {
  public:
    Node(size_t id);
    size_t id;

    /** Adding finite states of the NFA to a single DFA node. */
    void add_finite(Finite* finite);
    void add_finite(std::set<Finite*>& finites);
    
    /** Map a range of characters to the next DFA node. */
    void add_next(int first, int last, Node* next);
    Node* get_next(int c, int* last);

    /** Solving for the next states in the DFA from this state. */
    int find_next(int c, std::set<Finite*>* next, int max);
    void solve_closure();
    void solve_accept();
    
    std::set<Finite*> items;
    Term* accept;
    
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

    struct is_same {
        bool operator() (const std::unique_ptr<Node>& left,
                         const std::unique_ptr<Node>& right) const {
            return left->items < right->items;
        }
    };
    
    static bool lower(Node* left, Node* right);
};

#endif
