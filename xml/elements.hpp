#ifndef elements_hpp
#define elements_hpp

#include "tags.hpp"

/**
 * Base class for the contents inside of an XML element. The content may
 * include text, numbers, other elements, or a combination of these.
 */
class Content : public Value
{
  public:
    virtual void print(std::ostream& out, const std::string& ident) {}
};

class Contents : public Value
{
  public:
    std::vector<std::unique_ptr<Content>> contents;
};

/**
 * Character data found within an XML element.
 */
class CData : public Content
{
  public:
    std::string name;
    
    virtual void print(std::ostream& out, const std::string& ident);
};

/**
 * An XML (Extensible Markup Language) element is a fundamental building block
 * of an XML document, used to define and structure data or content within the
 * document. An XML element consists of three main parts:
 *
 *  Opening Tag: The opening tag encloses the element's name within angle
 *      brackets, and it is the starting point for defining the element.
 *  Closing Tag: The closing tag is similar to the opening tag but includes a
 *      forward slash before the element name.
 *  Content: The content is the data or information enclosed between the opening
 *      and closing tags of the element.  This content can vary widely and can
 *      include text, numbers, other elements, attributes, or any structured
 *      data that is meaningful within the context of the XML document.
 */
class Element : public Content
{
  public:
    std::unique_ptr<Tag> open;
    std::unique_ptr<Tag> close;
    std::vector<std::unique_ptr<Content>> contents;
    
    virtual void print(std::ostream& out, const std::string& ident);
};

#endif
