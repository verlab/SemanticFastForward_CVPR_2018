#include "stdafx.h"
#include "XML_Node.h"
#include "ParamIO.h"

// Helper classes
class FindElement
{
public:

   FindElement(const std::string &str) : _searchStr(str)
   {
   }

   bool operator()(const Element &el) const
   {
      return el.name == _searchStr;
   }

   std::string _searchStr;
};

class FindNode
{
public:
   FindNode(const std::string &name):
      _name(name)
   {
   }

   bool operator()(const XML_Node &node)
   {
      return _name.compare(node.getName()) == 0?true:false;
   }

   std::string _name;
};


//XML_Node
XML_Node::XML_Node()
{
}

XML_Node::XML_Node(const std::string &name, const std::string &attributes, nodes_iterator parent) :
_name(name),
_attributes(attributes),
_parent(parent)
{
}

void XML_Node::set(const std::string &name, const std::string &attributes, nodes_iterator parent)
{
    _name = name;
    _attributes = attributes;
    _parent = parent;
}

void XML_Node::set(const std::string &name, const std::string &attributes)
{
    _name = name;
    _attributes = attributes;
}

XML_Node::elements_const_iterator XML_Node::findElement(const std::string &str) const
{
    return std::find_if(_elements.begin(), _elements.end(), FindElement(str));
}

XML_Node::elements_iterator XML_Node::findElement(const std::string &str)
{
    return std::find_if(_elements.begin(), _elements.end(), FindElement(str));
}

void XML_Node::addElement(const std::string &name, const std::string &value, const std::string &attributes)
{
    Element el(name, value, attributes);
    elements_iterator it = std::find_if(_elements.begin(), _elements.end(), FindElement(name));
    
    if(it == _elements.end())
    {
        _elements.push_back(el);
    }
    else
    {
        *it = el;
    }
}

XML_Node::nodes_iterator XML_Node::addNode(nodes_iterator &parent, const std::string &name, const std::string &attributes)
{
    XML_Node newNode(name, attributes, parent);
    _nodes.push_back(newNode);
    return _nodes.begin() + (_nodes.size() - 1);
}

bool XML_Node::getElement(const std::string &name, std::string &value, std::string &attributes)
{
    elements_iterator it = findElement(name);
    
    if(it != _elements.end())
    {
        value      = it->value;
        attributes = it->attributes;
        return true;
    }
    
    return false;
}

bool XML_Node::setElementValue(const std::string &name, const std::string &value)
{
    elements_iterator it = findElement(name);
    
    if(it != _elements.end())
    {
        it->value = value;
        return true;
    }
    
    return false;
}

XML_Node::elements_const_iterator XML_Node::beginElements() const
{
    return _elements.begin();
}

XML_Node::elements_const_iterator XML_Node::endElements() const
{
    return _elements.end();
}

XML_Node::nodes_const_iterator XML_Node::beginNodes() const
{
    return _nodes.begin();
}

XML_Node::nodes_const_iterator XML_Node::endNodes() const
{
    return _nodes.end();
}

XML_Node::nodes_iterator XML_Node::beginNodes()
{
    return _nodes.begin();
}

XML_Node::nodes_iterator XML_Node::endNodes()
{
    return _nodes.end();
}

const std::string &XML_Node::getName() const
{
    return _name;
}

bool XML_Node::extractValue(const std::string &str, std::string &value) const
{
    elements_const_iterator it = std::find_if(_elements.begin(), _elements.end(), FindElement(str));
    
    if(it == endElements())
    {
        return false;
    }
    
    value = it->value;   
    return true;
}

XML_Node::nodes_iterator XML_Node::getParent()
{
    return _parent;
}

//void XML_Node::print(CArchive &os) const
//{
//	static int nTabCount = 1;
//	int i;
//
//	CString tab="\t", leftangle="<", rightangle=">";
//	for(i=1; i < nTabCount; i++)
//	{
//		os << tab;
//	}
//
//	nTabCount++;
//
//	os << leftangle << _name.c_str();
//	if(_attributes.length() > 0)
//	{
//		os << " " << _attributes.c_str() << " ";
//	}
//
//	os << ">" << "\n";
//
//	for(elements_const_iterator it = beginElements(); it != endElements(); it++)
//	{
//		for(i=1; i < nTabCount; i++)
//		{
//			os << "\t";
//		}
//
//		os << "<" << it->name.c_str();
//		if(it->attributes.length() > 0)
//		{
//			os << " " << it->attributes.c_str() << " ";
//		}
//		os << ">";
//		os << it->value.c_str();
//		os << "</" << it->name.c_str() << ">" << "\n";
//	}
//
//	for(nodes_const_iterator it2 = beginNodes(); it2 != endNodes(); it2++)
//	{
//		it2->print(os);
//	}
//
//	for(i=1; i < (nTabCount-1); i++)
//	{
//		os << "\t";
//	}
//
//	os << "</" << _name.c_str() << ">" << "\n";
//
//	nTabCount--;
//}

void XML_Node::print(std::ostream &os) const
{
    static int nTabCount = 1;
    int i;
    
    for(i=1; i < nTabCount; i++)
    {
        os << "\t";
    }
    
    nTabCount++;
    
    os << "<" << _name;
    if(_attributes.length() > 0)
    {
        os << " " << _attributes << " ";
    }
    
    os << ">" << std::endl;
    
    for(elements_const_iterator it = beginElements(); it != endElements(); it++)
    {
        for(i=1; i < nTabCount; i++)
        {
            os << "\t";
        }
        
        os << "<" << it->name;
        if(it->attributes.length() > 0)
        {
            os << " " << it->attributes << " ";
        }
        os << ">";
        os << it->value;
        os << "</" << it->name << ">" << std::endl;
    }
    
    for(nodes_const_iterator it2 = beginNodes(); it2 != endNodes(); it2++)
    {
        it2->print(os);
    }
    
    for(i=1; i < (nTabCount-1); i++)
    {
        os << "\t";
    }
    
    os << "</" << _name << ">" << std::endl;
    
    nTabCount--;
}

void XML_Node::clear()
{
    _name = std::string();
    _attributes = std::string();
    _elements.clear();
    _nodes.clear();
}


XML_Node::~XML_Node()
{
    clear();
}

XML_Node::nodes_const_iterator XML_Node::findNode(const std::string &name) const
{
    return std::find_if(beginNodes(), endNodes(), FindNode(name));
}

bool XML_Node::eraseElement(const std::string &name)
{
    elements_iterator it = std::find_if(_elements.begin(), _elements.end(), FindElement(name));
    
    if(it != _elements.end())
    {
        _elements.erase(it);
        return true;
    }
    
    return false;    
}

bool XML_Node::eraseNode(const std::string &name)
{
    // Find if we want to delete a node or an element
    XML_Node::nodes_const_iterator it1   = findNode(name);
    XML_Node::nodes_const_iterator begin = beginNodes();
    
    XML_Node::nodes_iterator it = beginNodes();
    std::advance(it, std::distance(begin, it1));
    
    if(it != endNodes())
    {
        _nodes.erase(it);
        return true;
    }
    
    return false;
}

// XML_Param_Notify
XML_Param_Notify::XML_Param_Notify():
_empty(true)
{
    _node.resize(1);
}

XML_Param_Notify::~XML_Param_Notify()
{
    clear();
}

void XML_Param_Notify::clear()
{
    _node[0].clear();
    _empty = true;
}

bool XML_Param_Notify::getNode(std::vector<std::string> &strs, XML_Node::nodes_const_iterator &it) const
{
    XML_Node::nodes_const_iterator end, begin;
    
    begin = _node.begin();
    end   = _node.end();
    it = begin;
    for(unsigned int i=0; i<strs.size(); i++)
    {
        it = std::find_if(begin, end, FindNode(strs[i]));
        if(it == end)
        {
            // Couldn't find the proper parameter
            return false; 
        }
        begin = it->beginNodes();
        end   = it->endNodes();
    }
    
    return true;
}

bool XML_Param_Notify::getNode(std::vector<std::string> &strs, XML_Node::nodes_iterator &it)
{
    XML_Node::nodes_iterator end, begin;
    
    begin = _node.begin();
    end   = _node.end();
    it = begin;
    for(unsigned int i=0; i<strs.size(); i++)
    {
        it = std::find_if(begin, end, FindNode(strs[i]));
        if(it == end)
        {
            // Couldn't find the proper parameter
            return false; 
        }
        begin = it->beginNodes();
        end   = it->endNodes();
    }
    
    return true;
}

XML_Node::nodes_iterator XML_Param_Notify::findFinalNode(std::vector<std::string> &strs)
{
    XML_Node::nodes_iterator end, begin;
    XML_Node::nodes_iterator res, prevRes;
    unsigned int i;
    
    begin = _node.begin();
    end   = _node.end();
    bool go_out = false;
  
    res = prevRes = _node.end();
    for(i=0; i<strs.size()-1 && go_out==false; i++)
    {
        prevRes = res;
        
        res = std::find_if(begin, end, FindNode(strs[i]));
        if(res == end)
        {
            // Couldn't find the proper parameter
            go_out = true;
        }
        else
        {
            begin = res->beginNodes();
            end   = res->endNodes();
        }
    }
    
    // Do we need to create any node?
    if(go_out == true)
    {
        i--;
        // We need to create new nodes
        for(;i<strs.size()-1; i++)
        {
            if(prevRes == _node.end())
            {
                // It means the tree is empty
                _node[0].set(strs[i], std::string());
                prevRes = _node.begin();
            }
            else
            {
                prevRes = prevRes->addNode(prevRes, strs[i], std::string());
            }
        }
    }
    else
    {
        prevRes = res;
    }
    
    return prevRes;
}

void XML_Param_Notify::print(std::ostream &os) const
{
    _node[0].print(os);
}

//void XML_Param_Notify::print(CArchive &os) const
//{
//	_node[0].print(os);
//}

// notify methods
void XML_Param_Notify::foundNode( std::string & name, std::string & attributes )
{
    if(_empty == false)
    {
        _currentNode = _currentNode->addNode(_currentNode, name, attributes);
    }
    else
    {
        _node[0].set(name, attributes);
        _currentNode = _node.begin();
        _empty = false;
    }
}

void XML_Param_Notify::endNode( std::string & /*name*/, std::string & /*attributes*/ )
{
    _currentNode = _currentNode->getParent();
}

void XML_Param_Notify::foundElement( std::string & name, std::string & value, std::string & attributes )
{
    _currentNode->addElement(name, value, attributes);
}

bool XML_Param_Notify::compare(const ParamIO &old, std::vector<std::string> &strs) const
{
    // First we need to check if it's a subtree or an element
    XML_Node::nodes_const_iterator res;
    
    if(getNode(strs, res) == true)
    {
        // Not an existing node, it's either an element, either a wrong access
        return compareSubTree(old, strs);
    }
    else
    {
        return compareElement(old, strs);
    }
    
    // Useless
    //return false;
}

bool XML_Param_Notify::compareElement(const ParamIO &old, std::vector<std::string> &strs) const
{
    // First we need to find the final node
    std::string paramName = strs.back();
    strs.pop_back();
    XML_Node::nodes_const_iterator  it;
    bool ok = getNode(strs, it);
    XML_Node::nodes_const_iterator  itOld;
    bool okOld = old.getTree().getNode(strs, itOld);
    
    if(ok == false && okOld == false)
    {
        // The element doesn't exist in both trees = >they are identical
        return true;
    }
    
    if(ok == false ||  okOld == false)
    {
        // The element exists only in one tree => they are different
        return false;
    }
    
    std::string value, oldValue;
    bool ret = it->extractValue(paramName, value);
    bool retOld = itOld->extractValue(paramName, oldValue);
    
    if(ret == false && retOld == false)
    {
        // The node doesn't exist in both cases
        return true;
    }
    
    if(ret == false || retOld == false)
    {
        // Only one of them doesn't have this node
        return false;
    }
    
    if(value.compare(oldValue) == 0)
    {
        return true;
    }
    
    return false;
}

// TODO
bool XML_Param_Notify::compareSubTree(const ParamIO &old, std::vector<std::string> &strs) const
{
    XML_Node::nodes_const_iterator  it;
    bool ok = getNode(strs, it);
    XML_Node::nodes_const_iterator  itOld;
    bool okOld = old.getTree().getNode(strs, itOld);
    
    if(ok == false &&  okOld == false)
    {
        // The subtree doesn't exist in both trees = >they are identical
        return true;
    }
    
    if(ok == false ||  okOld == false)
    {
        // The subtree exists only in one tree => they are different
        return false;
    }
    
    // Now compare subtrees
    
    // 1st compare the elements
    if(compareAllElements(it, itOld) == false)
    {
        return false;
    }
    
    // 2nd compare the children nodes
    if(compareAllChildren(it, itOld) == false)
    {
        return false;
    }
    
    return true;
}

bool XML_Param_Notify::compareAllElements(XML_Node::nodes_const_iterator  it, XML_Node::nodes_const_iterator  itOld) const
{
    // Check if they have the same number of elements
    int nbElem    = (int)std::distance(it->beginElements(),    it->endElements());
    int nbElemOld = (int)std::distance(itOld->beginElements(), itOld->endElements());
    
    if(nbElem != nbElemOld)
    {
        return false;
    }
    
    if(nbElem == 0)
    {
        // They both have no element
        return true;
    }
    
    // Compare the values of the elements
    for(XML_Node::elements_const_iterator elemIt = it->beginElements(); elemIt != it->endElements(); elemIt++)
    {
        std::string value = elemIt->value;
        std::string valueOld;
        
        bool res = itOld->extractValue(elemIt->name, valueOld);
        
        if(res == false || value.compare(valueOld)!=0)
        {
            return false;
        }
    }
    
    return true;
}

bool XML_Param_Notify::compareAllChildren(XML_Node::nodes_const_iterator  it, XML_Node::nodes_const_iterator  itOld) const
{
    // Check if they have the same number of elements
    int nb    = (int)std::distance(it->beginNodes(),    it->endNodes());
    int nbOld = (int)std::distance(itOld->beginNodes(), itOld->endNodes());
    
    if(nb != nbOld)
    {
        return false;
    }
    
    if(nb == 0)
    {
        // They both have no element
        return true;
    }
    
    // Compare the children
    for(XML_Node::nodes_const_iterator nodeIt = it->beginNodes(); nodeIt != it->endNodes(); nodeIt++)
    {
        XML_Node::nodes_const_iterator nodeItOld = itOld->findNode(nodeIt->getName());
        
        if(nodeItOld == itOld->endNodes())
        {
            // One node exists in it and not itOld
            return false;
        }
        
        if(compareAllElements(nodeIt, nodeItOld) == false)
        {
            // Subtree are not identical
            return false;
        }
        
        if(compareAllChildren(nodeIt, nodeItOld) == false)
        {
            // Subtree are not identical
            return false;
        }
    }
    
    return true;
}

////////// Extract 
bool XML_Param_Notify::extractSubTree(std::vector<std::string> &strs, ParamIO &subtree) const
{
    // First we need to check if it's a subtree or an element
    XML_Node::nodes_const_iterator  res;
    
    if(getNode(strs, res) == true)
    {
        *(subtree.getTree().root()) = (*res);
        return true;
    }
    
    // We didn't get a node (either element or wrong access)
    return false;
}

////////// Erase 
bool XML_Param_Notify::eraseSubTree(std::vector<std::string> &strs)
{
    std::string lastAccess = strs.back();
    strs.pop_back();
    
    // First we need to check if it's a subtree or an element
    XML_Node::nodes_iterator	res;
    
    if(getNode(strs, res) == true)
    {
        if(res->eraseElement(lastAccess) == true)
        {
            return true;
        }
        
        if(res->eraseNode(lastAccess) == true)
        {
            return true;
        }
    }
    
    return false;
}

