#ifndef tags_hpp
#define tags_hpp

#include <string>
#include <vector>
#include <memory>
#include <map>

/**
 * Value is the base class of all arguments passed to the user defined functions
 * of the parser.  The class provides a virtual destructor for releasing
 * memory during parsing.
 */
class Value {
  public:
    virtual ~Value() = default;
};

/**
 * The symbol table is also passed to each user defined function associated with
 * a grammar rule.
 */
class Table {
  public:
    std::map<std::string, int> vars;
    bool done = false;
};

/**
 *
 */
class Name : public Value {
  public:
    Name(const std::string& name) : name(name) {}
    std::string name;
};

/**
 * Attributes are additional pieces of information that can be associated with
 * an XML element. They provide metadata or characteristics about the element
 * itself and are typically used to convey information that is not part of the
 * element's content. Attributes are defined within the opening tag of an XML
 * element and consist of a name-value pair.
 */
class Attr : public Value
{
  public:
};

class Attrs : public Value
{
  public:
    std::vector<Attr> items;
};

/**
 * An XML element consists of an opening tag, content or data, and a closing
 * tag. The opening tag defines the element's name and may also include
 * attributes that provide additional information about the element. The content
 * or data between the opening and closing tags represents the actual
 * information or data associated with that element.
 */
class Tag : public Value
{
  public:
    Tag(const std::string& name) : name(name) {}
    std::string name;
};

#endif
