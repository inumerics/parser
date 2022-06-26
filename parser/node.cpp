#include "node.hpp"

#include <algorithm>

/** Construct the Nodes of the DFA. */
Node::Node(size_t id):
    id(id),
    accept(nullptr) {}

void
Node::add_finite(Finite* finite) {
    items.insert(finite);
}

void
Node::add_finite(std::set<Finite*>& finites) {
    items.insert(finites.begin(), finites.end());
}

void
Node::add_next(int first, int last, Node* next) {
    nexts[Range(first, last)] = next;
}

Node*
Node::get_next(int c, int* last)
{
    Node* result = nullptr;
    for (auto next : nexts) {
        Range r = next.first;
        if (c >= r.first && c <= r.last) {
            result = next.second;
        }
        
        if (r.first > c && r.first < *last) {
            *last = r.first - 1;
        } else if (r.last >= c && r.last < *last) {
            *last = r.last;
        }
    }
    return result;
}

int
Node::find_next(int c, std::set<Finite*>* found, int max)
{
    int last = max;
    for (Finite* item : items) {
        int lowest = item->find_next(c, found, max);
        if (last > lowest) {
            last = lowest;
        }
    }
    return last;
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

void
Node::Range::write(std::ostream& out) const
{
    if (first == last) {
        if (isprint(first) && first != '\'' && first != '\\') {
            out << "c == '" << (char)first << "'";
        } else {
            out << "c == " << first << "";
        }
    } else {
        if (isprint(first) && isprint(last)
                && first != '\'' && last != '\''
                && first != '\\' && last != '\\') {
            out << "(c >= '" << (char)first << "')";
            out << " && ";
            out << "(c <= '" << (char)last << "')";
        } else {
            out << "(c >= " << first << ")";
            out << " && ";
            out << "(c <= " << last << ")";
        }
    }
}

