// PVSShow.h : Haupt-Header-Datei f�r die Anwendung PVSSHOW
//

#if !defined(AFX_PVSSHOW_H__B40A1378_7387_4C26_963A_9729B92FF3C6__INCLUDED_)
#define AFX_PVSSHOW_H__B40A1378_7387_4C26_963A_9729B92FF3C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CPVSShowApp:
// Siehe PVSShow.cpp f�r die Implementierung dieser Klasse
//

class CPVSShowApp : public CWinApp
{
public:
	CPVSShowApp();

// �berladungen
	// Vom Klassenassistenten generierte �berladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPVSShowApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CPVSShowApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingef�gt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VER�NDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // !defined(AFX_PVSSHOW_H__B40A1378_7387_4C26_963A_9729B92FF3C6__INCLUDED_)
