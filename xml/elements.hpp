/**
 * An XML (Extensible Markup Language) element is a fundamental building block
 * of an XML document.  An XML element consists of three main parts:
 *
 *  Opening Tag: The opening tag encloses the element's name within angle
 *      brackets, and it is the starting point for defining the element.
 *  Closing Tag: The closing tag is similar to the opening tag but includes a
 *      forward slash before the element name.
 *  Content: The content is the data or information enclosed between the opening
 *      and closing tags of the element.  This content can include text,
 *      numbers, and other elements.
 */

#ifndef elements_hpp
#define elements_hpp

#include "tags.hpp"

#include <string>
using std::string;
using std::vector;
using std::ostream;
using std::unique_ptr;

/**
 * Base class for the contents inside of an XML element. The content may
 * include text, numbers, other elements, or a combination of these.
 */
class Content : public Value
{
  public:
    virtual void print(ostream& out, const string& ident) {}
};

class Contents : public Value
{
  public:
    vector<unique_ptr<Content>> items;
};

/**
 * Character data found within an XML element.
 */
class CData : public Content
{
  public:
    string name;
    
    virtual void print(ostream& out, const string& ident);
};

/**
 * An XML (Extensible Markup Language) element defines and structure data or
 * content within the document.
 */
class Element : public Content
{
  public:
    unique_ptr<Tag> open;
    unique_ptr<Tag> close;
    vector<unique_ptr<Content>> contents;
    
    virtual void print(ostream& out, const string& ident);
};

#endif
