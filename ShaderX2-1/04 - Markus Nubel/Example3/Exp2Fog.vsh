
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
    float4 Tex1		: TEXCOORD1;
	float  Fog		: FOG;
};

matrix matWorldViewProj : register(c0);
matrix matWorldView		: register(c4);
matrix matWorld			: register(c8);
vector fFog				: register(c12);
vector vCamera				: register(c13);


VS_OUTPUT main(const VS_INPUT Input)
{
	float4		clpPos, worldPos;

	// Init output
	VS_OUTPUT	Out = (VS_OUTPUT) 0; 

	// Calculate the clip space position
	clpPos			= mul(Input.Position, matWorldViewProj);
	Out.Position	= clpPos;

	// Simply pass on the texture coords and the vertex color
 	Out.Tex0.xy		= Input.Tex0.xy;
	Out.Diffuse		= Input.Diffuse;

	// Extract the fog parameters
	float fDensity	= fFog.x;
	float fFogEnd	= fFog.y;

	// Calculate the vertex position in world space
	worldPos		= mul(Input.Position, matWorld);

	// Calculate the distance to the viewer in world space
	float fDistance	= distance(worldPos, vCamera);
	// The distance is scaled to have a value of 4 at distance: FogEnd
	float fDist		= fDistance/fFogEnd*4;

	// Exp squared calculation
	float f	= exp(-(fDist*fDensity)*(fDist*fDensity));

	// Set the fog value
	Out.FogVal.x	= f;	// Passed to PixelShader using color register

	return Out;
}





