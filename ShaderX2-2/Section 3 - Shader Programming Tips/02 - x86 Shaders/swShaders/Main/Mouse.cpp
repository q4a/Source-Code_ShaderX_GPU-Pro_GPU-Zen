#include "Mouse.hpp"

#include "Error.hpp"

namespace swShader
{
	Mouse::Mouse(HWND windowHandle)
	{
		smoothFactor = 0.0f;
		init(windowHandle);
	}

	Mouse::~Mouse()
	{
		release();
	}

	float Mouse::getHorizontalDisplacement() const
	{
		return dx;
	}

	float Mouse::getVerticalDisplacement() const
	{
		return dy;
	}

	bool Mouse::getButton1() const
	{
		return b1 != 0;
	}

	bool Mouse::getButton2() const
	{
		return b2 != 0;
	}

	bool Mouse::getButton3() const
	{
		return b3 != 0;
	}

	bool Mouse::getButton4() const
	{
		return b4 != 0;
	}

	void Mouse::setSmoothFactor(float factor)
	{
		smoothFactor = factor;
	}

	void Mouse::init(HWND windowHandle)
	{
		if(!directInput) Input::init(windowHandle);

		directInput->CreateDevice(GUID_SysMouse, &mouseInterface, 0);
		mouseInterface->SetDataFormat(&c_dfDIMouse);
		mouseInterface->SetCooperativeLevel(windowHandle, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

		acquire();
	}

	bool Mouse::acquire()
	{
		switch(mouseInterface->Acquire())
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

	void Mouse::unacquire()
	{
		mouseInterface->Unacquire();
	}

	void Mouse::input()
	{
		DIMOUSESTATE dims;

		mouseInterface->GetDeviceState(sizeof(DIMOUSESTATE), &dims);

		dx = ((float)dims.lX + smoothFactor * dx) / (1.0f + smoothFactor);
		dy = ((float)dims.lY + smoothFactor * dy) / (1.0f + smoothFactor);
		dz = ((float)dims.lZ + smoothFactor * dz) / (1.0f + smoothFactor);

		b1 = dims.rgbButtons[0];
		b2 = dims.rgbButtons[1];
		b3 = dims.rgbButtons[2];
		b4 = dims.rgbButtons[3];
	}

	void Mouse::release()
	{
		unacquire();

		if(mouseInterface)
		{
			mouseInterface->Release();
			mouseInterface = 0;
		}
	}
}