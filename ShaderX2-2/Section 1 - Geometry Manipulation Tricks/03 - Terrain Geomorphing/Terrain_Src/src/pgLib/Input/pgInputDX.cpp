///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgInputDX
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgInputDX.cpp,v 1.3 2002/12/08 16:36:22 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgInputDX.h"

#include <pgLib/Base/pgLog.h>


pgInputDX::pgInputDX()
{
	m_pDI = NULL;
	m_pDIDeviceKB = NULL;
	m_pDIDeviceMouse = NULL;
	hMouseEvent = NULL;
	keyBuffer = oldKeyBuffer = NULL;

	mouseX = mouseY = 0;
	mouseB = 0;

	windowActive = false;
}


bool
pgInputDX::init(int nTypes, HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	types = nTypes;

	this->hInst = hInst;
	this->hWnd = hWnd;

	pgLog::info("starting directinput setup...");
 
	keyBuffer = new unsigned char[256];
	oldKeyBuffer = new unsigned char[256];

	memset(keyBuffer, 0, 256);
	memset(oldKeyBuffer, 0, 256);


	// create the directInput object
	//
	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, NULL); 
	if FAILED(hr)
	{
		pgLog::error("creating directinput failed");
		return false;
	}


	if(types&KEYBOARD)
	{
		// retrieve a pointer to an IDirectInputDevice8 keyboard interface 
		hr = m_pDI->CreateDevice(GUID_SysKeyboard, &m_pDIDeviceKB, NULL); 

		if FAILED(hr) 
			pgLog::error("creating keyboard device failed");
		else
		{
 			// now that we have an IDirectInputDevice8 interface, get
			// it ready to use

 			// set the data format using the predefined keyboard data 
			// format provided by the DirectInput object for keyboards..
			//
			hr = m_pDIDeviceKB->SetDataFormat(&c_dfDIKeyboard); 
			if FAILED(hr) 
				pgLog::error("setting keyboard data format failed");
			{
				// set the cooperative level
				//
				hr = m_pDIDeviceKB->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 
				if FAILED(hr) 
					pgLog::error("setting direct input cooperative level failed");
				{
					// get access to the input device
					//
					hr = m_pDIDeviceKB->Acquire(); 
					if FAILED(hr) 
						pgLog::error("acquiring input device failed");
				}
			}
		}
	}


	if(types&MOUSE)
	{
		// Obtain an interface to the system mouse device.
		hr = m_pDI->CreateDevice(GUID_SysMouse, &m_pDIDeviceMouse, NULL);
 
		if (FAILED(hr))
			pgLog::error("creating mouse device failed");
		else
		{
			// Set the data format to "mouse format" - a predefined data format 
			// This tells DirectInput that we will be passing a
			// DIMOUSESTATE2 structure to IDirectInputDevice::GetDeviceState.
			hr = m_pDIDeviceMouse->SetDataFormat(&c_dfDIMouse2);
 
			if (FAILED(hr))
				pgLog::error("setting mouse data format failed");
			else
			{
				// Set the cooperativity level to let DirectInput know how
				// this device should interact with the system and with other
				// DirectInput applications.
				hr = m_pDIDeviceMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
 
				if (FAILED(hr))
					pgLog::error("setting direct input cooperative level failed");
			}
		}
	}

	pgLog::info("directinput setup finished");
	return true;
}


void
pgInputDX::cleanup()
{ 
    if(m_pDI) 
    { 
        if(m_pDIDeviceKB)
        {
			// always unacquire devices before calling Release(). 
			//
            m_pDIDeviceKB->Unacquire(); 
            m_pDIDeviceKB->Release();
            m_pDIDeviceKB = NULL; 
        }

        if(m_pDIDeviceMouse)
        { 
			// always unacquire devices before calling Release(). 
			//
            m_pDIDeviceMouse->Unacquire(); 
            m_pDIDeviceMouse->Release();
            m_pDIDeviceMouse= NULL; 
        } 

        m_pDI->Release();
        m_pDI = NULL;
    }

    if(hMouseEvent)
        CloseHandle(hMouseEvent);

	if(keyBuffer)
	{
		delete keyBuffer;
		keyBuffer = NULL;
	}

	if(oldKeyBuffer)
	{
		delete oldKeyBuffer;
		oldKeyBuffer = NULL;
	}
}


void
pgInputDX::update()
{
	unsigned char*		tmpBuf;
	HRESULT				hr;

	// check if an init was already done
	if(!m_pDI)
		return;


	if(m_pDIDeviceKB)
	{
		// switch the keyboard buffers
		//
		tmpBuf = oldKeyBuffer;
		oldKeyBuffer = keyBuffer;
		keyBuffer = tmpBuf;

		// if GetDeviceState() fails we try to reacquire
		// the keyboard input device
		if FAILED(hr = m_pDIDeviceKB->GetDeviceState(256,(LPVOID)keyBuffer)) 
		{ 
			hr = m_pDIDeviceKB->Acquire();
			while( hr == DIERR_INPUTLOST ) 
				hr = m_pDIDeviceKB->Acquire();
			memset(keyBuffer, 0, 256);
		}
	}



	if(m_pDIDeviceMouse)
	{
		// read mouse state
		//
		DIMOUSESTATE2 dims2;
		ZeroMemory(&dims2, sizeof(dims2));
		hr = m_pDIDeviceMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &dims2);
		if( FAILED(hr) ) 
		{
			// if we lost the device try to reacquire it
			//
			hr = m_pDIDeviceMouse->Acquire();
			while(hr==DIERR_INPUTLOST) 
				hr = m_pDIDeviceMouse->Acquire();
		}
		else
		{
			mouseX = dims2.lX;
			mouseY = dims2.lY;
			mouseB =  (dims2.rgbButtons[0]!=0) ? BUTTON_LEFT : 0;
			mouseB |= (dims2.rgbButtons[2]!=0) ? BUTTON_MIDDLE : 0;
			mouseB |= (dims2.rgbButtons[1]!=0) ? BUTTON_RIGHT : 0;

			keyBuffer[KEY_MOUSE_LEFT]   = (unsigned char)((dims2.rgbButtons[0]!=0) ? 0x80 : 0);
			keyBuffer[KEY_MOUSE_MIDDLE] = (unsigned char)((dims2.rgbButtons[2]!=0) ? 0x80 : 0);
			keyBuffer[KEY_MOUSE_RIGHT]  = (unsigned char)((dims2.rgbButtons[1]!=0) ? 0x80 : 0);
		}
	}
}


bool
pgInputDX::isKeyDown(KEY nKey) const
{
	int k = (int)nKey;
	if(k<0 || k>255)
		return false;

	return ((keyBuffer[nKey] & 0x80)!=0);
}


bool
pgInputDX::isKeyNewDown(KEY nKey) const
{
	int k = (int)nKey;
	if(k<0 || k>255)
		return false;

	return ((keyBuffer[nKey] & 0x80)!=0) && !((oldKeyBuffer[nKey] & 0x80)!=0);
}


void
pgInputDX::processWindowMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM)
{
	switch(uMsg) 
	{
	// Sent when window changes active state
	case WM_ACTIVATE:
		// Set exclusive mode access to the mouse based on active state
		windowActive = (WA_INACTIVE == wParam);
		updateMouseAcquire();
		return;

	case WM_ENTERMENULOOP:
	case WM_ENTERSIZEMOVE:
		// Un-acquire device when entering menu or re-sizing
		// Zhis will show the mouse cursor again
		windowActive = false;
		updateMouseAcquire();
		return;

	case WM_EXITSIZEMOVE:
		// Re-acquire device when leaving menu or re-sizing
		// This will show the mouse cursor again

		// Even though the menu is going away, the app
		// might have lost focus or be an icon
		windowActive = (GetActiveWindow()==hWnd || !IsIconic(hWnd));
		updateMouseAcquire();
		return;

	case WM_SYSCOMMAND:
        // The WM_SYSCOMMAND might've been a WM_CLOSE, 
        // in which case our window no longer exists.  
        if(IsWindow(hWnd)) 
            updateMouseAcquire();
        return;

	}
}


void
pgInputDX::updateMouseAcquire()
{
	// Nothing to do if g_pMouse is NULL
	if(m_pDIDeviceMouse == NULL)
		return;

	if(windowActive)
		m_pDIDeviceMouse->Acquire();
	else 
		m_pDIDeviceMouse->Unacquire();
}
