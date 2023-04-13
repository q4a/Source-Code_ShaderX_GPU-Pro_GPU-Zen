#include "Input.hpp"

namespace swShader
{
	IDirectInput8 *Input::directInput = 0;

	Input::Input()
	{
		directInput = 0;
	}

	Input::~Input()
	{
		releaseDirectInput();
	}

	void Input::init(HWND windowHandle)
	{
		DirectInput8Create((HINSTANCE)GetWindowLong(windowHandle, GWL_HINSTANCE), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, 0);
	}

	void Input::releaseDirectInput()
	{
		if(directInput)
		{
			directInput->Release();
			directInput = 0;
		}
	}
}