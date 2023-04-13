#ifndef Keyboard_hpp
#define Keyboard_hpp

#include "Input.hpp"

namespace swShader
{
	class Keyboard : public Input
	{
	public:
		Keyboard(HWND windowHandle);
		
		~Keyboard();

		void input();
		bool keyPressed(int k) const;

		bool acquire();
		void unacquire();

	private:
		void init(HWND windowHandle);
		void release();

		IDirectInputDevice8 *keyboardInterface;
		char key[256];
	};
}

#endif   // Keyboard_hpp
