#include "solver.hpp"

/******************************************************************************/
Solver::Solver():
    start(nullptr){}

bool
Solver::solve(Grammar& grammar)
{
    if (grammar.all.size() == 0 || grammar.all.front()->rules.size() == 0) {
        std::cerr << "Error: No grammar rules to solve.\n";
        return false;
    }
    
    grammar.solve_first();
    grammar.solve_follows(&grammar.endmark);
    
    auto state = std::make_unique<State>(states.size());
    state->add(Item(grammar.all.front()->rules.front().get(), 0, &grammar.endmark));
    state->closure();
    
    start = state.get();
    sorted.push_back(start);
    states.insert(std::move(state));
    
    std::vector<State*> checking;
    checking.push_back(start);

    while (checking.size() > 0)
    {
        State* state = checking.back();
        checking.pop_back();

        for (auto& term : grammar.terms) {
            Symbol* sym = term.second.get();
            std::unique_ptr<State> next = state->solve_next(sym, states.size());
            if (next) {
                auto found = states.insert(std::move(next));
                State* target = found.first->get();
                state->nexts[sym] = target;
                if (found.second) {
                    sorted.push_back(target);
                    checking.push_back(target);
                }
            }
        }

        for (auto& nonterm : grammar.nonterms) {
            Symbol* sym = nonterm.second.get();
            std::unique_ptr<State> next = state->solve_next(sym, states.size());
            if (next) {
                auto found = states.insert(std::move(next));
                State* target = found.first->get();
                state->nexts[sym] = target;
                if (found.second) {
                    sorted.push_back(target);
                    checking.push_back(target);
                }
            }
        }
    }

    Nonterm::Rule* rule = grammar.all.front()->rules.front().get();
    Item accept(rule, rule->product.size(), &grammar.endmark);

    for (auto& state : states) {
        std::unique_ptr<State::Actions> acts = state->solve_actions(accept);
        
        auto found = actions.insert(std::move(acts));
        state->actions = found.first->get();
        state->solve_gotos();
    }
    
    for (auto itr  = sorted.rbegin();
              itr != sorted.rend(); ++itr) {
        (*itr)->actions->id = (*itr)->id;
    }
    
    return true;
}
