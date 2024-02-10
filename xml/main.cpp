#include "xml.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

/**
 * The main function reads an input one character at a time until a symbol is
 * found.  When found, the precomputed actions determine if the symbol is pushed
 * onto the stack, or if the stack is reduced by a rule of the grammar.
 */
int
main(int argc, const char * argv[])
{
    Table table;

    Calculator calculator;
    calculator.start();
    
    std::ifstream in;
    in.open("stock.xml");

    bool done = false;
    while (!done)
    {
        int c = in.get();
        if (c == EOF) {
            std::unique_ptr<Value> result = calculator.scan_end(&table);
            if (!result) {
                std::cerr << "Unexpected end of the input.\n";
                return 1;
            }
            std::unique_ptr<Document> doc(dynamic_cast<Document*>(result.release()));
            
            if (doc) {
                doc->print(std::cout);
            }
            
            std::cout << "Done\n";
            done = true;
        }
        else {
            std::cout << (char)c;
            bool ok = calculator.scan(&table, c);
            if (!ok) {
                std::cerr << "\nUnexpected character '" << (char)c <<  "'.\n";
                return 1;
            }
        }
    }
    
    return 0;
}

