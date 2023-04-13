#ifndef Input_hpp
#define Input_hpp

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace swShader
{
	class Input
	{
	protected:
		Input();

		~Input();

		static IDirectInput8 *directInput;

		void init(HWND windowHandle);
		void releaseDirectInput();
	};
}

#endif   // Input_hpp
