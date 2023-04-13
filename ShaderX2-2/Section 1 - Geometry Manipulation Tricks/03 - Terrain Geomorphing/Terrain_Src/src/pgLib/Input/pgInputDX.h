///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgInputDX
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgInputDX.h,v 1.1.1.1 2002/10/20 09:33:02 daniel Exp $
///////////////////////////////////////////////////////////////////

#ifndef __INPUTDX_HEADERFILE__
#define __INPUTDX_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Input/pgIInput.h>

#define DIRECTINPUT_VERSION  0x0800
#include <DINPUT.h>


/// This class implements the pgInput interface using directinput
/** 
 *  See pgInput for more documentation
 */
class PGLIB_API pgInputDX : public pgIInput
{
public:
	//enum  {  MOUSE__BUFFER_SIZE = 16  };

	pgInputDX();

	// implementation of pgInput
	// interface
	virtual bool init(int nTypes, HINSTANCE hInst, HWND hWnd);
	virtual void cleanup();
	virtual void update();
	virtual bool isKeyDown(KEY nKey) const;
	virtual bool isKeyNewDown(KEY nKey) const;
	virtual void processWindowMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


	virtual int getMouseX() const  {  return mouseX;  }
	virtual int getMouseY() const  {  return mouseY;  }
	virtual bool isButtonDown(BUTTON nButton) const  {  return (mouseB&nButton)!=0;  }


protected:
	void updateMouseAcquire();


	LPDIRECTINPUT8			m_pDI;
	LPDIRECTINPUTDEVICE8	m_pDIDeviceKB, m_pDIDeviceMouse;
	HANDLE                  hMouseEvent;

	unsigned char			*keyBuffer,				// stores the current keys
							*oldKeyBuffer;			// stores the keys of the previous frame

	int						mouseX, mouseY, mouseB;	// mouse x,y & buttons

	bool					windowActive;

	HINSTANCE	hInst;
	HWND		hWnd;
};


#endif //__INPUTDX_HEADERFILE__
