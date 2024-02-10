#include "tags.hpp"

#include <iostream>
#include <sstream>
#include <memory>
using std::unique_ptr;

/**
 * For terminals that specify an associated class, these functions takes the
 * matched string from the input and returns a value of that class.
 */
unique_ptr<Name>
scan_name(Table* table, const std::string& text) {
    return std::make_unique<Name>(text);
}

/**
 * Reduces the vector of attributes.
 */
unique_ptr<Attrs>
reduce_attrs(Table* table,
               unique_ptr<Attr>& E1) {
    auto attrs = std::make_unique<Attrs>();
    return attrs;
}

unique_ptr<Attrs>
append_attrs(Table* table,
               unique_ptr<Attrs>& E1,
               unique_ptr<Attr>& E2) {
    auto attrs = std::make_unique<Attrs>();
    return attrs;
}

unique_ptr<Attr>
reduce_attr(Table* table,
               unique_ptr<Name>& E1,
               unique_ptr<Name>& E2) {
    auto attr = std::make_unique<Attr>();
    return attr;
}

/**
 * Reduce the tag and its attributes.
 */
unique_ptr<Tag>
reduce_stag(Table* table, unique_ptr<Name>& E1) {
    auto tag = std::make_unique<Tag>(E1->name);
    return tag;
}

unique_ptr<Tag>
reduce_stag_attrs(Table* table, unique_ptr<Name>& E1, unique_ptr<Attrs>& E2) {
    auto tag = std::make_unique<Tag>(E1->name);
    return tag;
}

unique_ptr<Tag>
reduce_etag(Table* table, unique_ptr<Name>& E1) {
    auto tag = std::make_unique<Tag>(E1->name);
    return tag;
}
