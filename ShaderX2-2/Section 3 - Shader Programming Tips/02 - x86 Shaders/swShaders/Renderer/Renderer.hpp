#ifndef Renderer_hpp
#define Renderer_hpp

#include "Context.hpp"

#include "Sampler.hpp"
#include "X86Shader.hpp"
#include "Matrix.hpp"
#include "State.hpp"

namespace swShader
{
	class RenderTarget;
	class Rasterizer;
	struct XVertex;
	class Texture;
	class Clipper;
	class Viewport;
	class VertexBuffer;
	class IndexBuffer;

	class Renderer : public Context
	{
	public:
		Renderer(RenderTarget *renderTarget);

		~Renderer();

		void drawTriangle(const XVertex &v1, const XVertex &v2, const XVertex &v3, int FVF);
		void drawPrimitive(const VertexBuffer *VB, const IndexBuffer *IB);

		void setShader(const char *shaderFile);
			
		void setTextureMap(int sampler, Texture *texture);
		void releaseTextures();

		void setTextureFilter(int sampler, Sampler::FilterType textureFilter);
		void setAddressingMode(int sampler, Sampler::AddressingMode addressingMode);
		void setDepthCompare(DepthCompareMode depthCompareMode);
		void setDepthWriteEnable(bool depthWriteEnable);
		void setCullMode(CullMode cullMode);

		void setFOV(float FOV);
		void setNearClip(float nearClip);
		void setFarClip(float farClip);

		void setModelTransform(const Matrix &M);
		void setViewTransform(const Matrix &V);
		void setBaseTransform(const Matrix &B);

		void setPixelShaderConstant(int index, const float value[4]);

	private:
		const State &status() const;
		void update();
		void renderPolygon(XVertex *V, int n, int FVF);

		char *shaderFile;
		unsigned int cacheSize;
		unsigned int cacheTop;
		X86Shader **shaderCache;
		State *shaderState;

		Clipper *clipper;
		Viewport *viewport;
		Rasterizer *rasterizer;

		float W;
		float H;

		float tanFOV;
		float nearClip;
		float farClip;

		Matrix M;	// Model/Geometry/World matrix
		Matrix V;	// View/Camera/Eye matrix
		Matrix B;	// Base matrix
		Matrix P;	// Projection matrix

		Matrix PB;		// P * B
		Matrix PBV;		// P * B * V
		Matrix PBVM;	// P * B * V * M

		bool updateModelMatrix;
		bool updateViewMatrix;
		bool updateBaseMatrix;
		bool updateProjectionMatrix;

		bool updatePixelShader;
	};
}

#endif   // Renderer_hpp