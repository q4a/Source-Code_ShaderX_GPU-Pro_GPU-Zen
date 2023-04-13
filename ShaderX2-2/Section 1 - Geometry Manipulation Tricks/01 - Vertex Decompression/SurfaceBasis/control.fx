//HLSL code for simple transforming and a fixed colour.

matrix MVP;

struct VS_OUT
{
	float4 pos : POSITION;
	float4 col : COLOR;
};

VS_OUT mainVS( float4 pos : POSITION0  ) 
{
	VS_OUT outp;
	
	// transform position
	outp.pos =  mul( pos, MVP );
	outp.col = vector(1,1,1,1);

	return outp;
}

technique T0
{
	pass P0
	{ 
		VertexShader = compile vs_1_1 mainVS();
	}
}
