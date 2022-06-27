#include "calculator.hpp"

#include <iostream>
#include <sstream>
#include <memory>
using std::unique_ptr;

/********************************************************************************
 * Types and functions of the calculator.  Each terminal can specify an
 * associated class and the name of a function that takes the matched string
 * from the input and returns a value of that class.
 */
Expr::Expr(int value):
    value(value){}

unique_ptr<Expr>
scan_num(Table* table, const std::string& text)
{
    return std::make_unique<Expr>(std::stoi(text));
}

unique_ptr<Expr>
scan_hex(Table* table, const std::string& text)
{
    std::stringstream stream;
    stream << std::hex << text;
    int num = 0;
    stream >> num;
    return std::make_unique<Expr>(num);
}

/**
 * Functions listed for the nonterminals of the grammar.  Our program will call
 * these functions when its associated rule is seen in the input.  Each of these
 * function is passed arguments for each terminal in the rule that has an
 * associated type.  Once again, if any function is not implemented, including
 * with the exact argument types, then link errors will occur when building the
 * final calculator program.
 */

unique_ptr<Expr>
reduce_add_mul(Table* table, unique_ptr<Expr>& E1,
                             unique_ptr<Expr>& E2)
{
    unique_ptr<Expr> result = std::move(E1);
    result->value += E2->value;
    return result;
}

unique_ptr<Expr>
reduce_sub_mul(Table* table, unique_ptr<Expr>& E1,
                             unique_ptr<Expr>& E2)
{
    unique_ptr<Expr> result = std::move(E1);
    result->value -= E2->value;
    return result;
}

unique_ptr<Expr>
reduce_mul_int(Table* table, unique_ptr<Expr>& E1,
                             unique_ptr<Expr>& E2)
{
    unique_ptr<Expr> result = std::move(E1);
    result->value *= E2->value;
    return result;
}

unique_ptr<Expr>
reduce_div_int(Table* table, unique_ptr<Expr>& E1,
                             unique_ptr<Expr>& E2)
{
    unique_ptr<Expr> result = std::move(E1);
    result->value /= E2->value;
    return result;
}

unique_ptr<Expr>
reduce_paren(Table* table, unique_ptr<Expr>& E1)
{
    return std::move(E1);
}

/**
 * By definition, the first rule specified in the grammar is the rule that is
 * followed by the end of the input string and therefore the last nonterminal
 * left on the stack after reading the entire input.  Upon reducing the entire
 * input string to this terminal, the final program calls its associated
 * function. For our calculator example, the last function prints the calculated
 * result of the input expression.
*/
 
unique_ptr<Expr>
reduce_total(Table* table, unique_ptr<Expr>& E1)
{
    unique_ptr<Expr> result = std::move(E1);
    std::cout << result->value << "\n";
    return result;
}

/******************************************************************************/
int
main(int argc, const char * argv[])
{
    if (argc != 2) {
        std::cerr << "Expected a single input string.\n";
        return 1;
    }

    std::stringstream in(argv[1]);
    
    Table table;
    
    std::unique_ptr<Parser> parser = Parser::build();
            
    while (true) {
        int c = in.get();
        if (c == EOF) {
            Parser::Result result = parser->scan_end(&table);
            if (result != Parser::Done) {
                std::cerr << "Unexpected end of the input.\n";
            }
            break;
        } else {
            Parser::Result result = parser->scan(&table, c);
            if (result != Parser::Ready) {
                switch (result) {
                    case Parser::Unexpected_Char:
                        std::cerr << "Unexpected character.\n";
                        break;
                    case Parser::Unexpected_Symbol:
                        std::cerr << "Unexpected symbol.\n";
                        break;
                    case Parser::Done:
                        std::cerr << "Expected end of the input.\n";
                        break;
                    default:
                        std::cerr << "Unknown Error.\n";
                        break;
                }
                break;
            }
        }
    }
        
    return 0;
}

