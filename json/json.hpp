/*******************************************************************************
 * Json parser built using a parser generator.  The parser reads the grammar for
 * json format and generates action tables.  These action tables are compiled
 * along with the user defined functions to build the reader.
 */

#ifndef json_hpp
#define json_hpp

#include <string>
#include <vector>
#include <map>

/******************************************************************************/
class Value {
public:
    virtual ~Value() = default;
};

/******************************************************************************/
class JsonValue : public Value {
};

class JsonObject;
class JsonArray;

/******************************************************************************/
class JsonObject : public JsonValue {
public:
    std::map<std::string, std::unique_ptr<JsonValue>> values;
    
    bool has_key(const std::string& name) const;
    
    JsonObject* get_object(const std::string& name) const;
    JsonArray*  get_array( const std::string& name) const;
    std::string get_string(const std::string& name) const;
    bool        get_bool(  const std::string& name) const;
};

class JsonMember : public Value {
public:
    std::string name;
    std::unique_ptr<JsonValue> value;
};

/******************************************************************************/
class JsonArray : public JsonValue {
public:
    std::vector<std::unique_ptr<JsonValue>> values;
};

/******************************************************************************/
class JsonString : public JsonValue {
public:
    JsonString(const std::string& value);
    std::string value;
};

class JsonNumber : public JsonValue {
public:
    JsonNumber(double value);
    double value;
};

class JsonBool : public JsonValue {
public:
    JsonBool(bool value);
    bool value;
};

class JsonNull : public JsonValue {
};

/******************************************************************************/
class Table {
public:
    std::unique_ptr<JsonValue> value;
};

/*******************************************************************************
 * This parser generator builds tables for a shift-reduce parser.  To include
 * the shift-reduce parser in another program, this parser class maintains a
 * stack of parse states, symbols and values.  This stack is updated based on
 * the the symbol just read from the input and the actions defined in the parse
 * table.
 */
class Parser {
public:
    static std::unique_ptr<Parser> build();
    virtual ~Parser() = default;
    
    virtual bool scan(Table* table, int c) = 0;
    virtual bool scan_end(Table* table) = 0;
};

#endif
