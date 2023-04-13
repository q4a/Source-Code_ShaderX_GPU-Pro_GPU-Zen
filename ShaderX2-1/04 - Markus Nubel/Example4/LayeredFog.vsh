
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
	float  Fog		: FOG;
};

matrix matWorldViewProj : register(c0);
matrix matWorldView		: register(c4);
matrix matWorld			: register(c8);
vector fFog				: register(c12);
vector vCamera				: register(c13);


VS_OUTPUT main(const VS_INPUT Input)
{
	float4		clpPos, camPos, worldPos;
	float		fDistance;

	// Init output
	VS_OUTPUT	Out = (VS_OUTPUT)0; 

	// Calculate the clip space position
	clpPos			= mul(Input.Position, matWorldViewProj);
	Out.Position	= clpPos;

	// Simply pass on the texture coords and the vertex color
 	Out.Tex0.xy		= Input.Tex0.xy;
	Out.Diffuse		= Input.Diffuse;

	// Get fog parameter
	float fFogTop	= fFog.x;
	float fFogEnd	= fFog.y;
	float fFogRange	= fFog.x;

	// Calculate the world position
	worldPos		= mul(Input.Position, matWorld);

	// Calculate the distance to the viewer
	fDistance	= distance(worldPos, vCamera);

	
	// Project both points into the x-z plane
	float4	vCameraProj, vWorldProj;
	vCameraProj		= vCamera;
	vCameraProj.y		= 0;
	vWorldProj		= worldPos;
	vWorldProj.y	= 0;
	
	// Scaled distance calculation in x-z plane
	float fDeltaD			= distance(vCameraProj, vWorldProj)/fFogEnd*2.0f;

	// Height based calculations
	float fDeltaY, fDensityIntegral;
	if(vCamera.y > fFogTop)
	{
		if (worldPos.y < fFogTop)
		{
			fDeltaY = (fFogTop - worldPos.y)/fFogRange*2;
			fDensityIntegral = (fDeltaY * fDeltaY * 0.5f);
		}
		else
		{
			fDeltaY = 0.0f;
			fDensityIntegral = 0.0f;
		}
	}
	else
	{
		if (worldPos.y < fFogTop)
		{
			float fDeltaA = (fFogTop - vCamera.y)/fFogRange*2;
			float fDeltaB = (fFogTop - worldPos.y)/fFogRange*2;
			fDeltaY =abs(fDeltaA -fDeltaB);
			fDensityIntegral = abs((fDeltaA * fDeltaA * 0.5f) - (fDeltaB * fDeltaB * 0.5f));

		}
		else
		{
			fDeltaY = abs(fFogTop - vCamera.y)/fFogRange*2;
			fDensityIntegral = abs(fDeltaY * fDeltaY * 0.5f);
		}
	}

	float fDensity;
	if (fDeltaY != 0.0f)
	{
		fDensity = (sqrt(1.0f + ((fDeltaD / fDeltaY) * (fDeltaD / fDeltaY)))) * fDensityIntegral;
	}
	else
		fDensity = 0.0f;

	float f= exp(-fDensity);

	// Set the fog value
	Out.FogVal.x	= f;			// Passed to PixelShader using color register

    return Out;
}





