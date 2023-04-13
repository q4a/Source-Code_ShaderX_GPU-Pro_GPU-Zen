//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
// TSprite.fx
//
//    Texture Sprites demo source code
//
//                      ShaderX2 - www.shaderx2.com
//
//
//    Author:   Sylvain Lefebvre - 04/2003
//    ------
//
//    -= Check for updates at http://www.aracknea.net =-
//
//
//    File contents:
//    -------------
//       
//       HLSL code for the Texture Sprites technique
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// texture
texture Tex0;
texture Off0;

// offsat map size
float OffsetMapRes;
// number of tiles in reference texture
float RefTexNbTiles;

// transformations
float4x4 View       : VIEW;
float4x4 World      : WORLD;
float4x4 Projection : PROJECTION;

struct VS_OUTPUT
{
    float4 Pos          : POSITION;
    float2 OffsCoords   : TEXCOORD0;
    float2 RefTexCoords : TEXCOORD1;
};

VS_OUTPUT VS(
    float3 Pos  : POSITION, 
    float2 Tex  : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    float4x4 WorldView = mul(World, View);

    float3 P = mul(float4(Pos, 1), (float4x3)WorldView);
    Out.Pos  = mul(float4(P, 1), Projection);

    Out.OffsCoords   = Tex;
	// compute reference texture coordinates from offset map texture coordinates
    Out.RefTexCoords = Tex*OffsetMapRes/RefTexNbTiles;

    return Out;
}

sampler STex0 = sampler_state
{
    Texture   = (Tex0);
    MipFilter = None;
    MinFilter = Linear;
    MagFilter = Linear;
};

sampler SOff0 = sampler_state
{
    Texture   = (Off0);
    MipFilter = None;
    MinFilter = None;
    MagFilter = None;
};

PIXELSHADER ps14Offset = 
asm
{
    ps.1.4

    texcrd r0.xyz, t1      // read texture coordinates
    texld  r1,     t0      // read offset map

	add r1.xyz, r0, r1_bx2 // add offset to tex coords

    phase

    texld r0, r1.xyz       // read reference texture
    
};

PIXELSHADER ps13Offset = 
asm
{
    ps.1.3

    tex     t0
    texbem  t1, t0
    mov     r0, t1
};

half4 transformedLookup(uniform sampler2D tex,half2 ctex,half angle,half scale)
{
	half4 c;
    half4 bkgColor=half4(1.0,1.0,1.0,1.0);

	// transform coordinates from reference texture space to tile space
	half2 gcoords=ctex*RefTexNbTiles;
	half2 uv0=frac(gcoords);           // tile space
	half2 isprite=floor(gcoords);      // sprite index (si,sj)

	// apply rotation
	half si,cs;
	sincos(angle*6.28,si,cs);
 	uv0=uv0-0.5;
	half2 uv1=half2( uv0.x*cs + uv0.y*si,
					-uv0.x*si + uv0.y*cs);
	uv1=uv1+0.5;

	// apply scaling
	uv1=uv1-0.5;
	half2 uv2=uv1/scale;
	uv2=uv2+0.5;

	// are coordinates still in sprite tile ?
	if (   uv2.x > 1.0 || uv2.x < 0.0 
	    || uv2.y > 1.0 || uv2.y < 0.0)
		c=bkgColor;	    
	else
		c=tex2D(tex,(uv2+isprite)/RefTexNbTiles);

	return c;
}


float4 ps20Offset(VS_OUTPUT In) : COLOR
{   
	float4 color;

	// read offset map
	float4 mapdata=tex2D(SOff0,In.OffsCoords);

	// unpack offset
	float2 offset=2.0*mapdata.rg-1.0;

	// apply offset
	float2 uv0=offset+In.RefTexCoords;

	float angle=mapdata.b;
	float scale=mapdata.a;

	// apply transform
    color=transformedLookup(STex0,uv0,angle,scale);
	return (color);
}

technique TSprite1_4
{
    pass P0
    {        
        MinFilter[0] = Linear;
        MagFilter[0] = Linear;
        MipFilter[0] = None;
		Texture[0] = <Tex0>;

		Texture[1] = <Off0>;

        VertexShader = compile vs_1_1 VS();
        PixelShader  = <ps14Offset>;
    }  
}

technique TSprite1_3
{
    pass P0
    {
		Texture[0] = <Off0>;

		BumpEnvMat00[1]   = 1.0;
		BumpEnvMat01[1]   = 0.0;
		BumpEnvMat10[1]   = 0.0;
		BumpEnvMat11[1]   = 1.0;
		BumpEnvLScale[1]  = 1.0;
        MinFilter[1] = None;
        MagFilter[1] = None;
        MipFilter[1] = None;
		Texture[1] = <Tex0>;

        VertexShader = compile vs_1_1 VS();
        PixelShader  = <ps13Offset>;
    }  
}

technique TSprite2_0
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 ps20Offset();
    }  
}

