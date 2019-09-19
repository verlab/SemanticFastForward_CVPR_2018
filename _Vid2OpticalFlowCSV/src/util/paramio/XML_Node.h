#ifndef XML_NODE_H
#define XML_NODE_H

#pragma warning (disable : 4786)

#include <string>
#include <vector>
#include <algorithm>

#include "XmlNotify.h"
class ParamIO;

class Element
{
public:
    
    Element()
    {
    }
    
    Element(const std::string &n, const std::string &v, const std::string &a):
        name(n),
        value(v),
        attributes(a)
    {
    }
    
    std::string name;
    std::string value;
    std::string attributes;
};


class XML_Node
{
public:
    
    typedef std::vector<XML_Node>::const_iterator nodes_const_iterator;
    typedef std::vector<XML_Node>::iterator nodes_iterator;
    
    typedef std::vector< Element >::iterator elements_iterator;
    typedef std::vector< Element >::const_iterator elements_const_iterator;
    
    XML_Node();
    
    XML_Node(const std::string &name, const std::string &attributes, nodes_iterator parent);
    
    void set(const std::string &name, const std::string &attributes, nodes_iterator parent);
    void set(const std::string &name, const std::string &attributes);
    
    void addElement(const std::string &name, const std::string &value, const std::string &attributes);
    nodes_iterator addNode(nodes_iterator &parent, const std::string &name, const std::string &attributes);
    
    bool getElement(const std::string &name, std::string &value, std::string &attributes);
    bool setElementValue(const std::string &name, const std::string &value);
    
    elements_const_iterator beginElements() const;
    elements_const_iterator endElements() const;
    
    nodes_const_iterator beginNodes() const;
    nodes_const_iterator endNodes() const;
    
    nodes_iterator beginNodes();
    nodes_iterator endNodes();
    
    nodes_const_iterator findNode(const std::string &name) const;
    
    const std::string &getName() const;
    
    template<class T>
        bool extractValue(const std::string &str, T &value) const
    {
        elements_const_iterator it = findElement(str);
        if(it == endElements())
        {
            return false;
        }
        
        std::stringstream ist(it->value);
        ist >> value;
        
        return ist.fail()==0;
    }
    
    // We need a specific method in std::string case so that
    // we can get space-separated words
    bool extractValue(const std::string &str, std::string &value) const;
    
    nodes_iterator getParent();
    
    void print(std::ostream &os) const;
	//void print(CArchive &os) const;
    
    void clear();
    
    bool eraseElement(const std::string &name);
    bool eraseNode(const std::string &name);
    
    
    ~XML_Node();
    
private:
    
    elements_const_iterator findElement(const std::string &str) const;
    elements_iterator findElement(const std::string &str);
    
    std::vector< Element > _elements;
    std::string _name, _attributes;
    
    // In case we have child node
    std::vector<XML_Node> _nodes;
    
    // Parent
    nodes_iterator _parent;
};

class XML_Param_Notify : public XmlNotify
{
public:
    
    XML_Param_Notify();
    
    virtual ~XML_Param_Notify();
    
    void clear();
    
    XML_Node::nodes_iterator root()
    {
        return _node.begin();
    }
    
    template<class T>
        bool extractValue(std::vector<std::string> strs, T &value) const
    {
        // First we need to find the final node
        std::string paramName = strs.back();
        strs.pop_back();
        
        XML_Node::nodes_const_iterator res;
        
        if(getNode(strs, res) == false)
        {
            return false;
        }
        
        // We found the last node, find the element
        return res->extractValue(paramName, value);
    }
    
    bool getNode(std::vector<std::string> &strs, XML_Node::nodes_const_iterator &it) const;
    bool getNode(std::vector<std::string> &strs, XML_Node::nodes_iterator &it);
    
    XML_Node::nodes_iterator findFinalNode(std::vector<std::string> &strs);
    
    template<class T>
        bool	addElement(std::vector<std::string> &strs, T &value, const std::string &attribute = std::string())
    {
        std::stringstream stream;
        stream << value;
        
        XML_Node::nodes_iterator finalNode = findFinalNode(strs);
        finalNode->addElement(strs.back(), stream.str(), attribute);

        _empty = false;
        
        return true;
    }
    
    bool compare(const ParamIO &old, std::vector<std::string> &strs) const;
    
    bool extractSubTree(std::vector<std::string> &strs, ParamIO &subTree) const;
    
    bool eraseSubTree(std::vector<std::string> &strs);
    
    void print(std::ostream &os) const;
	//void print(CArchive &os) const;
    
    // notify methods
    virtual void foundNode( std::string & name, std::string & attributes );
    virtual void endNode( std::string & name, std::string & attributes );
    
    virtual void foundElement( std::string & name, std::string & value, std::string & attributes );
    
private:
    
    bool compareElement(const ParamIO &old, std::vector<std::string> &strs) const;
    bool compareSubTree(const ParamIO &old, std::vector<std::string> &strs) const;
    
    bool compareAllElements(XML_Node::nodes_const_iterator  it, XML_Node::nodes_const_iterator  itOld) const;
    bool compareAllChildren(XML_Node::nodes_const_iterator  it, XML_Node::nodes_const_iterator  itOld) const;
    
    // Will contain only the root node, but is OK with the design
    std::vector<XML_Node> _node;
    
    XML_Node::nodes_iterator _currentNode;
    bool _empty;
};

#endif

