///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgAppStub
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgAppStub.cxx,v 1.5 2002/12/12 21:15:27 daniel Exp $
///////////////////////////////////////////////////////////////////


#include <D3DX8.h>

#include <pgLib/Audio/pgIAudio.h>
#include <pgLib/Base/pgSettingsFile.h>
#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgIStringTool.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/Base/pgIResourceManager.h>
#include <pgLib/Base/pgISettings.h>
#include <pgLib/Base/pgITime.h>
#include <pgLib/Math/pgMatrix.h>
#include <pgLib/FrameWork/D3DApp.h>
#include <pgLib/FrameWork/D3DRes.h>
#include <pgLib/FrameWork/D3DFont.h>


// the one and only game instance
//
#ifndef PG_GAMECLASSNAME
#pragma message(">>> ERROR: can not create application stub. PG_GAMECLASSNAME not defined\n           PG_GAMECLASSNAME needs to be defined before '#include <pgLib/FrameWork/pgAppStub.cxx>'\n")
#error compilation stopped
#else
pgIGame*	theGame = new PG_GAMECLASSNAME();
#endif //PG_GAMECLASSNAME


// if some properties are not set then create default values
//
#ifndef PG_WINDOW_TITLE
#define PG_WINDOW_TITLE "PGE Render Window   -   © 2002"
#endif //PG_WINDOW_TITLE

#ifndef PG_WINDOW_WIDTH
#define PG_WINDOW_WIDTH 800
#endif //PG_WINDOW_WIDTH

#ifndef PG_WINDOW_HEIGHT
#define PG_WINDOW_HEIGHT 600
#endif //PG_WINDOW_HEIGHT

#ifndef PG_LOGFILE
#define PG_LOGFILE "pgLib.log"
#endif //PG_LOGFILE


/// Main Application Class
/**
 * The base class (CD3DApplication) provides the 
 * generic directx functionality needed. pgAppStub adds
 * adds functionality specific to pgLib
 */
class pgAppStub : public CD3DApplication
{
public:
    pgAppStub();

	void	SetInstance(HINSTANCE hInst)  { m_hInst = hInst; }
	HWND	getHWnd() const  {  return m_hWnd;  }

	void	readConfigFile(const pgString& nConfigFile);
	const pgSettingsFile& getConfigFile() const  {  return configFile;  }

protected:
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT FrameMove();
    HRESULT Render();
    HRESULT FinalCleanup();

	virtual LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


	pgSettingsFile	configFile;
	HINSTANCE		m_hInst;


};


// the one and only app instance
//
pgAppStub theApp;
pgList<pgString> programArgs;



/// Entry point to the program
/**
 * Initializes everything, and goes into a
 * message-processing loop. Idle time is used to render the scene.
 */
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR nArgs, INT )
{
	pgLog::init(PG_LOGFILE);

	pgIStringTool::fillTokenList(nArgs, ' ', programArgs);

	theApp.readConfigFile(PG_CONFIG_FILENAME);
	theApp.SetInstance(hInst);

    if(FAILED(theApp.Create(hInst)))
        return 0;

    return theApp.Run();
}


/// Application constructor. Sets attributes for the app.
pgAppStub::pgAppStub()
{
    m_strWindowTitle            = PG_WINDOW_TITLE;
    m_bUseDepthBuffer           = TRUE;

	m_dwCreationWidth			= PG_WINDOW_WIDTH;
	m_dwCreationHeight			= PG_WINDOW_HEIGHT;
}


void
pgAppStub::readConfigFile(const pgString& nConfigFile)
{
	bool bVal;

	if(!configFile.load(nConfigFile))
		return;

	if(configFile.getValueYes("Disable_HardwareVertexShader", bVal))
		m_allowHardwareProcessing = !bVal;

	if(configFile.getValueYes("Disable_WriteOnlyBuffers", bVal))
		pgIDirectX::setDisableWriteOnlyBuffers(bVal);
}


/// Initialize scene objects.
HRESULT pgAppStub::InitDeviceObjects()
{
	pgISettings::init();
	pgIDirectX::init(m_pd3dDevice, m_pD3D);
	pgIResourceManager::init();

#ifdef PG_USEAUDIO
	pgAudio::init(pgIAudio::TYPE_FMOD);
#endif //PG_USEAUDIO

	theGame->init(m_hInst, m_hWnd);

    return S_OK;
}


/// Restores scene objects.
HRESULT pgAppStub::RestoreDeviceObjects()
{
    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );

	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, pgIDirectX::getProjectionMatrix().getD3DMatrix());
    //m_pFont->RestoreDeviceObjects();

	theGame->restoreDeviceObjects();

    return S_OK;
}


/// Called once per frame
/**
 *  The call is the entry point for animating the scene
 */
HRESULT pgAppStub::FrameMove()
{
	theGame->update();
    return S_OK;
}


/// Called once per frame
/**
 *  The call is the entry point for 3d rendering.
 *  This function sets up render states, clears the
 *  viewport, and renders the scene.
 */
HRESULT pgAppStub::Render()
{
	theGame->setStatistics(m_strFrameStats, m_strDeviceStats);
	theGame->render();
	return S_OK;
}


/// InvalidateDeviceObjects()
HRESULT pgAppStub::InvalidateDeviceObjects()
{
	theGame->invalidateDeviceObjects();
    return S_OK;
}


/// Called when the app is exiting, or the device is being changed,
/**
 *  This function deletes any device dependent objects.  
 */
HRESULT pgAppStub::DeleteDeviceObjects()
{
    //m_pFont->DeleteDeviceObjects();
	theGame->deleteDeviceObjects();
    return S_OK;
}


/// Called before the app exits
/**
 *  This function gives the app the chance to cleanup after itself.
 */
HRESULT pgAppStub::FinalCleanup()
{
    //SAFE_DELETE( m_pFont );

	theGame->cleanup();
    return S_OK;
}


LRESULT
pgAppStub::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	theGame->callIInput(hWnd, uMsg, wParam, lParam);

	return CD3DApplication::MsgProc(hWnd, uMsg, wParam, lParam);
}
