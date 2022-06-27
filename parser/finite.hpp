/**
 * The state of a finite state machine for recognizing terminals, such as words
 * and numbers, in an input string.
 */

#ifndef finite_hpp
#define finite_hpp

#include <set>
#include <string>
#include <vector>
#include <iostream>

/**
 * Base class for the terminals and nonterminals of a grammar.
 */
class Symbol
{
  public:
    virtual ~Symbol() = default;
    std::string type;
    
    virtual void print(std::ostream& out) const = 0;
    virtual void write(std::ostream& out) const = 0;
};

/**
 * Name of a terminal to find.  The rank is required when multiple terminals
 * match a string and the one with the lowest rank is accepted.
 */
class Term : public Symbol {
  public:
    Term(const std::string& name, size_t rank);
    std::string name;
    std::string regex;
    std::string action;
    size_t rank;
    
    virtual void print(std::ostream& out) const;
    virtual void write(std::ostream& out) const;    
};

/**
 * The state machine is constructed by connecting states to each other with
 * outputs.  Each state contains a vector of outputs that defines the next
 * active state for a given input character.  While operating the machine, one
 * or more states is in a set of active states.
 */
class Finite
{
  public:
    Finite();
    Finite(Term* term);
    
    /** Pointer indicates a match when this state active. */
    Term* term;

    /** Checks an input stream for a match starting from this state. */
    Term* scan(std::istream& in, std::string* match);
    
    /**
     * If the next character is within the output range of an active state, then
     * the target state of that output is added to the new set of active states.
     * Empty, or epsilon, outputs provide optional patterns.
     */
    class Out {
      public:
        Out(int first, int last, Finite* next);
        Out(Finite* next);
        
        Finite* next;
        bool is_epsilon();
        bool in_range(char c);
        
        bool epsilon;
        int first;
        int last;
    };
    
    /** Builds and returns new outputs, but retains ownership. */
    Out* add_out(int c, Finite* next);
    Out* add_out(int first, int last, Finite* next);
    Out* add_epsilon(Finite* next);
        
    /** Finds output targets with the given character in its range. */
    void find_next(int c, std::set<Finite*>* next) const;
    int find_next(int c, std::set<Finite*>* next, int max) const;    
        
    /** Follows empty transitions until no new states are found. */
    static void closure(std::set<Finite*>* states);
    void closure(std::set<Finite*>* states, std::vector<Finite*>* stack) const;
    
    /** Determines the accepted match in cases with multiple final states. */
    static bool lower_rank(const Finite* left, const Finite* right);
    
  private:
    
    /** Next active states for a given input character. */
    std::vector<std::unique_ptr<Out>> outs;
    
};

#endif

