///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgXML
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIXML.h,v 1.1 2002/12/15 21:54:08 daniel Exp $
///////////////////////////////////////////////////////////////////


#include <pgLib/Base/pgString.h>

#include <Libs/MSXML/Include/msxml2.h>
#include <atlbase.h>
#include <comdef.h>


namespace pgIXML
{
	void getNodeName(IXMLDOMNode* nNode, pgString& nName);

	void getNodeText(IXMLDOMNode* nNode, pgString& nText);

	bool checkNodeName(IXMLDOMNode* nNode, const char* nName);

	int getSubnodeAsInt(IXMLDOMNode* nNode, const char* nSel, int nAlt);

	pgString getSubnodeAsString(IXMLDOMNode* nNode, const char* nSel, const char* nAlt);

};
