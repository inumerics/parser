#include "json.hpp"

#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>
using std::unique_ptr;

/******************************************************************************/
JsonBool::JsonBool(bool value): value(value) {}

JsonNumber::JsonNumber(double value): value(value) {}

JsonString::JsonString(const std::string& value): value(value) {}

/******************************************************************************/
bool
JsonObject::has_key(const std::string& name) const {
    return (values.count(name) > 0);
}

JsonObject*
JsonObject::get_object(const std::string& name) const{
    if (values.count(name) > 0) {
        return dynamic_cast<JsonObject*>(values.at(name).get());
    } else {
        return nullptr;
    }
}

JsonArray*
JsonObject::get_array(const std::string& name) const {
    if (values.count(name) > 0) {
        return dynamic_cast<JsonArray*>(values.at(name).get());
    } else {
        return nullptr;
    }
}

std::string
JsonObject::get_string(const std::string& name) const
{
    if (values.count(name) > 0) {
        JsonString* str = dynamic_cast<JsonString*>(values.at(name).get());
        if (str) {
            return str->value;
        }
    }
    return "";
}

bool
JsonObject::get_bool(const std::string& name) const
{
    if (values.count(name) > 0) {
        JsonBool* value = dynamic_cast<JsonBool*>(values.at(name).get());
        if (value) {
            return value->value;
        }
    }
    return false;
}

/******************************************************************************/
unique_ptr<JsonBool>
read_true(Table* table, const std::string& text) {
    return std::make_unique<JsonBool>(true);
}

unique_ptr<JsonBool>
read_false(Table* table, const std::string& text) {
    return std::make_unique<JsonBool>(false);
}

unique_ptr<JsonNull>
read_null(Table* table, const std::string& text) {
    return std::make_unique<JsonNull>();
}

unique_ptr<JsonValue>
read_num(Table* table, const std::string& text) {
    return std::make_unique<JsonNumber>(std::stoi(text));
}

unique_ptr<JsonString>
read_string(Table* table, const std::string& text)
{
    if (text.size() > 2) {
        return std::make_unique<JsonString>(text.substr(1, text.size() - 2));
    } else {
        return std::make_unique<JsonString>("");
    }
}

/******************************************************************************/
unique_ptr<JsonValue>
reduce_json(Table* table, unique_ptr<JsonValue>& E1) {
    table->value = std::move(E1);
    return std::make_unique<JsonValue>();
}

unique_ptr<JsonValue>
value_object(Table* table, unique_ptr<JsonObject>& E1) {
    return std::move(E1);
}

unique_ptr<JsonValue>
value_array(Table* table, unique_ptr<JsonArray>& E1) {
    return std::move(E1);
}

unique_ptr<JsonValue>
value_string(Table* table, unique_ptr<JsonString>& E1) {
    return std::move(E1);
}

unique_ptr<JsonObject>
object_empty(Table* table) {
    return std::make_unique<JsonObject>();
}

unique_ptr<JsonObject>
object_members(Table* table, unique_ptr<JsonObject>& E1) {
    return std::move(E1);
}

unique_ptr<JsonObject>
members_reduce(Table* table, unique_ptr<JsonMember>& E1) {
    auto result = std::make_unique<JsonObject>();
    result->values[E1->name] = std::move(E1->value);
    return result;
}

unique_ptr<JsonObject>
members_member(Table* table, unique_ptr<JsonObject>& E1,
               unique_ptr<JsonMember>& E2) {
    E1->values[E2->name] = std::move(E2->value);
    return std::move(E1);
}

unique_ptr<JsonMember>
member_reduce(Table* table, unique_ptr<JsonString>& E1,
              unique_ptr<JsonValue>& E2) {
    auto result = std::make_unique<JsonMember>();
    result->name = std::move(E1->value);
    result->value = std::move(E2);
    return result;
}

unique_ptr<JsonArray>
array_empty(Table* table) {
    return std::make_unique<JsonArray>();
}

unique_ptr<JsonArray>
array_values(Table* table, unique_ptr<JsonArray>& E1) {
    return std::move(E1);
}

unique_ptr<JsonArray>
values_reduce(Table* table, unique_ptr<JsonValue>& E1) {
    auto result = std::make_unique<JsonArray>();
    result->values.push_back(std::move(E1));
    return result;
}

unique_ptr<JsonArray>
values_value(Table* table, unique_ptr<JsonArray>& E1,
             unique_ptr<JsonValue>& E2) {
    E1->values.push_back(std::move(E2));
    return std::move(E1);
}
