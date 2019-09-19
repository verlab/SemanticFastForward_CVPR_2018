#ifndef PARAMIO_H
#define PARAMIO_H
#pragma warning( disable : 4786 )  

#include "XmlStream.h"

#include <fstream>
#include <string>

#include "XML_Node.h"

#ifdef _PARAMIO_CSTRING_SUPPORT_
#include <afx.h>
#endif

/** Transforms an access string in a vector of names.
  *
  * This method is static.
  * The vector is cleared before adding the names.
  *
  * @param str the access string to parse
  * @param the return vector
  */
void parseAccess(const char *str, std::vector<std::string> &access);

/** A class to read/write parameters in XML format.
*
* This class doesn't handle all the possibilities
* of XML. It's just a convenient way for us to create
* text format parameter files. Moreover IE 5.0 provides
* a nice way to read XML files.
*
* You don't need to read and write parameters in the same
* order, you can put parameters that will not be read by the
* application and in case of missing ones, you can set a 
* default value when you read.
*
*/
class ParamIO
{
public:
    
/** Writing method.
*
* You should try to avoid use of char* as value type
* because it may create some problems (allocated size,...).
* Use string instead.
*
* @param str the properly formated parameter (e.g. "NODE1:NODE2:PARAM")
* @param value the input value
    */
    template<class T>
        bool write(const char *str, T value, const std::string &attribute = std::string())
    {
        std::vector<std::string> strs;
        parseAccess(str, strs);
        
        std::string att;
        if(attribute.length() > 0)
        {
                att = std::string("comment=\"") + attribute + std::string("\"");
        }
        
        return _treeBuilder.addElement(strs, value, att);
    }

    
#ifdef _PARAMIO_CSTRING_SUPPORT_
    /** Writing method with MFC string.
    *
    * @param str the properly formated parameter (e.g. "NODE1:NODE2:PARAM")
    * @param value the input value
    */
    bool write(const char *str, CString value, const std::string &attribute = std::string())
    {
        std::string text;
        text = std::string(value);  // Convert from CString to std::string.
        return write(str, text, attribute);
    }
#endif
    
    /** Reading method.
    *
    * You should try to avoid use of char* as value and
    * defaultVal types  because it may create some problems 
    * (allocated size,...). Use string instead.
    *
    * value and defaultVal have to be of same type if you want 
    * this method to compile and execute properly.
    *
    * @param str the properly formated parameter 
    *			   (e.g. "NODE1:NODE2:PARAM")
    * @param value the output value
    * @param defaultVal default value to set to value in case the 
    *					  parameter cannot be found
    */
    template<class T>
        bool read(const char *str, T &value, T defaultVal) const
    {
        std::vector<std::string> strs;
        parseAccess(str, strs);

        if(_treeBuilder.extractValue(strs, (T&)value) == true)
        {
            return true;
        }
        else
        {
            value = defaultVal;
            return false;
        }
    }
    
#ifdef _PARAMIO_CSTRING_SUPPORT_
    /** Reading method with MFC strings.
    *
    * @param str the properly formated parameter 
    *			   (e.g. "NODE1:NODE2:PARAM")
    * @param value the output value
    * @param defaultVal default value to set to value in case the 
    *					  parameter cannot be found
    */
    bool read(const char *str, CString &value, LPCSTR defaultVal) const
    {
        std::string text;
        bool bSuccess = read(str, text, std::string(defaultVal));
        value = text.c_str();
        return bSuccess;
    }
    
    bool read(const char *str, CString &value, CString defaultVal) const
    {
        return read(str, value, (LPCSTR)defaultVal);
    }
#endif
    
    /** Compare two XML trees
    *
    * str can be the path to a parameter or a sub-tree.
    * In the case of a sub-tree, the output will be true
    * if at least one parameter has been modified.
    * The default value of str (0), will perform the
    * comparison of the entire tree.
    *
    * @param str the properly formated parameter 
    *			   (e.g. "NODE1:NODE2:PARAM")
    * @return true if both trees are identical, false if not
    */
    bool compare(const ParamIO &old, const char *str = 0) const;
    
    bool operator==(const ParamIO &old) const
    {
        return compare(old);
    }
    
    bool operator!=(const ParamIO &old) const
    {
        return !compare(old);
    }
    
    /** Extract a sub-tree
    *
    * @param str     the properly formated parameter (e.g. "NODE1:NODE2:PARAM")
    * @param subtree the extracted sub-tree
    * @return true if str can be found, false in other cases
    */
    bool extract(const char *str, ParamIO &subtree) const;
    
    /** Erase a sub-tree
    *
    * @param str     the properly formated parameter (e.g. "NODE1:NODE2:PARAM")
    * @return true if str can be found and erased, false in other cases
    */
    bool erase(const char *str);
    
    
    /** Reads an XML file.
    *
    * @param filename the filename
    */
    void readFile(const char *filename);
    
    /** Writes an XML file.
    *
    * @param filename the filename
    */
    bool writeFile(const char *filename) const;
    
    /** Reads an XML stream.
    *
    * @param is the input stream
    */
    void readStream(std::istream &is);
    /** Writes an XML stream.
    *
    * @param os the output stream
    */
    void writeStream(std::ostream &os) const;
	//void writeStream(CArchive &os) const;
    
    /** Gets the XML tree.
    *
    * This method is used in the XML_Dialog class.
    * 
    * @return the pointer to the tree builder (that contains the tree)
    */
    XML_Param_Notify& getTree() 
    {
        return _treeBuilder;
    }
    
    const XML_Param_Notify& getTree() const
    {
        return _treeBuilder;
    }
    
    /** Clears the XML tree.
    *
    */
    void clear()
    {
        _treeBuilder.clear();
    }
    
private:
    
    XmlStream _xml;
    
    XML_Param_Notify _treeBuilder;
};
#endif

