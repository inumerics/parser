#include "elements.hpp"

#include <iostream>
#include <sstream>
#include <memory>
using std::unique_ptr;

/**
 * Build content from characters or an element.
 */
unique_ptr<Contents>
content_text(Table* table,
             unique_ptr<Name>& E1)
{
    auto cdata = std::make_unique<CData>();
    cdata->name = std::move(E1->name);

    auto result = std::make_unique<Contents>();
    result->items.push_back(std::move(cdata));
    return result;
}

unique_ptr<Contents>
content_element(Table* table, 
                unique_ptr<Element>& E1)
{
    auto result = std::make_unique<Contents>();
    result->items.push_back(std::move(E1));
    return result;
}

unique_ptr<Contents>
content_element_text(Table* table,
                     unique_ptr<Element>& E1,
                     unique_ptr<Name>& E2) 
{
    auto result = std::make_unique<Contents>();
    result->items.push_back(std::move(E1));
    return result;
}

/**
 * Append content within an element into a vector.
 */
unique_ptr<Contents>
reduce_contents(Table* table, 
                unique_ptr<Contents>& E1) {
    return std::move(E1);
}

unique_ptr<Contents>
append_contents(Table* table,
                unique_ptr<Contents>& E1,
                unique_ptr<Contents>& E2) 
{
    auto result = std::move(E1);
    for (auto& c : E2->items) {
        result->items.push_back(std::move(c));
    }
    return result;
}

/**
 * CData
 */
void
CData::print(std::ostream& out, const std::string& ident)
{
    out << ident << name << std::endl;
}

/**
 * Reduce an element from its tags and contents.
 */
unique_ptr<Element>
reduce_empty(Table* table,
             unique_ptr<Tag>& E1,
             unique_ptr<Tag>& E2) 
{
    auto result = std::make_unique<Element>();
    return result;
}

unique_ptr<Element>
reduce_element(Table* table,
               unique_ptr<Tag>& E1,
               unique_ptr<Contents>& E2,
               unique_ptr<Tag>& E3) 
{
    auto result = std::make_unique<Element>();
    result->open     = std::move(E1);
    result->contents = std::move(E2->items);
    result->close    = std::move(E3);
    return result;
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

