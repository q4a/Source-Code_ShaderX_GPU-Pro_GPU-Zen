
// Shader input structure
struct VS_INPUT
{
    float4 Position     : POSITION;
    float3 Diffuse		: COLOR0;
    float2 Tex0			: TEXCOORD0;
};

// Shader output structure
struct VS_OUTPUT
{
    float4 Position : POSITION;
	float3 Diffuse	: COLOR;
	float2 Tex0  	: TEXCOORD0;
	float  Fog		: FOG;
};

// Constants
matrix matWorldViewProj : register(c0);		// World-View-Projection matrix
matrix matWorldView		: register(c4);		// World-View matrix
vector fFog				: register(c10);	// Fog parameters:
											// fFog.x: fog end value
											// fFog.y: fog start value
											// fFog.z: not used
											// fFog.w: not used


VS_OUTPUT main(const VS_INPUT Input)
{
	float4		clpPos, camPos;

	// Init output
	VS_OUTPUT	Out = (VS_OUTPUT) 0; 

	// Retrieve fog parameters
	float fFogEnd	= fFog.x;
	float fFogStart	= fFog.y;

	// Calcualte the clip space position
    clpPos			= mul(Input.Position, matWorldViewProj);
	Out.Position	= clpPos;

	// Simply pass on the texture coords and the vertex color
 	Out.Tex0.xy		= Input.Tex0.xy;
	Out.Diffuse		= Input.Diffuse;

	// Calculate vertex position in camera space
    camPos			= mul(Input.Position, matWorldView);

	// Calculate the linear fog factor
	float fFogRange		= fFogEnd-fFogStart;
	float fVertexDist	= fFogEnd - camPos.z;
	float f				= clamp((fVertexDist/fFogRange), 0.0f, 1.0f);

	//  Write the calculated factor to the FOG register
	Out.Fog = f;
  
    return Out;
}





