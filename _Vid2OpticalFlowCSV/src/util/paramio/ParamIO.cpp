#include "stdafx.h"
#include "ParamIO.h"
#pragma warning (disable : 4267)

void parseAccess(const char *str, std::vector<std::string> &access)
{
	access.clear();

   if(str == 0 || strlen(str) == 0)
   {
      return;
   }

	std::istringstream stream(str);

	char *char_str = new char[strlen(str)];

	while(stream.getline(char_str, strlen(str), ':'))
	{
		access.push_back(std::string(char_str));
	}

	delete[] char_str;
}

void ParamIO::readStream(std::istream &is)
{
	std::string str;

	// Copy file content to string
	char c;

	is.get(c);
	while(is.eof() == false)
	{
		str += c;
		is.get(c);
	}

	_treeBuilder.clear();
	_xml.setSubscriber(_treeBuilder);

	// Parse XML
	_xml.parse(str.c_str(), str.size());
}

void ParamIO::writeStream(std::ostream &os) const
{
	_treeBuilder.print(os);
}

//void ParamIO::writeStream(CArchive &os) const
//{
//	_treeBuilder.print(os);
//}

void ParamIO::readFile(const char *filename)
{
	std::ifstream fileIn;
	fileIn.open(filename);
	if(fileIn)
	{
		readStream(fileIn);
		fileIn.close();
	}
}

bool ParamIO::writeFile(const char *filename) const
{
	std::ofstream os(filename);
	if(!os)
		return false;
	writeStream(os);
	os.close();
	return true;
}

bool ParamIO::compare(const ParamIO &old, const char *str) const
{
	std::vector<std::string> strs;
   parseAccess(str, strs);

	return _treeBuilder.compare(old, strs);
}

bool ParamIO::extract(const char *str, ParamIO &subtree) const
{
	std::vector<std::string> strs;
   parseAccess(str, strs);

   if(strs.size() == 0)
   {
       return false;
   }

   subtree.clear();

   return _treeBuilder.extractSubTree(strs, subtree);
}

bool ParamIO::erase(const char *str)
{
    std::vector<std::string> strs;
    parseAccess(str, strs);

   if(strs.size() == 0)
   {
       return false;
   }

   return _treeBuilder.eraseSubTree(strs);
}


//// Tests
//#include <assert.h>
//bool firstTest()
//{
//   ParamIO xml;
//
//   std::istringstream is("<ACTION><P1>1</P1><P2>2</P2><ACTION><P3>3</P3></ACTION></ACTION>");
//
//   ParamIO in;
//   in.readStream(is);
//
//   int i;
//
//   in.read("ACTION:P1", i, 0);
//   assert(1 == i);
//   in.read("ACTION:P2", i, 0);
//   assert(2 == i);
//   in.read("ACTION:ACTION:P3", i, 0);
//   assert(3 == i);
//
//   return true;
//}
//
//static bool t1 = firstTest();
//
//bool secondTest()
//{
//   ParamIO xml;
//
//   std::istringstream is("<?xml ?><!--Comment--><ACTION><P1>1</P1><P2>2</P2><ACTION><P3>3</P3><ACTION><P4>4</P4></ACTION></ACTION></ACTION>");
//
//   ParamIO in;
//   in.readStream(is);
//
//   int i;
//
//   in.read("ACTION:P1", i, 0);
//   assert(1 == i);
//   in.read("ACTION:P2", i, 0);
//   assert(2 == i);
//   in.read("ACTION:ACTION:P3", i, 0);
//   assert(3 == i);
//   in.read("ACTION:ACTION:ACTION:P4", i, 0);
//   assert(4 == i);
//
//   return true;
//}
//
//static bool t2 = secondTest();
//
//bool thirdTest()
//{
//   ParamIO xml;
//
//   std::istringstream is("<?xml ?><!--Comment--><ACTION><P1>1</P1><P2>2</P2><ACTION><P3>3</P3><ACTION><P4>4</P4></ACTION></ACTION></ACTION>");
//
//   ParamIO in;
//   in.readStream(is);
//
//   int i;
//
//   in.read("ACTION:ACTION:P3", i, 0);
//   assert(3 == i);
//
//   in.erase("ACTION:ACTION");
//   in.read("ACTION:ACTION:P3", i, -1);
//   assert(-1 == i);
//   in.read("ACTION:ACTION:ACTION:P4", i, -1);
//   assert(-1 == i);
//
//   return true;
//}
//
//static bool t3 = thirdTest();
//
//bool fourthTest()
//{
//   ParamIO xml;
//
//   std::istringstream is("<?xml ?><!--Comment--><ACTION><P1>1</P1><P2>2</P2><ACTION><P3>3</P3><ACTION><P4>4</P4></ACTION></ACTION></ACTION>");
//
//   ParamIO in;
//   in.readStream(is);
//
//   int i;
//
//   in.read("ACTION:ACTION:P3", i, 0);
//   assert(3 == i);
//
//   in.write("ACTION:ACTION:P3", 3);
//   in.read("ACTION:ACTION:P3", i, -1);
//   assert(3 == i);
//
//   in.read("ACTION:ACTION:P10", i, -1);
//   assert(-1 == i);
//   in.write("ACTION:ACTION:P10", 10);
//   in.read("ACTION:ACTION:P10", i, -1);
//   assert(10 == i);
//
//   return true;
//}
//
//static bool t4 = fourthTest();
//
//bool fifthTest()
//{
//   ParamIO xml;
//
//   std::istringstream is("<ACTION><P1>1</P1><P2>2</P2><ACTION><P3>3</P3><ACTION><P4>4</P4></ACTION></ACTION></ACTION>");
//
//   ParamIO in;
//   in.readStream(is);
//
//   int i;
//
//   in.read("ACTION:ACTION:P3", i, 0);
//   assert(3 == i);
//
//   in.erase("ACTION");
//
//
//   return true;
//}
//
//static bool t5 = fifthTest();
//
//bool sixthTest()
//{
//   ParamIO xml;
//
//   std::istringstream is("<ACTION><P1>1</P1><P2>2</P2><ACTION comment=\"toto\"><P3>3</P3><ACTION><P4>4</P4></ACTION></ACTION></ACTION>");
//
//   ParamIO in;
//   in.readStream(is);
//
//   int i;
//
//   in.read("ACTION:ACTION:P3", i, 0);
//   assert(3 == i);
//
//   in.erase("ACTION");
//
//
//   return true;
//}
//
//static bool t6 = sixthTest();
//
//bool sevenTest()
//{
//   ParamIO xml;
//
//   std::istringstream is("<P><Z1><N1>3</N1></Z1><P1>dsgs</P1></P>");
//
//   ParamIO in;
//   in.readStream(is);
//
//   int i;
//
//   in.read("P:Z1:N1", i, 0);
//   assert(3 == i);
//
//
//   return true;
//}
//
//static bool t7 = sevenTest();
