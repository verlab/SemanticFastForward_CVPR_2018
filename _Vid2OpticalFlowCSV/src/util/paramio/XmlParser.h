#if !defined(XmlParser_H)
#define XmlParser_H

#include "XmlUtil.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>


//////////////////////////////////////////////////////////////////////////////
// XmlParserr
//
// Purpose: used to parse a buffer that has xml formatted data

class XmlParser
{
public:

	static long _parseid;
	
	// id of parser (for debugging)
	long		_id;

	// provides string interface to buffer
	std::string		_strParse;

	// parse buffer and offsets of where we are parsing
	char *		_buffer;
	long		_parseLength;

	// current position in parse buffer
	long		_current;

	// tag, attributes and data position information
	long		_firstTagStart;
	long		_firstTagEnd;

	long		_lastTagStart;
	long		_lastTagEnd;

	long		_nameStart;
	long		_nameEnd;
	long		_attrStart;
	long		_attrEnd;
	long		_valueStart;
	long		_valueEnd;


	XmlParser ();

	// create/release
	bool create ( const char * buffer, long parseStart, long parseEnd );

	bool create ( const char * buffer, long length );

	void release ();

	// cur position methods
	long getCurrent ();

	long getParseLength ();

	long getCurLength ();

	long getOffsetLength ( long offset );


	char * getBufferPos ();

	char * getLastBufferPos ();

	char * getCurPos ();

	char * getParseState ( long & parseLength );

	// get ref to parse buffer
	std::string & str ();

	// state methods
	void reset ();

	bool isValid ();

	void resetTagPositions ( long start=-1 );
	
	void clear ();

	// parse methods
	bool parse ( const char * buffer, long parseStart, long parseEnd );

	bool parse ( const char * buffer, long parseLength );

	bool parse ();
	bool parse ( std::string & name,
				 std::string & value,
				 std::string & attributes,
				 long & current );



	// tag search methods

	bool hasNullTag ();
   bool isSpecialXMLTag();

	/////////////////////////////////////////////////////////
	// these methods are protected because the state
	// of parsing might not be properly setup, and
	// if that were so then calling these methods
	// would cause errors

protected:

	// name search methods
	bool parseName ();
	bool parseName ( std::string & name );

	// attribute search methods
	bool parseAttributes ();
	bool parseAttributes ( std::string & attributes );

	// data search methods
	bool parseValue ();

	bool parseValue ( std::string & value );

public:

	// name access methods
	char * getNamePos ();

	bool hasName ();

	long getNameLength ();


	std::string getName ();

	// attribute access methods
	char * getAttributesPos ();

	bool hasAttributes ();

	long getAttributesLength ();

	std::string getAttributes ();

	// value access methods
	char * getValuePos ();

	bool hasValue ();

	long getValueLength ();

	std::string getValue ();

	char * getValueState ( long & valueLength );

	bool valueHasTag ();

	// tag access methods
	long getTagLength ();

	long getLastTagLength ();

	bool hasTag ();

	bool hasLastTag ();

	char * getTagPos ();

	char * getLastTagPos ();

	std::string getTag ();

	// string utility methods
	long getLength ( long startPos,
			         long endPos );

	long find ( char srchChar, long offset, long length = -1 );

	long find ( char * srchStr, long offset, long length = -1 );

	long find ( std::string & srchStr, long offset, long length = -1 );

	std::string substr ( long offset, long length );
};


#endif

