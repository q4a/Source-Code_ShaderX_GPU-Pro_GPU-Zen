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
float4	 vVector;
float	 fDisplacement;

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

texture ParticleMap;
sampler ParticleSamp = sampler_state
{
    Texture = <ParticleMap>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

texture NormalMap;
sampler NormalSamp = sampler_state
{
    Texture = <NormalMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

texture CoordMap;
sampler CoordSamp = sampler_state
{
    Texture = <CoordMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

texture VelocityMap;
sampler VelocitySamp = sampler_state
{
    Texture = <VelocityMap>;
    MinFilter = POINT;
    MagFilter = POINT;
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

struct PARTICLE_VS_OUTPUT
{
    float4 Pos			: POSITION;
	float2 Tex0			: TEXCOORD0;
	float2 Tex1			: TEXCOORD1;
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

struct REFLECT_VS_OUTPUT
{
    float4 Pos			: POSITION;
	float2 Tex0			: TEXCOORD0;
	float2 Tex1			: TEXCOORD1;
};

// -------------------------------------------------------------
// Path volume
// 
//  c0 - c3 - vertex transformation
//  c4.x    - displacement degree
//  c5      - velocity of the model
//  c12     - (0.0, 0.999, 1.0, 2.0)
// -------------------------------------------------------------
VertexShader VS_CCW = asm
{
    vs_1_1
    
    def c12, 0.0f ,0.999f, 1.0f, 2.0f
    
    dcl_position0 v0
    dcl_normal0   v1
    dcl_sample0   v3
    
    dp3 r0.w, c5, v1		// float VN = dot( Normal, vVelocity );
    slt r0.w, c12.x, r0.w	// float scale = (0<VN) ? 0.0f : 1.0f;
    mul r0,   c5, r0.w		// 
    mad r0, v0, c12.y, -r0	// Pos = (Pos-0.001f*Pos) - scale * vVelocity
    
    mad r11, v3, c12.w, -c12.z
    mad r0, r11, c4.x, r0   // add displacement map
    mov r0.w, v0.w          // w=1
    m4x4 oPos, r0, c0       // transform
    
    mov oD0, c12.z      // output color
};

// -------------------------------------------------------------
// Path volume 2
// 
//  c0 - c3 - vertex transformation
//  c4.x    - displacement degree
//  c5      - velocity of the model
//  c12     - (0.0, 0.999, 1.0, 2.0)
// -------------------------------------------------------------
VertexShader VS_CW = asm
{
    vs_1_1
    
    def c12, 0.0f ,0.999f, 1.0f, 2.0f
    
    dcl_position0 v0
    dcl_normal0   v1
    dcl_sample0   v3
    
    dp3 r0.w, c5, v1		// float VN = dot( Normal, vVelocity );
    slt r0.w, c12.x, r0.w	// float scale = (0<VN) ? 0.0f : 1.0f;
    mul r0,   c5, r0.w		// 
    mad r0, v0, c12.y, -r0	// Pos = (Pos-0.001f*Pos) - scale * vVelocity
    
    mad r11, v3, c12.w, -c12.z
    mad r0, r11, c4.x, r0   // add displacement map
    mov r0.w, v0.w          // w=1
    m4x4 oPos, r0, c0       // transform
    
    mov oD0, c12.x          // output color
};

// -------------------------------------------------------------
// Update particle map
// -------------------------------------------------------------
float4 ParticlePS(PARTICLE_VS_OUTPUT In) : COLOR
{   
    float4 Color;
	
	Color  = tex2D( ParticleSamp, In.Tex0 )
			 + 2.0f*tex2D( ParticleSamp, In.Tex1 )-1.004;
	
    return Color;
}
// -------------------------------------------------------------
// Blur
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
// Displacement mapping
// 
//  c0 - c3 - vertex transformation
//  c4.x    - displacement degree
//  c12     - (0.0, 0.5, 1.0, 2.0)
// -------------------------------------------------------------
VertexShader DisplacementVS = asm
{
    vs_1_1
    
    dcl_position0 v0
    dcl_normal0   v1
    dcl_texcoord0 v2
    dcl_sample0   v3
    
    def c12, 0.0f ,0.5f, 1.0f, 2.0f
    
    mad r0, v3, c12.w, -c12.z
    mul r0, r0, c4.x    // mul a degree of the displacement map
    add r0, v0, r0      // add displacement to the vertex coordinate
    mov r0.w, v0.w      // w=1
    m4x4 oPos, r0, c0   // transform
    
    mov oT0, v2         // Texture coordinate
};
// -------------------------------------------------------------
PIXELSHADER DisplacementPS = asm
{
    ps.1.1
	
	tex t0
	
	mov r0, t0
};
// -------------------------------------------------------------
VS_OUTPUT CoordVS (
        float4 Pos    : POSITION
      , float2 Tex    : TEXCOORD0
      , float3 Normal : NORMAL
){
    VS_OUTPUT Out = (VS_OUTPUT)0;
	
    // position
    Out.Pos = mul( Pos, mWVPbg );
    
    // Color (Texture coordinate)
	Out.Color.x = 0.5f*Tex.x+0.5f;
	Out.Color.y =-0.5f*Tex.y+0.5f;
	Out.Color.z = 1.0f;
    
    return Out;
}
// -------------------------------------------------------------
VS_OUTPUT NormalVS (
        float4 Pos    : POSITION
      , float3 Normal : NORMAL
      , float2 Tex    : TEXCOORD0
){
    VS_OUTPUT Out = (VS_OUTPUT)0;
	
    // position
    Out.Pos.x =  2.0f * Tex.x - 1.0f;
    Out.Pos.y = -2.0f * Tex.y + 1.0f;
    Out.Pos.z = 0.5f;
    Out.Pos.w = 1.0f;
    
    // Color (Normal vector)
    Out.Color.xyz = 0.5f*Normal+0.5f;
    
    return Out;
}
// -------------------------------------------------------------
float4 ReflectPS ( REFLECT_VS_OUTPUT In ) : COLOR0
{
	float4 O;
	
	float4 velocity = 2.0f*tex2D( VelocitySamp, In.Tex1 )-1.0f;
	float4 coord = tex2D( CoordSamp, In.Tex0 );
	float  pixels = coord.z;
	coord /= pixels+0.0000001f;
	
	float4 normal = 2.0f*tex2D( NormalSamp, coord )-1.0f;
	
	O = -2.0f*dot(normal.xyz, velocity.xyz) * normal;
	
	if(O.y<0)O.y=0;// Does not dive into the bottom of the world

	if(pixels<0.0000001f) O = 0;
	
	return 0.5f*O + 0.5f
		+0.0021f;// I can't understand, but correct the curve
}
// -------------------------------------------------------------
// technique
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // Shader
        VertexShader = <VS_CCW>;
        VertexShaderConstantF[0]  = (mWVPbg);
        VertexShaderConstantF[4]  = (fDisplacement);
        VertexShaderConstantF[5]  = (vVelocity);
        PixelShader  = NULL;
    }
    pass P1
    {
        // Shader
        VertexShader = <VS_CW>;
        VertexShaderConstantF[0]  = (mWVPbg);
        VertexShaderConstantF[4]  = (fDisplacement);
        VertexShaderConstantF[5]  = (vVelocity);
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
    pass P5// Update particle map
    {
        // Shader
        PixelShader  = compile ps_2_0 ParticlePS();
    }
    pass P6// Displacement transformation
    {
        // Shader
        VertexShader = <DisplacementVS>;
        PixelShader  = <DisplacementPS>;
        VertexShaderConstantF[4]  = (fDisplacement);
    }
    pass P7// Create Texture coordinate
    {
        // Shader
        VertexShader = compile vs_1_1 CoordVS();
        PixelShader  = NULL;
    }
    pass P8// Create Normal Map
    {
        // Shader
        VertexShader = compile vs_1_1 NormalVS();
        PixelShader  = NULL;
    }
    pass P9// Create Acceleration
    {
        // Shader
        PixelShader  = compile ps_2_0 ReflectPS();
    }
}
