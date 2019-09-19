#include "stdafx.h"
#include "XmlStream.h"
#include "XmlParser.h"

using std::string;

// parser id for debugging
long XmlParser::_parseid = 0;

// notify methods
void XmlStream::foundNode ( string & name, string & attributes )
{
	// if no name stop
	if ( name.empty() )
		return;
  
	// tell subscriber
	if ( _subscriber )
		_subscriber->foundNode(name,attributes);
}

// ARNAUD
void XmlStream::endNode( string & name, string & attributes )
{
	if ( _subscriber )
		_subscriber->endNode(name, attributes);
}

void XmlStream::foundElement ( string & name, string & value, string & attributes )
{
	// if no name stop
	if ( name.empty() )
		return;

	// tell subscriber
	if ( _subscriber )
		_subscriber->foundElement(name,value,attributes);
}

bool XmlStream::parseNodes ( XmlParser & parser, const char * buffer, long parseLength )
{
	// #DGH note
	// need to address a null node within another node
	// i.e.
	// <Contacts>
	//		<Contact/>
	// </Contacts>
	// in this instance Contact will be reported as an
	// element 

	// debug info

	// while parse success, note for the first parser
	// this set the internal state also
	while ( parser.parse(buffer,parseLength) )
	{
		// if the value has a tag marker
      if(!parser.isSpecialXMLTag())
      {
		   if ( parser.valueHasTag() )
		   {
			   // show found node
			   string   name		 = parser.getName();
			   string	 attributes  = parser.getAttributes();

			   foundNode( name, attributes );

			   // get the parse state
			   long valueLength;
			   char * valueBuffer = parser.getValueState(valueLength);

			   // if parsing the node fails
			   XmlParser parserNode;
			   if ( !parseNodes(parserNode,valueBuffer,valueLength) )
				   return false;

			   // ARNAUD
			   endNode(name, attributes);

			   // if new parse cur position is in the last parser
			   // last tag position we are done with the node
			   char * curPos     = parserNode.getCurPos();
			   char * lastCurPos = parser.getLastTagPos();
			   if ( curPos >= lastCurPos )
			   {
				   break;
			   }
		   }
		   else
		   {
			   // show found element
			   string   name		 = parser.getName();
			   string   value		 = parser.getValue();
			   string	 attributes  = parser.getAttributes();

			   foundElement(name,value,attributes);
		   }
      }

		// get new parse state
		buffer = parser.getParseState(parseLength);
	}

	return true;
}



// parse the buffer

bool XmlStream::parse ( const char * buffer, long length )
{
	// if invalid stop
	if ( !buffer || length <= 0 )
		return false;

	// debug info
	assign( buffer, length );

	// declare parser
	XmlParser parser;

	// parse nodes
	bool docParsed = parseNodes(parser,buffer,length);

	return docParsed;
}

XmlStream ::XmlStream () :
//		std::string(),
	_subscriber(NULL)
{}

XmlStream ::~XmlStream ()
{ 
	release(); 
}

// release resources
bool XmlStream ::create ()
{
	return true;
}

bool XmlStream ::create ( char * buffer, long len )
{
	if ( buffer && len > 0 )
	{
		assign( buffer, len );
		return true;
	}
	else
		return false;
}

void XmlStream ::release ()
{
	erase( begin(), end() );
}

// get/set subscriber
bool XmlStream ::hasSubscriber ()
{
	if ( _subscriber )
		return true;
	else
		return false;
}

XmlNotify * XmlStream ::getSubscriber ()
{
	return _subscriber;
}

void XmlStream ::setSubscriber ( XmlNotify & set )
{
	_subscriber = &set;
}


// get ref to tag stream
XmlStream & XmlStream ::getTagStream ()
{ return *this; }


// get string ref
std::string & XmlStream ::str()
{ return (std::string &) *this; }



