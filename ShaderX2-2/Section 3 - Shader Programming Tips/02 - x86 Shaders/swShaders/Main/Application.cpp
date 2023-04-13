#include "Application.hpp"

#include "FrameBuffer.hpp"
#include "Error.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "VertexBuffer.hpp"
#include "XVertex.hpp"
#include "Color.hpp"
#include "Matrix.hpp"
#include "Sampler.hpp"
#include "Timer.hpp"
#include "Model3DS.hpp"
#include "CPUID.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"

namespace swShader
{
	Application::Application(HINSTANCE instance) : Window(instance, "swShader Demo", 640, 480)
	{
		if(!CPUID::supportsMMX() || !CPUID::supportsSSE())
		{
			throw Error("swShader requires MMX/SSE support.");
		}

		frameBuffer = 0;
		renderer = 0;
		keyboard = 0;
		mouse = 0;

		init();
	}

	Application::~Application()
	{
		destroy();
	}

	void Application::init()
	{
		frame = 0;

		elapsedTime = 0;
		totalTime = 0;

		fpsFrames = 0;
		fpsTime = Timer::seconds();
		fpsCurrent = -1;
		fpsUpdate = 1;

		delete frameBuffer;
		frameBuffer = new FrameBuffer();

		if(style & FULLSCREEN)
		{
			hideCursor();
			frameBuffer->initFullscreen(handle, width, height);
		}
		else
		{
			showCursor();
			frameBuffer->initWindowed(handle, width, height);
		}

		renderer = new Renderer(frameBuffer);

		renderer->setShader("Shader.txt");

		texture[0] = new Texture("Crate.jpg");
		texture[1] = new Texture("Lightning.jpg");

		renderer->setTextureMap(0, texture[0]);
		renderer->setTextureMap(1, texture[1]);

		frameBuffer->clear();
		frameBuffer->flip();

		keyboard = new Keyboard(handle);
		mouse = new Mouse(handle);

		model = new Model3DS("Teapot.3ds");

		camera.setPosition(0, -100, 0);
	}

	void Application::destroy()
	{
		delete frameBuffer;
		frameBuffer = 0;

		delete renderer;
		renderer = 0;

		delete texture[0];
		delete texture[1];
		texture[0] = 0;
		texture[1] = 0;

		delete model;
		model = 0;
	}

	void Application::eventDraw()
	{
		static double previousTime = Timer::seconds();

		try
		{
			frameBuffer->lock();

			renderFrame();
			handleInput();

			frame++;

			const double currentTime = Timer::seconds();
			elapsedTime = (float)(currentTime - previousTime);
			totalTime += elapsedTime;
			previousTime = currentTime;

			fpsFrames++;

			if(currentTime - fpsTime >= fpsUpdate)
			{
				fpsCurrent = fpsFrames / (float)(currentTime - fpsTime);

				fpsFrames = 0;
				fpsTime = currentTime;
			}

			frameBuffer->updateBounds();
			frameBuffer->flip();
		}
		catch(const Error &error)
		{
			if(style & FULLSCREEN) toggleFullscreen();

			throw error;
		}
		catch(...)
		{
			throw Error("Unknown fatal error in render loop");
		}
	}

	void Application::eventKeyDown(int key)
	{
		if(key == VK_ESCAPE)
		{
			terminate();
		}
	}

	void Application::renderFrame()
	{
		if(!renderer || !frameBuffer) throw Error("Initialization error");

		frameBuffer->clear();

		float pulse[4];
		pulse[0] = (1 + sin((float)totalTime - M_PI_2)) / 2;

		renderer->setPixelShaderConstant(0, pulse);
		renderer->setModelTransform(Matrix::eulerRotate(0.2f * (float)totalTime,
			                                            0.4f * (float)totalTime,
														0.3f * (float)totalTime));
		renderer->setViewTransform(camera.getTransform());
	
		renderer->drawPrimitive(model, model);
	}

	void Application::handleInput()
	{
		if(!keyboard || !mouse) throw Error("Initialization error");

		if(!hasFocus())return;
		if(!keyboard->acquire()) return;
		if(!mouse->acquire()) return;

		keyboard->input();
		mouse->input();

		float speedFactor = 100 * elapsedTime;

		if(keyboard->keyPressed(DIK_W) || keyboard->keyPressed(DIK_UP))
		{
			camera.translateRelative(speedFactor * Vector(0, +1, 0));
		}

		if(keyboard->keyPressed(DIK_S) || keyboard->keyPressed(DIK_DOWN))
		{
			camera.translateRelative(speedFactor * Vector(0, -1, 0));
		}

		if(keyboard->keyPressed(DIK_A) || keyboard->keyPressed(DIK_LEFT))
		{
			camera.translateRelative(speedFactor * Vector(-1, 0, 0));
		}

		if(keyboard->keyPressed(DIK_D) || keyboard->keyPressed(DIK_RIGHT))
		{
			camera.translateRelative(speedFactor * Vector(+1, 0, 0));
		}

		if(mouse->getHorizontalDisplacement() || mouse->getVerticalDisplacement())
		{
			float phi = 0.002f * mouse->getHorizontalDisplacement();
			float theta = 0.002f * mouse->getVerticalDisplacement();

			camera.eulerRotate(theta, 0, phi);
		}
	}
}