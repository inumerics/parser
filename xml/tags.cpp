#include "tags.hpp"

#include <iostream>
#include <sstream>
#include <memory>
using std::unique_ptr;
using std::make_unique;

/**
 * For terminals that specify an associated class, these functions takes the
 * matched string from the input and returns a value of that class.
 */
Name::Name(const string& name):
    name(name) {}

unique_ptr<Name>
scan_name(Table* table, const string& text) {
    return make_unique<Name>(text);
}

/**
 * Build a vector of attributes.
 */
unique_ptr<Attrs>
reduce_attrs(Table* table,
             unique_ptr<Attr>& E1) {
    auto attrs = make_unique<Attrs>();
    return attrs;
}

unique_ptr<Attrs>
append_attrs(Table* table,
             unique_ptr<Attrs>& E1,
             unique_ptr<Attr>& E2) {
    auto attrs = make_unique<Attrs>();
    return attrs;
}

unique_ptr<Attr>
reduce_attr(Table* table,
            unique_ptr<Name>& E1,
            unique_ptr<Name>& E2) {
    auto attr = make_unique<Attr>();
    attr->name = std::move(E1->name);
    attr->value = std::move(E1->name);
    return attr;
}

/**
 * Build a tag from its name and attributes.
 */
Tag::Tag(const string& name):
    name(name) {}

unique_ptr<Tag>
reduce_stag(Table* table, 
            unique_ptr<Name>& E1) {
    auto tag = make_unique<Tag>(E1->name);
    return tag;
}

unique_ptr<Tag>
reduce_stag_attrs(Table* table, 
                  unique_ptr<Name>& E1,
                  unique_ptr<Attrs>& E2) {
    auto tag = make_unique<Tag>(E1->name);
    tag->attrs = std::move(E2->items);
    return tag;
}

unique_ptr<Tag>
reduce_etag(Table* table, 
            unique_ptr<Name>& E1) {
    auto tag = make_unique<Tag>(E1->name);
    return tag;
}
