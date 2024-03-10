/**
 * An XML element consists of an opening tag, content or data, and a closing
 * tag. The opening tag defines the element's name and may also include
 * attributes. The content between the opening and closing tags represents the
 * actual information or data associated with that element.  Attributes are
 * additional pieces of information that can be associated with an XML element.
 */

#ifndef tags_hpp
#define tags_hpp

#include <string>
#include <vector>
#include <memory>
#include <map>

using std::map;
using std::string;
using std::vector;
using std::ostream;
using std::unique_ptr;

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
 * The symbol table is also passed to each user defined function for while
 * reducing the grammar rules.
 */
class Table {
  public:
};

/**
 * Name of an attribute in the name/value pairs of an opening tag.
 */
class Name : public Value {
  public:
    Name(const string& name);
    string name;
};

/**
 * Attributes provide characteristics about the element itself and convey
 * information that is not part of the content. Attributes are defined within
 * the opening tag and consist of a name-value pair.
 */
class Attr : public Value
{
  public:
    string name;
    string value;
};

class Attrs : public Value
{
  public:
    vector<Attr> items;
};

/**
 * The opening tag defines the element's name and may also include attributes.
 */
class Tag : public Value
{
  public:
    Tag(const string& name);
    string name;
    vector<Attr> attrs;
};

#endif
