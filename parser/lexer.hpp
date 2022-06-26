/**
 * Builds a lexer for identifying tokens in an input string.  The lexer combines
 * multiple regular expressions into a single deterministic finite automaton
 * (DFA).  This DFA can then be written to source code and later compiled into
 * another program to identify tokens in an input string.
 */

#ifndef lexer_hpp
#define lexer_hpp

#include "literal.hpp"
#include "regex.hpp"
#include "node.hpp"

/**
 * This class provide two ways to add patterns for matching, a literal string of
 * characters to match, or a regular expression.  After adding expressions, call
 * solve to solve the nodes the DFA.  Each node matches an input character to a
 * single next node, creating a finite state machine.  When the next character
 * is not found in the ranges of a node, than the presents of an term pointer
 * indicates a match for that terminal.
 */
class Lexer
{
  public:
    Lexer();

    /** Add patterns to match in the input string. */
    bool add_regex(Term* accept, const std::string& regex);
    bool add_literal(Term* accept, const std::string& series);
    
    /** After adding all expressions, call solve to build to DFA. */
    void solve();
    
    /** After building the DFA, call reduce to minimize the nodes. */
    void reduce();
    
    /** The DFA is defined by an initial node and unique sets of NFA nodes. */
    Node* initial;
    std::set<std::unique_ptr<Node>, Node::is_same> nodes;
    std::set<Node*> primes;
 
  private:
    std::vector<std::unique_ptr<Regex>> exprs;
    std::vector<std::unique_ptr<Literal>> literals;
    
    /** Groups of nodes for minimizing the number of DFA nodes. */
    class Group {
      public:
        void insert(Node* node);

        bool belongs(Node* node, const std::set<Group>& all) const;
        std::vector<Group> divide(const std::set<Group>& PI) const;
        
        /** Find a node that represents all nodes in the group. */
        Node* represent(std::map<Node*, Node*>& replace, Node* start);

        bool operator<(const Group& other) const;
        bool operator==(const Group& other) const;

      private:
        std::set<Node*> nodes;
        static bool same_group(Node* s1, Node* s2, const std::set<Group>& all);
    };

    /** Initial partion of the nodes for finding the unique sets of nodes. */
    std::set<Group> partition();
};

#endif
