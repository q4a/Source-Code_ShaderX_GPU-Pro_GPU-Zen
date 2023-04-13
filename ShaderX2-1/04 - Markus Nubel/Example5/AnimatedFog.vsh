
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
	float2 Tex0  	: TEXCOORD0;
	float  Fog		: FOG;
};

matrix matWorldViewProj : register(c0);
matrix matWorldView		: register(c4);
matrix matWorld			: register(c8);
vector fFog				: register(c12);
vector vCamera				: register(c13);

VS_OUTPUT main(const VS_INPUT Input)
{
	float4		clpPos;

	// Init output
	VS_OUTPUT	Out = (VS_OUTPUT) 0; 

	// Calculate the clip space position
    clpPos			= mul(Input.Position, matWorldViewProj);
	Out.Position	= clpPos;

	// Simply pass on the texture coords and the vertex color
 	Out.Tex0.xy		= Input.Tex0.xy;
	Out.Diffuse		= Input.Diffuse;

	// Get fog parameter
	float	fAnim			= fFog.x;
	float	fFogEnd			= fFog.y;
	float	fDensity		= fFog.z;

	// Calculate the distance. (Same as for exp-fog)
	// Camera space z coords are scaled to have a value of 4 at distance FogEnd
    float4	camPos	= mul(Input.Position, matWorldView);
	float	fDist	= camPos.z/fFogEnd*4;

	// Exp fog calculation
	float fExpFog	= exp(-fDist*fDensity);

	// Animation is calculated based on world coordinates
    float4 worldPos		= mul(Input.Position, matWorld);

	// Do the animation: -(1+0.5*cos(5*PI*z)+0.2*cos(7Pi*(z+0.1*x))+0.2*cos(5*PI*(z-0.05*x))+0.1*cos(PI+x)*cos(PI*y/2))
	float k = -1-0.5*cos(5*3.14*worldPos.z+fAnim)-0.2*cos(7*3.14*(worldPos.z+0.1*worldPos.x))-0.2*cos(5*3.14*(worldPos.z-0.05*worldPos.x))-0.1*cos(3.14*worldPos.x)*cos(3.14*worldPos.y/2);

	// Final fog is addition of exp and animation
	float f	= fExpFog + (camPos.z/fFogEnd)/4.0f*k;

	// Set the fog value
	Out.Fog = f;
 
    return Out;
}





