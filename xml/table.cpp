#include "table.hpp"

#include "xml.hpp"


bool
HTMLTable::read(std::istream& in)
{
    Table table;

    Calculator calculator;
    calculator.start();
    
    bool done = false;
    while (!done)
    {
        int c = in.get();
        if (c == EOF) {
            std::unique_ptr<Value> result = calculator.scan_end(&table);
            if (!result) {
                std::cerr << "Unexpected end of the input.\n";
                return false;
            }
            Document* ptr = dynamic_cast<Document*>(result.release());
            
            if (ptr) {
                doc = std::unique_ptr<Document>(ptr);
            }
                        
            std::cout << "Done\n";
            done = true;
        }
        else {
            std::cout << (char)c;
            bool ok = calculator.scan(&table, c);
            if (!ok) {
                std::cerr << "\nUnexpected character '" << (char)c <<  "'.\n";
                return false;
            }
        }
    }
    
    return true;
}

void 
HTMLTable::print_csv(std::ostream& out)
{
    Content* body = nullptr;
    for (auto& c : doc->root->contents) {
        Element* e = dynamic_cast<Element*>(c.get());
        if (e && e->open->name.compare("tr") == 0) {
            body = e;
        }
    }
}
