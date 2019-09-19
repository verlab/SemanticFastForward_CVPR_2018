#include "stdafx.h"
#include "XmlParser.h"

XmlParser::XmlParser () :
_strParse()
{ 
	clear(); 

	// get parser id
	_id = _parseid;

	// next id
	_parseid++;
}

// create/release
bool XmlParser::create ( const char * buffer, long parseStart, long parseEnd )
{
	// if invalid stop
	if ( !buffer || parseStart < 0 || parseEnd < parseStart )
		return false;

	return create( buffer + parseStart, getLength(parseStart,parseEnd) );
}	



bool XmlParser::create ( const char * buffer, long length )
{
	// if invalid stop
	if ( !buffer || length <= 0 )
		return false;

	// parse buffer and offsets of where we are parsing
	_buffer      = (char *) buffer;
	_parseLength = length;

	// current position in parse buffer
	_current    = 0;

	return true;
}	

void XmlParser::release ()
{

}

// cur position methods
long XmlParser::getCurrent ()
{
	return _current;
}

long XmlParser::getParseLength ()
{
	return _parseLength;
}

long XmlParser::getCurLength ()
{
	return getOffsetLength(_current);
}

long XmlParser::getOffsetLength ( long offset )
{
	return getLength(offset,_parseLength - 1);
}


char * XmlParser::getBufferPos ()
{
	return _buffer;
}

char * XmlParser::getLastBufferPos ()
{
	return _buffer + _parseLength;
}

char * XmlParser::getCurPos ()
{
	if ( isValid() )
		return _buffer + _current;
	else
		return NULL;
}

char * XmlParser::getParseState ( long & parseLength )
{
	// if not valid stop
	if ( !isValid() )
	{
		parseLength = 0;
		return getCurPos();
	}

	/*
	// if value has length
	if ( getValueLength() > 0 )
	{
		// if value has a tag it must be parsed
		if ( valueHasTag() )
		{
			// get parse length
			parseLength = getValueLength();

			// return ptr to start of value
			return _buffer + _valueStart;
		}
	}
	*/

	// get parse state
	parseLength = getCurLength();

	// get current buffer position
	//char * buffer = getCurPos();

	// if last tag is valid
	/*
	if ( hasLastTag() )
	{
		// if current position is in last tag
		// pos then show stop
		if ( getCurPos() >= getLastTagPos() )
		{
			parseLength = 0;
			return getLastBufferPos();
		}
	}
	*/

	return getCurPos();

}

// get ref to parse buffer
std::string & XmlParser::str ()
{
	return _strParse;
}

// state methods
void XmlParser::reset ()
{
	resetTagPositions();
}

bool XmlParser::isValid ()
{
	// if buffer state not valid
	if ( !_buffer || _parseLength <= 0 )
		return false;

	// if cur position not valid
	if ( _current < 0 || _current > _parseLength )
		return false;

	return true;
}

void XmlParser::resetTagPositions ( long start)
{
	// set tag positions
	_firstTagStart  = start;
	_firstTagEnd	= start;

	_lastTagStart   = start;
	_lastTagEnd	    = start;

	_nameStart	    = start;
	_nameEnd		= start;

	_attrStart      = start;
	_attrEnd        = start;

	_valueStart     = start;
	_valueEnd       = start;
}

void XmlParser::clear ()
{
	// parse buffer and offsets of where we are parsing
	_buffer		 = 0;
	_parseLength = 0;

	// current position in parse buffer
	_current	= 0;

	// reset tag positions
	reset();
}

// parse methods
bool XmlParser::parse ( const char * buffer, long parseStart, long parseEnd )
{
	// if create fails stop
	if ( !create(buffer,parseStart,parseEnd) )
		return false;

	return parse();
}

bool XmlParser::parse ( const char * buffer, long parseLength )
{
	// if create fails stop
	if ( !create(buffer,parseLength) )
		return false;

	return parse();
}

bool XmlParser::parse () 
{
	// init tag position
	_firstTagStart = _current;
	_firstTagEnd   = _current;

	_lastTagStart  = _current;
	_lastTagEnd    = _current;

				 
	// find first tag
	long first = find( idTagLeft, _current );
	if ( first == -1 )
		return false;


	// if find right tag
	long last  = find( idTagRight, first );
	if ( last == -1  )
		return false;

	// set first tag start/end
	_firstTagStart = first;
	_firstTagEnd   = last;

	// now parse name
	if ( !parseName() )
		return false;

	// parse attributes
	parseAttributes();

	// if null tag no data or last tag
	if ( hasNullTag() || isSpecialXMLTag() )
	{
		// update cur position
		_current  = _firstTagEnd + idTagRightLength;

		// done so show success
		return true;
	}		

	// form end tag
	std::string endTag;
	endTag  = idTagEnd;
	endTag += getName();
	endTag += idTagRight;

	// find last tag
	first = find( endTag, last );
	if ( first == -1 )
		return false;


   // ARNAUD bug fix
   // If there is a tag with the same name before endTag we're in troubles
   // So we have to check this possibility
	std::string Tag;
	Tag  = idTagLeft;
	Tag += getName();

   long sameNameFirst; 
   while((sameNameFirst = find(Tag, last, first-last)) != -1)
   {
      // Check if we have space or '>' in this case fix the problem
      // In other cases we have a different tag, just skip
      char charAfterTag = _buffer[sameNameFirst + 1 + getName().length()];
      if(charAfterTag=='>' || isspace(charAfterTag) != 0)
      {
            // We found similar tag
            // We have to fix
            first = find(endTag, first+1);
            if ( first == -1 )
               return false;
      }
      last = sameNameFirst+1;   
   }
   // ARNAUD bug fix

	// set last tag start/end
	_lastTagStart = first;
	_lastTagEnd   = first + (int)endTag.size() - 1;

	// parse the value if not a null tag
	if ( !hasNullTag() )
		parseValue();

	// update cur position
	// we have parsed a tag so look for the start
	// of a new tag, if found set current position
	// to it, else set to last tag
	long pos = find( idTagLeft, _lastTagEnd );
	if ( pos != -1 )
		_current = pos;
	else
		_current = _lastTagEnd;


	return true;
}

bool XmlParser::parse ( std::string & /*name*/,
						std::string & /*value*/,
						std::string & /*attributes*/,
						long & /*current*/ )
{

	return true;
}



// tag search methods

bool XmlParser::hasNullTag ()
{
	// get end of first tag
	char * buffer = _buffer + _firstTagEnd - 1;

	// if null tag marker
	if ( *buffer == '/' && *(buffer+1) == '>' )
		return true;
	else
		return false;
}

bool XmlParser::isSpecialXMLTag()
{
	std::string name = getName();

   return name.compare(0, 4, "?xml") == 0 || // <?xml tag ?>
          name.compare(0, 3, "!--")  == 0;   // <!-- comment tar -->
}

// name search methods
bool XmlParser::parseName ()
{
	// if first tag search failed show failed
	if ( _firstTagStart < 0 ||  _firstTagEnd < 0 ||
		 _firstTagEnd <= _firstTagStart )
	{
		_nameStart = -1;
		_nameEnd   = -1;
		return false;
	}

	// init name start/end position
	_nameStart = _firstTagStart + idTagLeftLength;
	_nameEnd   = _firstTagEnd - 1;

	// if null tag then backup before
	// null tag marker
	if ( hasNullTag() )
		_nameEnd -= 1;

	// check for separator (i.e. there are attributes)
	long last = find(' ',_nameStart, getNameLength());
	if ( last != -1 )
	{
		// there are attributes so backup
		// before attributes
		_nameEnd = last - 1;
	}

	return true;
}

bool XmlParser::parseName ( std::string & name )
{
	// set name state
	if ( !parseName() )
		return false;

	name = getName();

	return true;
}

// attribute search methods
bool XmlParser::parseAttributes ()
{
	// init name start/end position
	_attrStart = -1;
	_attrEnd   = -1;

	// if tag or name length invalid stop
	long tagLength  = getTagLength();
	long nameLength = getNameLength();
	if ( tagLength <= 0 || nameLength <= 0 )
		return 0;

	// if the difference in the lengths is
	// less than the length of the left/right marker
	// then no attributes
	long diff = getTagLength() - getNameLength();

	switch ( diff )
	{
		// no attribute case
		case 0:
		case 1:
		case 2:
			return false;

		// no attribute case but has null tag
		case 3:
			return false;
	}

	// init attributes start, move past space after name
	_attrStart = _nameEnd + 2;

	// init attribute end move before right tag marker
	// if null tag move before null tag marker
	_attrEnd = _firstTagEnd - 1;
	if ( hasNullTag() )
		_attrEnd -= -1;

	return true;
}

bool XmlParser::parseAttributes ( std::string & attributes )
{
	// set name state
	if ( !parseAttributes() )
		return false;

	attributes = getAttributes();

	return true;
}


// data search methods
bool XmlParser::parseValue ()
{
	// if first tag search failed show failed
	if ( _firstTagStart < 0 ||  _lastTagEnd < 0 ||
		 _lastTagEnd <= _firstTagStart )
	{
		_valueStart = -1;
		_valueEnd   = -1;
		return false;
	}

	// init value start/end positions
	_valueStart = _firstTagEnd + 1;
	_valueEnd   = _lastTagStart - 1;

	return true;
}


bool XmlParser::parseValue ( std::string & value )
{
	// set name state
	if ( !parseValue() )
		return false;

	value = getValue();

	return true;
}

char * XmlParser::getNamePos ()
{
	if ( hasName() )
		return _buffer + _nameStart;
	else
		return NULL;
}

bool XmlParser::hasName ()
{
	if ( getNameLength() > 0 )
		return true;
	else
		return false;
}

long XmlParser::getNameLength ()
{
	long length = getLength(_nameStart,_nameEnd);
	return length;
}


std::string XmlParser::getName ()
{
	// get name length
	long length = getNameLength();

	// if length invalid show null string
	// else get string
	if ( length <= 0 )
		return std::string("");
	else
		return substr(_nameStart,length);
}


// attribute access methods
char * XmlParser::getAttributesPos ()
{
	if ( hasAttributes() )
		return _buffer + _attrStart;
	else
		return NULL;
}

bool XmlParser::hasAttributes ()
{
	if ( getValueLength() > 0 )
		return true;
	else
		return false;
}

long XmlParser::getAttributesLength ()
{
	long length = getLength(_attrStart,_attrEnd);
	return length;
}

std::string XmlParser::getAttributes ()
{
	// get attribute length
	long length = getAttributesLength();

	// if length invalid show null string
	// else get string
	if ( length <= 0 )
		return std::string("");
	else
		return substr(_attrStart,length);
}

// value access methods
char * XmlParser::getValuePos ()
{
	if ( hasValue() )
		return _buffer + _valueStart;
	else
		return NULL;
}

bool XmlParser::hasValue ()
{
	if ( getValueLength() > 0 )
		return true;
	else
		return false;
}

long XmlParser::getValueLength ()
{
	long length = getLength(_valueStart,_valueEnd);
	return length;
}

std::string XmlParser::getValue ()
{
	// get tag data length
	long length = getValueLength();

	// if length invalid show null string
	// else get string
	if ( length <= 0 )
		return std::string("");
	else
		return substr(_valueStart,length);
}

char * XmlParser::getValueState ( long & valueLength )
{
	// get value state
	valueLength = getValueLength();

	// return value buffer pos
	return _buffer + _valueStart;
}

bool XmlParser::valueHasTag ()
{
	// if find end tag
	long pos = find( idTagLeft, _valueStart, getValueLength() );

	// if found tag
	if ( pos != -1 )
		return true;
	else
		return false;
}

// tag access methods
long XmlParser::getTagLength ()
{
	long length = getLength( _firstTagStart, _firstTagEnd );
	return length;
}

long XmlParser::getLastTagLength ()
{
	long length = getLength( _lastTagStart, _lastTagEnd );
	return length;
}

bool XmlParser::hasTag ()
{
	if ( getTagLength() > 0 )
		return true;
	else
		return false;
}

bool XmlParser::hasLastTag ()
{
	if ( getLastTagLength() > 0 )
		return true;
	else
		return false;
}

char * XmlParser::getTagPos ()
{
	if ( hasTag() )
		return _buffer + _firstTagStart;
	else
		return NULL;
}

char * XmlParser::getLastTagPos ()
{
	if ( hasTag() )
		return _buffer + _lastTagStart;
	else
		return NULL;
}

std::string XmlParser::getTag ()
{
	// get tag data length
	long length = getTagLength();
	return substr(_firstTagStart,length);
}

// string utility methods
long XmlParser::getLength ( long startPos,
			     long endPos )
{
	// if positions invalid show no length
	if ( startPos < 0 || endPos < 0 ||
		 endPos < startPos )
		return 0;

	// get length
	long length = endPos - startPos + 1;
	return length;
}

long XmlParser::find ( char srchChar, long offset, long length)
{
	// if no length set to length to
	// end of parse buffer
	if ( length == -1 )
		length = getOffsetLength(offset);

	// set start and end of search 
	char* start = _buffer + offset;
	char* end   = _buffer + (offset + length);

	// search for it
	char* found = std::find( start, end, srchChar );

	// if at end did not find it
	if ( found >= end )
	{
		return -1;
	}
	else
	{
		// as a last check make sure found is valid
		if ( found < start )
			return -1;

		// set position
		long pos = (long)(found - start);
		pos += offset;

		return pos;
	}
}

long XmlParser::find ( char * srchStr, long offset, long length)
{
	// if no length set to length to
	// end of parse buffer
	if ( length == -1 )
		length = getOffsetLength(offset);

	// set start and end of search 
	char* start = _buffer + offset;
	char* end   = _buffer + (offset + length);


	char* srchStart = srchStr;
	char* srchEnd   = srchStr + strlen(srchStr);

	// search for it
	char* found = std::search( start, end, srchStart, srchEnd );

	// if at end did not find it
	if ( found >= end )
	{
		return -1;
	}
	else
	{
		// as a last check make sure found is valid
		if ( found < start )
			return -1;

		// set position
		long pos = (long)(found - start);
		pos += offset;

		return pos;
	}
}

long XmlParser::find ( std::string & srchStr, long offset, long length)
{
	// if no length set to length to
	// end of parse buffer
	if ( length == -1 )
		length = getOffsetLength(offset);

	// set start and end of search 
	char *start = _buffer + offset;
	char *end = _buffer + (offset + length);

	// search for it
    char *found = std::search( start, end, srchStr.begin(), srchStr.end());

	// if at end did not find it
	if ( found >= end )
	{
		return -1;
	}
	else
	{
		// as a last check make sure found is valid
		if ( found < start )
        {
			return -1;
        }

		// set position
		long pos = (long)(found - start);
		pos += offset;

		return pos;
	}
}

std::string XmlParser::substr ( long offset, long length )
{
	// get start of sub string
	char * ptr = _buffer + offset;

	// create string for it
	std::string str;
	str.assign( ptr, length );

	return std::string(str);
}
