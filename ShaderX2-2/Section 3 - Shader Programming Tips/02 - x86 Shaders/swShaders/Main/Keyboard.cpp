#include "Keyboard.hpp"

#include "Error.hpp"

namespace swShader
{
	Keyboard::Keyboard(HWND windowHandle)
	{
		init(windowHandle);
	}

	Keyboard::~Keyboard()
	{
		release();
	}

	void Keyboard::init(HWND windowHandle)
	{
		if(!directInput) Input::init(windowHandle);

		directInput->CreateDevice(GUID_SysKeyboard, &keyboardInterface, 0);
		keyboardInterface->SetDataFormat(&c_dfDIKeyboard);
		keyboardInterface->SetCooperativeLevel(windowHandle, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

		acquire();
	}

	bool Keyboard::acquire()
	{
		switch(keyboardInterface->Acquire())
		{
		case DI_OK:
		case S_FALSE:
			return true;
		case DIERR_OTHERAPPHASPRIO:
			return false;
		case DIERR_INVALIDPARAM:
		case DIERR_NOTINITIALIZED:
		default:
			throw INTERNAL_ERROR;
		}
	}

	void Keyboard::unacquire()
	{
		keyboardInterface->Unacquire();
	}

	void Keyboard::input()
	{
		keyboardInterface->GetDeviceState(sizeof(key), &key);
	}

	bool Keyboard::keyPressed(int k) const
	{
		return (key[k] & 0x80) != 0;
	}

	void Keyboard::release()
	{
		unacquire();

		if(keyboardInterface)
		{
			keyboardInterface->Release();
			keyboardInterface = 0;
		}
	}
}