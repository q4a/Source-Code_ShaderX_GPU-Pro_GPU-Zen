//HLSL code for simple transforming and a fixed colour.

matrix MVP;

// enough for a cube (8 vertices)
#define NUM_BASE_VERTICES 8

vector VertexPos[NUM_BASE_VERTICES];
vector VertexNorm[NUM_BASE_VERTICES];

struct VS_IN
{
	float2 barycentric	: POSITION0;
	float3 indices		: POSITION1;
};

struct VS_OUT
{
	float4 pos : POSITION;
	float4 col : COLOR;
};

VS_OUT mainVS( VS_IN vertexStream ) 
{
	VS_OUT outp;
	
	float i =  vertexStream.barycentric.x;
	float j =  vertexStream.barycentric.y;
	float k = 1.f - i - j;
	float i0 =  vertexStream.indices.x * 256;
	float i1 =  vertexStream.indices.y * 256;
	float i2 =  vertexStream.indices.z * 256;

	// position and normals of this triangle
	float3 v0 =	VertexPos[ i0 ];
	float3 n0 = VertexNorm[ i0 ];

	float3 v1 = VertexPos[ i1 ];
	float3 n1 = VertexNorm[ i1 ];

	float3 v2 = VertexPos[ i2 ];
	float3 n2 = VertexNorm[ i2 ];

	float3 pos, norm;
	// linear pos
	pos = (i * v0) + (j * v1) + (k * v2);
	// linear normal
	norm = (i * v0) + (j * v1) + (k * v2);
	norm = normalize( norm );
	
	// this is only to demo that the normal is 
	// smooth, and the displacement mapping won't
	// produce cracks. Essentially we have a displacement
	// map of 0.3 everywhere
	pos += norm * 0.3f;

	// transform position
	outp.pos =  mul( float4(pos,1), MVP );
	outp.col = float4( (norm*0.5)+0.5,1);

	return outp;
}

technique T0
{
	pass P0
	{ 
		VertexShader = compile vs_1_1 mainVS();
	}
}
