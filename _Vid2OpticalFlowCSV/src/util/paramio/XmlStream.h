#if !defined(XmlStream_H)
#define XmlStream_H

#include "XmlParser.h"
#include "XmlNotify.h"

#include <string>
#include <iostream>
#include <list>

//////////////////////////////////////////////////////////////////////////////
// XmlStream
//
// Purpose: stores a string that contain start,end tag delimited value


class XmlStream : public std::string
{
public:

	XmlStream ();

	virtual ~XmlStream ();

	// release resources
	bool create ();

	bool create ( char * buffer, long len );

	void release ();

	// notify methods
	void foundNode		( std::string & name, std::string & attributes );
	void endNode		( std::string & name, std::string & attributes ); // ARNAUD
	void foundElement	( std::string & name, std::string & value, std::string & attributes );

	// parse the current buffer
	bool parse			();
	bool parse			( const char * buffer, long parseLength );
	bool parseNodes		( XmlParser & parser, const char * buffer, long parseLength );

	// get/set subscriber
	bool hasSubscriber ();

	XmlNotify * getSubscriber ();

	void setSubscriber ( XmlNotify & set );

	// get ref to tag stream
	XmlStream & getTagStream ();


	// get string ref
	std::string & str();
private:
		XmlNotify *_subscriber;	// notification subscriber
};



#endif

