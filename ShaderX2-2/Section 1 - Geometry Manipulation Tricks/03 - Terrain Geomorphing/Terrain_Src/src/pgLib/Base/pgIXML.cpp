///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgXML
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIXML.cpp,v 1.1 2002/12/15 21:54:08 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgIXML.h"

void
pgIXML::getNodeName(IXMLDOMNode* nNode, pgString& nName)
{
	CComBSTR bstr;

	HRESULT hr = nNode->get_nodeName(&bstr);
	if(FAILED(hr))
		return;

	_bstr_t bstrt(bstr);
	nName = (const char*)bstrt;
}


void
pgIXML::getNodeText(IXMLDOMNode* nNode, pgString& nText)
{
	CComBSTR bstr;

	HRESULT hr = nNode->get_text(&bstr);
	if(FAILED(hr))
		return;

	_bstr_t bstrt(bstr);
	nText = (const char*)bstrt;
}


bool
pgIXML::checkNodeName(IXMLDOMNode* nNode, const char* nName)
{
	pgString str;

	getNodeName(nNode, str);
	return str==nName;
}


int
pgIXML::getSubnodeAsInt(IXMLDOMNode* nNode, const char* nSel, int nAlt)
{
	CComBSTR		bstr(nSel);
	IXMLDOMNode*	node = NULL;
	pgString		str;

	if(FAILED(nNode->selectSingleNode(bstr, &node)))
		return nAlt;
	if(node==NULL)
		return nAlt;

	getNodeText(node, str);

	return atoi((LPCSTR)str);
}


pgString
pgIXML::getSubnodeAsString(IXMLDOMNode* nNode, const char* nSel, const char* nAlt)
{
	CComBSTR		bstr(nSel);
	IXMLDOMNode*	node = NULL;
	pgString		str;

	if(FAILED(nNode->selectSingleNode(bstr, &node)))
		return nAlt;
	if(node==NULL)
		return nAlt;

	getNodeText(node, str);

	return (LPCSTR)str;
}
