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
    std::cout << "> ";
    
    std::ifstream in;
    in.open("test.xml");

    bool done = false;
    while (!done)
    {
        int c = in.get();
        std::cout << (char)c;
        if (c == EOF) {
            std::unique_ptr<Value> result = calculator.scan_end(&table);
            if (!result) {
                std::cerr << "Unexpected end of the input.\n";
                return 1;
            }
            std::unique_ptr<Document> num(dynamic_cast<Document*>(result.release()));
            std::cout << "Done";
            done = true;
        }
        else {
            bool ok = calculator.scan(&table, c);
            if (!ok) {
                std::cerr << "Unexpected character '" << c <<  "'.\n";
                return 1;
            }
        }
    }
    
    return 0;
}

