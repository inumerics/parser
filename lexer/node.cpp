#include "node.hpp"

#include <algorithm>
#include <climits>

Node*
Node::get_next(int c, int* last)
{
    Node* result = nullptr;
    for (auto next : nexts) {
        Range r = next.first;
        if (c >= r.first && c <= r.last) {
            result = next.second;
            if (r.last < *last)
                *last = r.last;
        } else if (r.first > c && r.first <= *last) {
            *last = r.first - 1;
        }
    }
    return result;
}

void
Node::find_next(int c, std::set<Finite*>* found, int* last) {
    for (Finite* item : items) {
        item->find_next(c, found, last);
    }
}

/**
 * After folliwng outputs that contain the input character, add the targets of
 * empty transitions to the newly found set of states.
 */
void
Node::solve_closure()
{
    std::vector<Finite*> stack;
    stack.insert(stack.end(), items.begin(), items.end());
    
    while (stack.size() > 0) {
        Finite* check = stack.back();
        stack.pop_back();
        check->closure(&items, &stack);
    }
}

/**
 * Since the DFA states contain multiple finite states, determine the NFA state
 * with the lowest ranked accept to represent the pattern matched by the
 * current DFA state.
 */
void
Node::solve_accept()
{
    auto lowest = std::min_element(items.begin(), items.end(), Finite::lower_rank);
    if (lowest != items.end()) {
        accept = (*lowest)->term;
    }
}

void
Node::replace_targets(const std::map<Node*, Node*>& prime)
{
    for (auto& next : nexts) {
        if (prime.count(next.second) > 0) {
            next.second = prime.at(next.second);
        }
    }
}

void
Node::merge_ranges()
{
    std::map<Node::Range, Node*> updated;
    
    auto itr = nexts.begin();
    while (itr != nexts.end()) {
        int first = itr->first.first;
        int last  = itr->first.last;
        Node* next = itr->second;
        itr++;
        
        bool matches = true;
        while (itr != nexts.end() && matches) {
            matches = itr->second == next && (last + 1) == itr->first.first;
            if (matches) {
                last = itr->first.last;
                itr++;
            }
        }
        updated[Node::Range(first, last)] = next;
    }
    nexts = updated;
}

bool
Node::lower(Node* left, Node* right)
{
    if (left->accept && right->accept) {
        return left->accept->rank < right->accept->rank;
    } else if (left->accept) {
        return true;
    } else {
        return false;
    }
}

/** Construct the Range. */
Node::Range::Range(int first, int last):
first(first), last(last) {}

bool
Node::Range::operator<(const Range& other) const {
    return last < other.first;
}

/******************************************************************************/
std::vector<std::unique_ptr<Group>>
Group::divide(const std::map<Node*, Group*>& lookup)
{
    std::vector<std::unique_ptr<Group>> result;
    
    for (auto node : nodes) {
        bool inserted = false;
        
        for (auto& add : result) {
            if (add->belongs(node, lookup)) {
                add->nodes.push_back(node);
                inserted = true;
            }
        }
        
        if (!inserted) {
            auto add = std::make_unique<Group>();
            add->nodes.push_back(node);
            result.push_back(std::move(add));
        }
    }
    
    return result;
}

/**
 * A node belong in a group if every target of the node is the same as every
 * node already in the group.  Since all nodes in a group have the same targets,
 * only compare the node to the first node of the group.
 */
bool
Group::belongs(Node* node, const std::map<Node*, Group*>& lookup)
{
    Node* other = nodes.front();
    
    int c = 0;
    int max = INT_MAX;
    
    while (true)
    {
        int last = max;
        Node* next = node->get_next(c, &last);
        Node* comp = other->get_next(c, &last);
        
        if (comp || next) {
            if (!comp || !next) {
                return false;
            } else if (lookup.at(comp) != lookup.at(next)) {
                return false;
            }
        }
        
        if (last == max) {
            break;
        } else {
            c = last + 1;
        }
    }
    
    return true;
}
