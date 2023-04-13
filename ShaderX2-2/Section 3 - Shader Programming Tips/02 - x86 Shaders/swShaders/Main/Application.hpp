#ifndef Application_hpp
#define Application_hpp

#include "Window.hpp"

#include "Point.hpp"
#include "Camera.hpp"

namespace swShader
{
	class Renderer;
	class Texture;
	class Model3DS;
	class FrameBuffer;
	class Keyboard;
	class Mouse;

	class Application : public Window
	{
	public:
		Application(HINSTANCE instance);

		~Application();

	protected:
		void init();
		void destroy();

		void eventDraw();
		void eventKeyDown(int key);
		
		void renderFrame();
		void handleInput();

	private:
		FrameBuffer *frameBuffer;
		Renderer *renderer;
		Keyboard *keyboard;
		Mouse *mouse;

		Texture *texture[2];
		Model3DS *model;

		Camera camera;

		int frame;
		float elapsedTime;
		double totalTime;

		int fpsFrames;
		double fpsTime;
		float fpsCurrent;
		double fpsUpdate;
	};
}

#endif   // Application_hpp