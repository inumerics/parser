/*******************************************************************************
 * An XML element consists of an opening tag, content or data, and a closing
 * tag. The opening tag defines the element's name and may also include
 * attributes.  The attributes are additional pieces of information that can be
 * associated with an element.
 */

#ifndef tags_hpp
#define tags_hpp

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;
using std::unique_ptr;

/*******************************************************************************
 * Value is the base class of all arguments passed to the user defined functions
 * of the parser.  The class provides a virtual destructor for releasing
 * memory during parsing.  The symbol table is also passed to each user defined 
 * function for storing symbol names while reducing the input by grammar rules.
 */
class Value {
  public:
    virtual ~Value() = default;
};

class Table {
  public:
};

/*******************************************************************************
 * The opening tag defines the element's name and may also include attributes.
 * Attributes provide characteristics about the element itself and convey
 * information that is not part of the content. Attributes are defined within
 * the opening tag and consist of a name-value pair.
 */
class Name : public Value {
  public:
    Name(const string& name);
    string name;
};

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

class Tag : public Value
{
  public:
    Tag(const string& name);
    string name;
    vector<Attr> attrs;
};

#endif
