#ifndef __INSTANCING_H
#define __INSTANCING_H

#include "instancing.fxh"
#include "moduleoperations.fx"

// drawing stuff

struct light_struct
{
    float4 direction;
    float4 color;
};

cbuffer cimmutable
{
    light_struct g_lights[4] = { 
                    { float4(0.620275,  0.683659, 0.384537, 1),  float4(0.75, 0.599, 0.405, 1) },
                    { float4(0.063288, -0.987444, 0.144735, 1),  float4(0.192, 0.273, 0.275, 1) },
                    { float4(0.23007,   0.785579, -0.574422, 1),  float4(0.300, 0.292, 0.223, 1) },
                    { float4(-0.620275,  -0.683659, -0.384537, 1),  float4(0.0, 0.0, 0.1, 1) }
                    };
};

//******************
// INSTANCING PHASE
//******************

struct instancingVSInput
{
	// per vertex
	float3 basic_pos :	POSITION;
	float3 normal :		NORMAL;
	
	// per instance
	float4 inst_pos :	INSTANCE_POSITION;
	float size :		SIZE;
	float4 orientation : ORIENTATION;
};

struct instancingPSInput
{
	float4 pos : SV_POSITION;
    float3 basic_pos : TEXTURE0;
    float3 normal : TEXTURE1;
};

// VS of the instancing phase
instancingPSInput vsInstance( instancingVSInput input )
{
	instancingPSInput output;
	
	output.basic_pos = input.basic_pos;
	output.pos.xyz = input.basic_pos;
	output.pos.w = 1;	
	
	output.pos.x -= 0.5;
	quaternionRotate( output.pos.xyz, input.orientation );
	output.pos.x += 0.5;
	
	output.pos.xyz *= input.size;
	
	output.pos.xyz += input.inst_pos.xyz;
	output.pos = mul(output.pos, viewProjMatrix);
	
	output.normal = mul(input.normal,modelMatrixInverse);
	output.normal = normalize(output.normal);
	
	return output;
};

float4 psInstance( instancingPSInput input ) : SV_TARGET
{
	float4 color = float4(0,0,0,0);
	float4 ownColor = float4(55.0/255.0,220.0/255.0,70.0/255.0,1);
    
    // add the contributions of 4 directional lights
    for( int i=0; i<4; i++ )
    {
        color += saturate( dot(g_lights[i].direction,input.normal) )*g_lights[i].color;
    }
    
    color *= ownColor;
    color.a = 1;
    
	return  color;
}

RasterizerState NoCull
{
	CullMode = NONE;
};

BlendState NoBlend
{ 
    BlendEnable[0] = False;
};

DepthStencilState EnableDepthTestWrite
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};


technique10 instance
{
    pass P0
    {
        SetVertexShader ( CompileShader( vs_4_0, vsInstance() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, psInstance() ) );
     
        SetRasterizerState( NoCull );
        SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepthTestWrite, 0 );
    }
}


#endif