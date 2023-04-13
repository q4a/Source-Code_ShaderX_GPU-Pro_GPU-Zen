#ifndef Mouse_hpp
#define Mouse_hpp

#include "Input.hpp"

namespace swShader
{
	class Mouse : public Input
	{
	public:
		Mouse(HWND windowHandle);

		~Mouse();

		float getHorizontalDisplacement() const;
		float getVerticalDisplacement() const;
		bool getButton1() const;
		bool getButton2() const;
		bool getButton3() const;
		bool getButton4() const;

		void setSmoothFactor(float factor);

		void input();

		bool acquire();
		void unacquire();

	private:
		void init(HWND windowHandle);
		void release();

		float dx;
		float dy;
		float dz;

		char b1;
		char b2;
		char b3;
		char b4;

		float smoothFactor;

		IDirectInputDevice8 *mouseInterface;
	};
}

#endif   // Mouse_hpp