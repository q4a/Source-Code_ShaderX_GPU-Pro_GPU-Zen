textureCUBE	texCubeMap;

matrix matWorld;
matrix matView;
matrix matProject;
matrix matTexture;

sampler BaseMapSampler = sampler_state
{
	Texture = (texCubeMap);

	MinFilter = Point;
	MagFilter = Point;
	AddressU  = Clamp;
	AddressV  = Clamp;
	AddressW  = Clamp;
};

float3 sampleCube(float3 fPos)
{
	float4 tmp = texCUBE(BaseMapSampler, fPos);
	return tmp.rgb * exp2(tmp.a * 255.0 - 128);
}

float4 hlsl_filtered_cube_shader(float3 uvw : TEXCOORD0) : COLOR
{
	float3 textureSize = float3(32, 32, 32);
	float3 textureSizeDiv = float3(0.03125, 0.03125, 0.03125);

	float3 halfPixel   = float3(0.5, 0.5, 0.5);
	float3 oneConst    = float3(1.0, 1.0, 1.0);

	// multiply coordinates by the texture size
	float3 texPos = uvw * textureSize;
	
	// compute first integer coordinates
	float3 texPos0 = floor(texPos + halfPixel);

	// compute second integer coordinates
	float3 texPos1 = texPos0 + oneConst;

	// perform division on integer coordinates
	texPos0 = texPos0 * textureSizeDiv;
	texPos1 = texPos1 * textureSizeDiv;

	// compute contributions
	float3 blend = frac(texPos + halfPixel);

	// construct 8 new coordinates
	float3 texPos000 = texPos0;
	float3 texPos001 = float3(texPos0.x, texPos0.y, texPos1.z);
	float3 texPos010 = float3(texPos0.x, texPos1.y, texPos0.z);
	float3 texPos011 = float3(texPos0.x, texPos1.y, texPos1.z);
	float3 texPos100 = float3(texPos1.x, texPos0.y, texPos0.z);
	float3 texPos101 = float3(texPos1.x, texPos0.y, texPos1.z);
	float3 texPos110 = float3(texPos1.x, texPos1.y, texPos0.z);
	float3 texPos111 = texPos1;

	// sample cube map 
	float3 C000 = sampleCube(texPos000);
	float3 C001 = sampleCube(texPos001);
	float3 C010 = sampleCube(texPos010);
	float3 C011 = sampleCube(texPos011);
	float3 C100 = sampleCube(texPos100);
	float3 C101 = sampleCube(texPos101);
	float3 C110 = sampleCube(texPos110);
	float3 C111 = sampleCube(texPos111);

	// compute final pixel value by lerping everything
	float3 C = lerp(
	                lerp(lerp(C000, C010, blend.y), 
	                     lerp(C100, C110, blend.y), 
	                     blend.x), 
	                lerp(lerp(C001, C011, blend.y), 
	                     lerp(C101, C111, blend.y), 
	                     blend.x), 
	                blend.z);

	return float4(C.r, C.g, C.b, 1.0f);
}

technique Cube
{
	  pass P0
	  {
			VertexShader = null;
			PixelShader = compile ps_2_0 hlsl_filtered_cube_shader();

			WorldTransform[0] = <matWorld>;
			ViewTransform = <matView>;
			ProjectionTransform = <matProject>;
			TextureTransform[0] = <matTexture>;

			Texture[0] = <texCubeMap>;
			
			TexCoordIndex[0] = CameraSpaceReflectionVector;
			TextureTransformFlags[0] = Count3;
	  }
}

technique CubeStandardBilinearFiltering
{
	pass P0
	{
		WorldTransform[0] = <matWorld>;
		ViewTransform = <matView>;
		ProjectionTransform = <matProject>;
		TextureTransform[0] = <matTexture>;

		Texture[0] = <texCubeMap>;
			
		MinFilter[0] = Linear;
		MagFilter[0] = Linear;

		AddressU[0] = Clamp;
		AddressV[0] = Clamp;
		AddressW[0] = Clamp;

		ColorOp[0] = SelectArg1;
		ColorArg1[0] = Texture;

		TexCoordIndex[0] = CameraSpaceReflectionVector;
		TextureTransformFlags[0] = Count3;
	}
}
