#include "lexer.hpp"

#include <algorithm>
#include <climits>
#include <sstream>
using std::cerr;

/******************************************************************************/
Lexer::Lexer():
    initial(nullptr) {}

/**
 * Builds a NFA for a user defined regular expression.  The method returns true
 * if the provided expression is valid.  Solve then combines all of these NFAs
 * into a single DFA.
 */
bool
Lexer::add_regex(Term* accept, const std::string& regex)
{
    std::unique_ptr<Regex> expr = std::make_unique<Regex>();
    
    bool ok = expr->parse(regex, accept);
    if (!ok) {
        std::cerr << "Unable to parse expression.\n";
        return false;
    }
    
    exprs.push_back(std::move(expr));
    return true;
}

bool
Lexer::add_literal(Term* accept, const std::string& series)
{
    std::unique_ptr<Literal> expr = std::make_unique<Literal>();
    
    bool ok = expr->parse(series, accept);
    if (!ok) {
        std::cerr << "Unable to parse character series.\n";
        return false;
    }
        
    literals.push_back(std::move(expr));
    return true;
}

/**
 * Converts the multiple non-deterministic finite automaton (NFA) defined by
 * regular expressions into a single deterministic finite automaton (DFA).
 * The lexer DFA is built by finding new nodes that are the possible sets of
 * finite nodes of a NFA while reading input characters.  Starting with the
 * initial set of finite nodes as the first DFA node, solve will follow
 * character ranges to new sets of nodes.  Each new found set of nodes will
 * define a new DFA node.  This searching will continue until no new sets of
 * nodes are found.
 */
void
Lexer::solve()
{
    /** Build the first node from the start node of all expressions. */
    std::unique_ptr<Node> first = std::make_unique<Node>(nodes.size());
    for (auto& expr : exprs) {
        first->add_finite(expr->start);
    }
    for (auto& expr : literals) {
        first->add_finite(expr->start);
    }
    
    first->solve_closure();
    first->solve_accept();
    initial = first.get();
    nodes.insert(std::move(first));
    
    std::vector<Node*> pending;
    pending.push_back(initial);
    
    /** While still finding new nodes. */
    while (pending.size() > 0) {
        Node* current = pending.back();
        pending.pop_back();
        
        /** Check every character for a possible new set. */
        int c = 0;
        int max = INT_MAX;
        while (true)
        {
            std::set<Finite*> found;
            int last = current->find_next(c, &found, max);
                        
            /** After searching check to see if the node was already found. */
            if (found.size() > 0) {
                auto node = std::make_unique<Node>(nodes.size());
                node->add_finite(found);
                node->solve_closure();
                
                auto inserted = nodes.insert(std::move(node));
                Node* next = inserted.first->get();
                current->add_next(c, last, next);
                
                /** Check newly found node for other possible DFA nodes. */
                if (inserted.second) {
                    next->solve_accept();
                    pending.push_back(next);
                }
            }
            
            if (last == max) {
                break;
            } else {
                c = last + 1;
            }
        }
    }
    
    for (auto& n : nodes) {
        primes.insert(n.get());
    }
}

/******************************************************************************/
void
Lexer::reduce()
{
    primes.clear();

    std::set<Group> current = partition();
    std::set<Group> found = current;

    while (true) {
        for (auto group : current) {
            std::vector<Group> divided = group.divide(current);
            found.erase(group);
            copy(divided.begin(), divided.end(), inserter(found, found.end()));
        }
        if (current == found) {
            break;
        } else {
            current = found;
        }
    }

    std::map<Node*, Node*> replacement;
    for (auto group : current) {
        Node* prime = group.represent(replacement, initial);
        primes.insert(prime);
    }
    for (Node* prime : primes) {
        prime->replace_targets(replacement);
        prime->merge_ranges();
    }
}

std::set<Lexer::Group>
Lexer::partition()
{
    std::map<Term*, Group> split;
    for (auto& node : nodes) {
        split[node->accept].insert(node.get());
    }

    std::set<Group> result;
    for (auto group : split) {
        result.insert(group.second);
    }
    return result;
}

/******************************************************************************/
void
Lexer::Group::insert(Node* node) {
    nodes.insert(node);
}

bool
Lexer::Group::belongs(Node* node, const std::set<Group>& all) const
{
    if (nodes.size() < 1) {
        return false;
    }

    Node* check = *nodes.begin();
    
    int c = 0;
    int max = INT_MAX;
    while (true)
    {
        int last_check = max;
        int last_node  = max;
        Node* check_next = check->get_next(c, &last_check);
        Node* node_next = node->get_next(c, &last_node);
        int last = last_check < last_node ? last_check : last_node;
        
        if (check_next || node_next) {
            if (!check_next || !node_next) {
                return false;
            } else if (!same_group(check_next, node_next, all)) {
                return false;
            }
        }
        if (last == max) {
            break;
        } else {
            c = last_check + 1;
        }
    }
    return true;
}

bool
Lexer::Group::same_group(Node* s1, Node* s2, const std::set<Group>& all)
{
    for (auto check : all) {
        if (check.nodes.count(s1) > 0) {
            if (check.nodes.count(s2) > 0) {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

std::vector<Lexer::Group>
Lexer::Group::divide(const std::set<Group>& PI) const
{
    std::vector<Group> result;

    for (auto node : nodes) {
        bool found = false;
        for (auto& group : result) {
            if (group.belongs(node, PI)) {
                group.nodes.insert(node);
                found = true;
                break;
            }
        }
        if (!found) {
            Group added;
            added.nodes.insert(node);
            result.push_back(added);
        }
    }
    return result;
}

Node*
Lexer::Group::represent(std::map<Node*, Node*>& replace, Node* start)
{
    Node* result = nullptr;
    for (auto s : nodes) {
        if (s == start) {
            result = s;
            break;
        }
    }
    if (!result) {
      auto lowest = std::min_element(nodes.begin(), nodes.end(), Node::lower);
        result = *lowest;
    }

    for (Node* node : nodes) {
        replace[node] = result;
    }
    return result;
}

bool
Lexer::Group::operator<(const Group& other) const {
    return nodes < other.nodes;
}

bool
Lexer::Group::operator==(const Group& other) const {
    return nodes == other.nodes;
}

