//HLSL code for calculating interior points of a number of triangles.

// enough for a cube (12 triangles)
#define NUM_BASE_TRIANGLE 12

vector VertexPos[3 * NUM_BASE_TRIANGLE];
matrix MVP;

struct VS_OUT
{
	float4 pos : POSITION;
	float4 col : COLOR;
};

VS_OUT mainVS( float3 vertexStream : POSITION0  ) 
{
	VS_OUT outp;
	
	float i =  vertexStream.x;
	float j =  vertexStream.y;
	float k = 1.0 - i - j;
	float baseIndex =  vertexStream.z * 256;
	float3 pos =	i * VertexPos[ (baseIndex * 3) + 0 ] + 
					j * VertexPos[ (baseIndex * 3) + 1 ] + 
					k * VertexPos[ (baseIndex * 3) + 2 ];

	// transform position
	outp.pos =  mul( float4(pos,1), MVP );
	outp.col = vector(i,j,k,1);

	return outp;
}

technique T0
{
	pass P0
	{ 
		VertexShader = compile vs_1_1 mainVS();
	}
}
