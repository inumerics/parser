#include "lexer.hpp"

#include <algorithm>
#include <climits>

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
    auto first = std::make_unique<Node>();
    for (auto& expr : exprs) {
        if (expr->start) {
            first->items.insert(expr->start);
        }
    }
    for (auto& expr : literals) {
        if (expr->start) {
            first->items.insert(expr->start);
        }
    }
    
    first->solve_closure();
    first->solve_accept();
    Node* initial = first.get();
    nodes.push_back(std::move(first));
    
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
            int last = max;
            std::set<Finite*> found;
            current->find_next(c, &found, &last);
            
            /** After searching check to see if the node was already found. */
            if (found.size() > 0) {
                auto node = std::make_unique<Node>();
                node->items.insert(found.begin(), found.end());
                node->solve_closure();
                node->solve_accept();
                
                bool exists = false;
                
                for (auto& check : nodes) {
                    if (check->items == node->items) {
                        current->nexts[Node::Range(c, last)] = check.get();
                        exists = true;
                    }
                }
                
                if (!exists) {
                    current->nexts[Node::Range(c, last)] = node.get();
                    pending.push_back(node.get());
                    nodes.push_back(std::move(node));
                }
            }
            
            if (last == max) {
                break;
            } else {
                c = last + 1;
            }
        }
    }
}

/**
 * Initial partion of the nodes for finding the unique sets of nodes.
 */
std::vector<std::unique_ptr<Group>>
initial_partition(const std::vector<std::unique_ptr<Node>>& nodes)
{
    std::map<Term*, std::unique_ptr<Group>> split;
    
    for (auto& node : nodes) {
        auto found = split.find(node->accept);
        if (found != split.end()) {
            found->second->nodes.push_back(node.get());
        } else {
            auto group = std::make_unique<Group>();
            group->nodes.push_back(node.get());
            split.emplace(node->accept, std::move(group));
        }
    }
    
    std::vector<std::unique_ptr<Group>> result;
    
    for (auto& group : split) {
        result.push_back(std::move(group.second));
    }
    
    return result;
}

void
Lexer::reduce()
{
    std::vector<std::unique_ptr<Group>> groups = initial_partition(nodes);
    
    std::map<Node*, Group*> lookup;
    
    for (auto& g : groups) {
        for (auto n : g->nodes) {
            lookup[n] = g.get();
        }
    }
    
    bool adding = true;
    
    while (adding) {
        adding = false;
        
        for (auto itr = groups.begin(); itr != groups.end(); ++itr) {
            std::vector<std::unique_ptr<Group>> added = (*itr)->divide(lookup);
            if (added.size() > 1) {
                groups.erase(itr);
                
                for (auto& g : added) {
                    for (auto n : g->nodes) {
                        lookup[n] = g.get();
                    }
                    groups.push_back(std::move(g));
                }
                adding = true;
                break;
            }
        }
    };
    
    std::map<Node*, Node*> replacement;
    
    std::vector<Node*> keep;
    
    for (auto& group : groups) {
        Node* initial = nodes.front().get();
        auto found = std::find(group->nodes.begin(), group->nodes.end(), initial);
        if (found == group->nodes.end()) {
            found = group->nodes.begin();
        }
        Node* prime = *found;
        keep.push_back(prime);
        for (auto node : group->nodes) {
            replacement[node] = prime;
        }
    }
    
    std::vector<std::unique_ptr<Node>> reduced;
    for (auto& node : nodes) {
        auto found = std::find(keep.begin(), keep.end(), node.get());
        if (found != keep.end()) {
            reduced.push_back(std::move(node));
        }
    }
    
    nodes = std::move(reduced);
    
    for (auto& prime : nodes) {
        prime->replace_targets(replacement);
        prime->merge_ranges();
    }
}
