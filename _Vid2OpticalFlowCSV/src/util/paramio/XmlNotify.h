#if !defined(XmlNotify_H)
#define XmlNotify_H





/////////////////////////////////////////////////////////////////////////////////
// XmlNotify
//
// Purpose:		abstract class used to notify about xml tags found. 

class XmlNotify
{
public:


	XmlNotify ()
	{}

	// notify methods
	virtual void foundNode		( std::string & name, std::string & attributes ) = 0;
	virtual void foundElement	( std::string & name, std::string & value, std::string & attributes ) = 0;

	// ARNAUD
	virtual void endNode( std::string & name, std::string & attributes ) = 0;
};





#endif

