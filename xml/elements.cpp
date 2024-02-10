#include "elements.hpp"

#include <iostream>
#include <sstream>
#include <memory>
using std::unique_ptr;


void
CData::print(std::ostream& out, const std::string& ident)
{
    out << ident << name << std::endl;
}

/**
 * Build content from elements or caharcters.
 */
unique_ptr<Contents>
content_name(Table* table, unique_ptr<Name>& E1) 
{
    auto result = std::make_unique<Contents>();
    auto cdata = std::make_unique<CData>();
    cdata->name = std::move(E1->name);
    
    result->contents.push_back(std::move(cdata));
    return result;
}

unique_ptr<Contents>
content_element(Table* table, unique_ptr<Element>& E1) 
{
    auto content = std::make_unique<Contents>();
    content->contents.push_back(std::move(E1));
    return content;
}

unique_ptr<Contents>
content_element_name(Table* table,
                     unique_ptr<Element>& E1,
                     unique_ptr<Name>& E2) 
{
    auto content = std::make_unique<Contents>();
    content->contents.push_back(std::move(E1));
    return content;
}

/**
 * Append content within an element into a vector.
 */
unique_ptr<Contents>
reduce_contents(Table* table, unique_ptr<Contents>& E1) {
    return std::move(E1);
}

unique_ptr<Contents>
append_contents(Table* table,
                unique_ptr<Contents>& E1,
                unique_ptr<Contents>& E2) {
    auto contents = std::move(E1);
    for (auto& c : E2->contents) {
        contents->contents.push_back(std::move(c));
    }
    return contents;
}

/**
 * Reduce an element from its tags and contents.
 */
unique_ptr<Element>
reduce_element(Table* table,
               unique_ptr<Tag>& E1,
               unique_ptr<Contents>& E2,
               unique_ptr<Tag>& E3) {
    auto elememt = std::make_unique<Element>();
    elememt->open = std::move(E1);
    elememt->contents = std::move(E2->contents);
    elememt->close = std::move(E3);
    return elememt;
}

unique_ptr<Element>
reduce_empty(Table* table,
               unique_ptr<Tag>& E1,
               unique_ptr<Tag>& E2) {
    auto elememt = std::make_unique<Element>();
    return elememt;
}









void
Element::print(std::ostream& out, const std::string& ident)
{
    std::string indented = ident + "  ";
    
    out << ident << open->name << std::endl;
    for (auto& content : contents) {
        content->print(out, indented);
    }
    out << ident << close->name << std::endl;
}

