
struct VS_INPUT
{
    float4 Position     : POSITION;
    float3 Diffuse		: COLOR0;
    float2 Tex0			: TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position : POSITION;
	float3 Diffuse	: COLOR;
	float3 FogVal	: COLOR1;
	float2 Tex0  	: TEXCOORD0;
};

matrix matWorldViewProj : register(c0);
matrix matWorldView		: register(c4);
matrix matWorld			: register(c8);
vector fFog				: register(c12);


VS_OUTPUT main(const VS_INPUT Input)
{
	float4		clpPos, camPos;

	// Init output
	VS_OUTPUT	Out = (VS_OUTPUT)0; 

	// Calculate the clip space position
	clpPos			= mul(Input.Position, matWorldViewProj);
	Out.Position	= clpPos;

	// Simply pass on the texture coords and the vertex color
 	Out.Tex0.xy		= Input.Tex0.xy;
	Out.Diffuse		= Input.Diffuse;

	// Calculate vertex position in camera space
	camPos			= mul(Input.Position, matWorldView);

	// Extract the fog parameters
	float fDensity	= fFog.x;
	float fFogEnd	= fFog.y;

	// Calculate the distance. 
	// Camera space z coords are scaled to have a value of 4 at distance: FogEnd
	float fDist	= camPos.z/fFogEnd*4;

	// Exp calculation
	float f	= exp(-fDist*fDensity);

	// Set the fog value
	Out.FogVal.x	= f;	// Passed to PixelShader using color register

	return Out;
}





