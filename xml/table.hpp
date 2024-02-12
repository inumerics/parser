#ifndef table_hpp
#define table_hpp

#include "xml.hpp"

#include <stdio.h>

#include <iostream>

class HTMLTable
{
  public:
    bool read(std::istream& in);
    
  //protected:
    std::unique_ptr<Document> doc;
    
    void print_csv(std::ostream& out);
    
};

#endif
