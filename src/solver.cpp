#include "solver.hpp"

/******************************************************************************/
bool
Solver::solve(Grammar& grammar)
{
    if (grammar.nonterms.size() == 0 ||
            grammar.nonterms.front()->rules.size() == 0) {
        std::cerr << "Error: No grammar rules to solve.\n";
        return false;
    }
    
    Nonterm::solve_first(grammar.nonterms);
    Nonterm::solve_follows(grammar.nonterms, &grammar.endmark);
    
    Nonterm* nonterm = grammar.nonterms.front().get();
    Nonterm::Rule* rule = nonterm->rules.front().get();
    
    Item item(rule, 0, &grammar.endmark);
    
    auto state = std::make_unique<State>(states.size());
    state->items.insert(item);
    state->closure();
    
    std::vector<State*> checking;
    checking.push_back(state.get());
    
    states.push_back(std::move(state));
    
    while (checking.size() > 0)
    {
        State* state = checking.back();
        checking.pop_back();
        
        for (auto& term : grammar.terms) {
            Symbol* sym = term.get();
            std::unique_ptr<State> next = state->solve_next(sym, states.size());
            if (next) {
                State* target = nullptr;
                for (auto& s : states) {
                    if (next->items == s->items) {
                        target = s.get();
                        break;
                    }
                }
                if (!target) {
                    target = next.get();
                    states.push_back(std::move(next));
                    checking.push_back(target);
                }
                state->nexts[sym] = target;
            }
        }
        
        for (auto& nonterm : grammar.nonterms) {
            Symbol* sym = nonterm.get();
            std::unique_ptr<State> next = state->solve_next(sym, states.size());
            if (next) {
                State* target = nullptr;
                for (auto& s : states) {
                    if (next->items == s->items) {
                        target = s.get();
                        break;
                    }
                }
                if (!target) {
                    target = next.get();
                    states.push_back(std::move(next));
                    checking.push_back(target);
                }
                state->nexts[sym] = target;
            }
        }
    }
    
    Item accept(rule, rule->product.size(), &grammar.endmark);
    
    for (auto& state : states) {
        std::unique_ptr<State::Actions> acts = state->solve_actions(accept);
        
        State::Actions* found = nullptr;
        for (auto& a : actions) {
            if (a->is_same(acts)) {
                found = a.get();
                break;
            }
        }
        if (!found) {
            found = acts.get();
            actions.push_back(std::move(acts));
        }
        state->actions = found;
        state->solve_gotos();
    }
    
    for (auto itr  = states.rbegin();
         itr != states.rend(); ++itr) {
        (*itr)->actions->id = (*itr)->id;
    }
    
    return true;
}
