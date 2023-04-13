// -------------------------------------------------------------
// Path volume
// 
// Copyright (c) 2002-2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// Global access
// -------------------------------------------------------------
float4x4 mWVPbg;	// Local-projection
float4x4 mWVP;		// Local-projection
float4	 vVelocity;

texture DecaleMap;
sampler DecaleSamp = sampler_state
{
    Texture = <DecaleMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
texture BurnMap;
sampler BurnSamp = sampler_state
{
    Texture = <BurnMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
texture CatMap;
sampler CatSamp = sampler_state
{
    Texture = <CatMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

// -------------------------------------------------------------
// data from vertex shader to pixel shader
// -------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
};
struct BLUR_VS_OUTPUT
{
	float4 Pos		: POSITION;
	float2 Tex0		: TEXCOORD0;
	float2 Tex1		: TEXCOORD1;
	float2 Tex2		: TEXCOORD2;
	float2 Tex3		: TEXCOORD3;
	float4 Color	: COLOR0;
};
struct FINAL_VS_OUTPUT
{
	float4 Pos		: POSITION;
	float2 Decale	: TEXCOORD0;
	float2 TexPos	: TEXCOORD1;
};

struct FIRE_VS_OUTPUT
{
	float4 Pos		: POSITION;
	float2 TexDummy	: TEXCOORD0;
	float2 TexPos	: TEXCOORD1;
	float2 TexCat	: TEXCOORD2;
};

// -------------------------------------------------------------
// vertex shader program
// -------------------------------------------------------------
VS_OUTPUT VS (
      float4 Pos    : POSITION,
      float3 Normal : NORMAL
){
    VS_OUTPUT Out = (VS_OUTPUT)0;
	
    // Extended back the reverse side seeing from light
    float VN = dot( Normal, vVelocity );
    float scale = (0<VN) ? 0.0f : 1.0f;
    
    // Position
    Pos.xyz -= 0.001f*Pos;// Not becomeing stripes
    Out.Pos = mul( Pos - scale * vVelocity, mWVPbg );
    
	Out.Color = 1;
    
    return Out;
}
// -------------------------------------------------------------
VS_OUTPUT VS2 (
      float4 Pos    : POSITION,
      float3 Normal : NORMAL
){
    VS_OUTPUT Out = (VS_OUTPUT)0;
	
    // Extended back the reverse side seeing from light
    float VN = dot( Normal, vVelocity );
    float scale = (0<VN) ? 0.0f : 1.0f;
    
    // Position
    Pos.xyz -= 0.001f*Pos;// Not becomeing stripes
    Out.Pos = mul( Pos - scale * vVelocity, mWVPbg );

	Out.Color = 0;
    
    return Out;
}
// -------------------------------------------------------------
BLUR_VS_OUTPUT BlurVS (
      float4 Pos    : POSITION,
      float2 Tex    : TEXCOORD0	
){
    BLUR_VS_OUTPUT Out = (BLUR_VS_OUTPUT)0;
	
    // position
    Out.Pos = Pos;
	
	// texture coordinates
	float MAP_SIZE = 512.0f;
	Out.Tex0 = Tex+float2(-1.0f/MAP_SIZE, -1.0f/MAP_SIZE);
	Out.Tex1 = Tex+float2(+1.0f/MAP_SIZE, -1.0f/MAP_SIZE);
	Out.Tex2 = Tex+float2(-1.0f/MAP_SIZE, +1.0f/MAP_SIZE);
	Out.Tex3 = Tex+float2(+1.0f/MAP_SIZE, +1.0f/MAP_SIZE);
    
    return Out;
}
// -------------------------------------------------------------
FIRE_VS_OUTPUT FireVS (
      float4 Pos    : POSITION,
      float2 Tex    : TEXCOORD0
){
    float4 pos;
    FIRE_VS_OUTPUT Out = (FIRE_VS_OUTPUT)0;
	
    // position
    pos = mul( Pos, mWVP );
    Out.Pos = pos;
    
	Out.TexCat.x = 0.5f*pos.x/pos.w+0.5f;
	Out.TexCat.y =-0.5f*pos.y/pos.w+0.5f;
    
    pos = mul( Pos, mWVPbg );
	Out.TexPos.x = 0.5f*pos.x/pos.w+0.5f;
	Out.TexPos.y =-0.5f*pos.y/pos.w+0.5f;
    
    return Out;
}
// -------------------------------------------------------------
float4 FirePS ( FIRE_VS_OUTPUT In ) : COLOR0
{
	float4 dummy= tex2D(DecaleSamp, In.TexCat );
	float4 burn = tex2D(BurnSamp,   In.TexPos );
	float4 cat  = tex2D(CatSamp,    In.TexCat );
	
	return cat*burn;
}
// -------------------------------------------------------------
FINAL_VS_OUTPUT FinalVS (
      float4 Pos    : POSITION,
      float2 Tex    : TEXCOORD0
){
    FINAL_VS_OUTPUT Out = (FINAL_VS_OUTPUT)0;
	
    // position
    Out.Pos = mul( Pos, mWVP );
    
	// decale
	Out.Decale = Tex;
	
    float4 pos = mul( Pos, mWVPbg );
	Out.TexPos.x = 0.5f*pos.x/pos.w+0.5f;
	Out.TexPos.y =-0.5f*pos.y/pos.w+0.5f;
    
    return Out;
}
// -------------------------------------------------------------
float4 FinalPS ( FINAL_VS_OUTPUT In ) : COLOR0
{
	float4 decale = tex2D(DecaleSamp, In.Decale );
	float4 burn   = tex2D(BurnSamp,   In.TexPos );
	
	return decale-0.3f*burn;
}
// -------------------------------------------------------------
// technique
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // Shader
        VertexShader = compile vs_1_1 VS();
        PixelShader  = NULL;
    }
    pass P1
    {
        // Shader
        VertexShader = compile vs_1_1 VS2();
        PixelShader  = NULL;
    }
    pass P2
    {
        // Shader
        VertexShader = compile vs_1_1 BlurVS();
        PixelShader  = NULL;
        
        // Sampler state
        ADDRESSU[0] = Clamp;
        ADDRESSU[1] = Clamp;
        ADDRESSU[2] = Clamp;
        ADDRESSU[3] = Clamp;
        ADDRESSV[0] = Clamp;
        ADDRESSV[1] = Clamp;
        ADDRESSV[2] = Clamp;
        ADDRESSV[3] = Clamp;
    }
    pass P3
    {
        // Shader
        VertexShader = compile vs_1_1 FireVS();
        PixelShader  = compile ps_1_1 FirePS();
        
        // Sampler
        Sampler[0] = (DecaleSamp);
        Sampler[1] = (BurnSamp);
        Sampler[2] = (CatSamp);

    }
    pass P4
    {
        // Shader
        VertexShader = compile vs_1_1 FinalVS();
        PixelShader  = compile ps_1_1 FinalPS();
        
        // Sampler
        Sampler[0] = (DecaleSamp);
        Sampler[1] = (BurnSamp);
    }
}
