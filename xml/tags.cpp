#include "tags.hpp"

#include <iostream>
#include <sstream>
#include <memory>
using std::unique_ptr;
using std::make_unique;

/*******************************************************************************
 * Build a vector of attributes from a list name/value pairs.
 */
Name::Name(const string& name):
    name(name) {}

unique_ptr<Name>
scan_name(Table* table, const string& text) {
    return make_unique<Name>(text);
}

unique_ptr<Attr>
reduce_attribute(Table* table,
                 unique_ptr<Name>& E1,
                 unique_ptr<Name>& E2)
{
    auto result = make_unique<Attr>();
    result->name = std::move(E1->name);
    result->value = std::move(E2->name);
    return result;
}

unique_ptr<Attrs>
reduce_attributes(Table* table,
                  unique_ptr<Attr>& E1)
{
    auto result = make_unique<Attrs>();
    return result;
}

unique_ptr<Attrs>
append_attributes(Table* table,
                  unique_ptr<Attrs>& E1,
                  unique_ptr<Attr>& E2) 
{
    auto result = make_unique<Attrs>();
    return result;
}

/*******************************************************************************
 * Build a tag from its name and vector of attributes.
 */
Tag::Tag(const string& name):
    name(name) {}

unique_ptr<Tag>
reduce_start_tag(Table* table,
            unique_ptr<Name>& E1)
{
    auto tag = make_unique<Tag>(E1->name);
    return tag;
}

unique_ptr<Tag>
reduce_start_tag_attrs(Table* table,
                       unique_ptr<Name>& E1,
                       unique_ptr<Attrs>& E2) 
{
    auto tag = make_unique<Tag>(E1->name);
    tag->attrs = std::move(E2->items);
    return tag;
}

unique_ptr<Tag>
reduce_end_tag(Table* table,
               unique_ptr<Name>& E1)
{
    auto tag = make_unique<Tag>(E1->name);
    return tag;
}
