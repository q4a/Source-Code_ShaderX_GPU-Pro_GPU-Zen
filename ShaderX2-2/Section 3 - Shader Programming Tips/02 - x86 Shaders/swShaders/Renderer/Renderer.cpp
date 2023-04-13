#include "Renderer.hpp"

#include "Window.hpp"
#include "FrameBuffer.hpp"
#include "Rasterizer.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "PS20Shader.hpp"
#include "Vertex.hpp"
#include "Error.hpp"
#include "Texture.hpp"
#include "Viewport.hpp"
#include "Clipper.hpp"

namespace swShader
{
	Renderer::Renderer(RenderTarget *renderTarget)
	{
		if(!renderTarget) throw Error("Could not create renderer");

		clipper = new Clipper();
		viewport = new Viewport(renderTarget);
		rasterizer = new Rasterizer(renderTarget);

		W = 0;
		H = 0;

		tanFOV = tan(rad(90 / 2));
		nearClip = 1;
		farClip = 1000;

		M = 1;
		V = 1;
		B = Matrix(1, 0, 0, 0,
		           0, 0, 1, 0,
		           0, 1, 0, 0,
		           0, 0, 0, 1);
		P = 0;

		updateModelMatrix = true;
		updateViewMatrix = true;
		updateBaseMatrix = true;
		updateProjectionMatrix = true;

		updatePixelShader = true;

		shaderFile = 0;
		cacheSize = 256;
		cacheTop = 0;
		shaderCache = new X86Shader*[cacheSize];
		for(unsigned int i = 0; i < cacheSize; i++) shaderCache[i] = 0;
		shaderState = new State[cacheSize];
	}

	Renderer::~Renderer()
	{
		delete clipper;
		delete viewport;
		delete rasterizer;

		releaseTextures();

		delete[] shaderFile;
		for(unsigned int i = 0; i < cacheSize; i++) delete shaderCache[i];
		delete shaderCache;
	}

	void Renderer::drawTriangle(const XVertex &v1, const XVertex &v2, const XVertex &v3, int FVF)
	{
		if(!clipper || !rasterizer || !viewport) throw INTERNAL_ERROR;
		if(!(FVF & FVF_RHW)) throw INTERNAL_ERROR;

		update();

		XVertex V1 = PBVM * v1;
		XVertex V2 = PBVM * v2;
		XVertex V3 = PBVM * v3;

		XVertex *V = clipper->clipTriangle(V1, V2, V3, FVF | FVF_RHW);
		int n = clipper->getNumVertices();
		if(n < 3) return;

		const float L = viewport->getLeft();
		const float T = viewport->getTop() + H;

		for(int i = 0; i < n; i++)
		{
			XVertex &v = V[i];

			const float RHW = 1.0f / v.w;
				
			v.x = L + W * v.x * RHW;
			v.y = T - H * v.y * RHW;
			v.z = v.z * RHW;
			v.w = RHW;

			for(int t = 0; t < (FVF & FVF_TEX_MASK) >> FVF_TEX_SHIFT; t++)
			{
				v.T[t].u *= RHW;
				v.T[t].v *= RHW;
			}
		}

		renderPolygon(V, n, FVF);
	}

	void Renderer::drawPrimitive(const VertexBuffer *VB, const IndexBuffer *IB)
	{
		if(!clipper || !rasterizer || !viewport) throw INTERNAL_ERROR;
		if(!VB || !IB) throw Error("No primitive specified");

		const VertexBuffer &vb = *VB;
		const IndexBuffer &ib = *IB;
		int FVF = VB->flags;

		update();

		XVertex *X = new XVertex[vb.numVertices];

		for(int i = 0; i < VB->numVertices; i++)
		{
			XVertex &x = X[i];

			Point P(vb.Px[i], vb.Py[i], vb.Pz[i]); 

			x.x = PBVM(1, 1) * P.x + PBVM(1, 2) * P.y + PBVM(1, 3) * P.z + PBVM(1, 4);
			x.y = PBVM(2, 1) * P.x + PBVM(2, 2) * P.y + PBVM(2, 3) * P.z + PBVM(2, 4);
			x.z = PBVM(3, 1) * P.x + PBVM(3, 2) * P.y + PBVM(3, 3) * P.z + PBVM(3, 4);
			x.w = PBVM(4, 1) * P.x + PBVM(4, 2) * P.y + PBVM(4, 3) * P.z + PBVM(4, 4);

			if(FVF & FVF_DIFFUSE)
			{
				x.C.b = vb.Cb[i];
				x.C.g = vb.Cg[i];
				x.C.r = vb.Cr[i];
				x.C.a = vb.Ca[i];
			}

			if(FVF & FVF_SPECULAR)
			{
				x.L.b = vb.Lb[i];
				x.L.g = vb.Lg[i];
				x.L.r = vb.Lr[i];
				x.L.a = vb.La[i];
			}

			for(int t = 0; t < (FVF & FVF_TEX_MASK) >> FVF_TEX_SHIFT; t++)
			{
				x.T[t].u = vb.Tu[t][i];
				x.T[t].v = vb.Tv[t][i];
			}
		}

		const float L = viewport->getLeft();
		const float T = viewport->getTop() + H;

		for(i = 0; i < ib.numFaces; i++)
		{
			const IndexBuffer::Face &face = ib.face[i];
			XVertex &V1 = X[face[0]];
			XVertex &V2 = X[face[1]];
			XVertex &V3 = X[face[2]];

			XVertex *V = clipper->clipTriangle(V1, V2, V3, FVF | FVF_RHW);
			int n = clipper->getNumVertices();
			if(n < 3) continue;

			for(int i = 0; i < n; i++)
			{
				XVertex &v = V[i];

				const float RHW = 1.0f / v.w;
					
				v.x = L + W * v.x * RHW;
				v.y = T - H * v.y * RHW;
				v.z = v.z * RHW;
				v.w = RHW;

				for(int t = 0; t < (FVF & FVF_TEX_MASK) >> FVF_TEX_SHIFT; t++)
				{
					v.T[t].u *= RHW;
					v.T[t].v *= RHW;
				}
			}

			renderPolygon(V, n, FVF);
		}

		delete[] X;
	}

	const State &Renderer::status() const
	{
		static State state;

		state.setShaderFile(shaderFile);

		state.setPipelineState(depthCompareMode << 0 |
		                       depthWriteEnable << 3 |
							   cullMode         << 1);

		for(unsigned int i = 0; i < 16; i++)
		{
			state.setSamplerState(i, sampler[i].status());
		}

		return state;
	}

	void Renderer::update()
	{
		if(updateProjectionMatrix ||
		   W != viewport->getWidth() ||
		   H != viewport->getHeight())
		{
			W = viewport->getWidth();
			H = viewport->getHeight();

			const float D = 0.5f * W / tanFOV;
			const float F = farClip;
			const float N = nearClip;
			const float Q = F / (F - N);

			P = Matrix(D / W,  0,     0.5f,  0,
			           0,      D / H, 0.5f,  0,
			           0,      0,     Q,    -Q * N,
			           0,      0,     1,     0);

			PB = P * B;
			PBV = PB * V;
			PBVM = PBV * M;

			updateModelMatrix = false;
			updateViewMatrix = false;
			updateBaseMatrix = false;
			updateProjectionMatrix = false;
		}

		if(updateBaseMatrix)
		{
			PB = P * B;
			PBV = PB * V;
			PBVM = PBV * M;

			updateModelMatrix = false;
			updateViewMatrix = false;
			updateBaseMatrix = false;
		}

		if(updateViewMatrix)
		{
			PBV = PB * V;
			PBVM = PBV * M;

			updateModelMatrix = false;
			updateViewMatrix = false;
		}

		if(updateModelMatrix)
		{
			PBVM = PBV * M;

			updateModelMatrix = false;
		}

		if(updatePixelShader)
		{
			if(!shaderFile) throw Error("Pixel shader not specified");

			State state = status();

			scanline = 0;

			for(unsigned int i = 0; i < cacheSize; i++)
			{
				unsigned int j = (cacheTop - i) % cacheSize;

				if(state == shaderState[j])
				{
					if(!shaderCache[j]) throw INTERNAL_ERROR;

					scanline = shaderCache[j]->shader();
					break;
				}
			}

			if(!scanline)
			{
				cacheTop++;
				unsigned int i = cacheTop % cacheSize;

				delete shaderCache[i];
				shaderCache[i] = new PS20Shader(shaderFile);
				shaderState[i] = state;
				scanline = shaderCache[i]->shader();
			}

			if(!scanline) throw Error("Shader creation failure");

			updatePixelShader = false;
		}
	}

	void Renderer::setShader(const char *shaderFile)
	{
		delete[] this->shaderFile;
		this->shaderFile = strdup(shaderFile);

		updatePixelShader = true;
	}

	void Renderer::setTextureMap(int stage, Texture *texture)
	{
		if(stage < 0 || stage >= 16) throw Error("Texture stage index out of [0, 15] range: %d", stage);

		sampler[stage].setTextureMap(texture);
	}

	void Renderer::releaseTextures()
	{
		for(int stage = 0; stage < 16; stage++)
		{
			sampler[stage].releaseTexture();
		}
	}

	void Renderer::setTextureFilter(int stage, Sampler::FilterType textureFilter)
	{
		if(stage < 0 || stage >= 16) throw Error("Texture stage index out of [0, 15] range: %d", stage);

		updatePixelShader = sampler[stage].setTextureFilter(textureFilter);
	}

	void Renderer::setAddressingMode(int stage, Sampler::AddressingMode addressMode)
	{
		if(stage < 0 || stage >= 16) throw Error("Texture stage index out of [0, 15] range: %d", stage);

		updatePixelShader = sampler[stage].setAddressingMode(addressMode);
	}

	void Renderer::setDepthCompare(DepthCompareMode depthCompareMode)
	{
		updatePixelShader = (this->depthCompareMode != depthCompareMode);
		this->depthCompareMode = depthCompareMode;
	}

	void Renderer::setDepthWriteEnable(bool depthWriteEnable)
	{
		updatePixelShader = (this->depthWriteEnable != (int)depthWriteEnable);
		this->depthWriteEnable = depthWriteEnable;
	}

	void Renderer::setCullMode(CullMode cullMode)
	{
		updatePixelShader = (this->cullMode != cullMode);
		this->cullMode = cullMode;
	}

	void Renderer::renderPolygon(XVertex *V, int n, int FVF)
	{
		for(int i = 2; i < n; i++)
		{
			const XVertex &V1 = V[0];
			const XVertex &V2 = V[i - 1];
			const XVertex &V3 = V[i];

			float area = 0.5f * ((V1.x - V2.x) * (V3.y - V1.y) - (V1.y - V2.y) * (V3.x - V1.x));

			if(cullMode != CULL_NONE)
			{
				if(cullMode == CULL_CLOCKWISE)
				{
					if(area <= 0) continue;
				}
				else if(cullMode == CULL_COUNTER_CLOCKWISE)
				{
					if(area >= 0) continue;
				}
				else
				{
					throw INTERNAL_ERROR;
				}
			}

			rasterizer->renderTriangle(&V1, &V2, &V3, FVF);
		}
	}

	void Renderer::setFOV(float FOV)
	{
		tanFOV = tan(rad(FOV / 2));
		updateProjectionMatrix = true;
	}

	void Renderer::setNearClip(float nearClip)
	{
		this->nearClip = nearClip;
		updateProjectionMatrix = true;
	}

	void Renderer::setFarClip(float farClip)
	{
		this->farClip = farClip;
		updateProjectionMatrix = true;
	}

	void Renderer::setModelTransform(const Matrix &M)
	{
		this->M = M;
		updateModelMatrix = true;
	}

	void Renderer::setViewTransform(const Matrix &V)
	{
		this->V = V;
		updateViewMatrix = true;
	}

	void Renderer::setBaseTransform(const Matrix &B)
	{
		this->B = B;
		updateBaseMatrix = true;
	}

	void Renderer::setPixelShaderConstant(int index, const float value[4])
	{
		if(index < 0 || index >= 32) throw Error("Constant register index out of [0, 31] range: %d", index);

		PS20Shader::setConstant(index, value);
	}
}