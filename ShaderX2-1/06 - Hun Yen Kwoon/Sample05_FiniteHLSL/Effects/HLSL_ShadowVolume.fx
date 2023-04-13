float4    vObjLightPos;   // Light pos in object space
float4x4  mClip;          // World*View*Project transformation matrix


struct VS_OUTPUT
{
  float4 Pos  : POSITION; 
  float4 Col  : COLOR0;
};


VS_OUTPUT VS( 
  float4 Pos    : POSITION,
  float4 Normal : NORMAL )
{
  VS_OUTPUT Out = (VS_OUTPUT)0;
  
  // Output color to expose shadow if needed
  const float4 vCol = { 0.5f, 0.0f, 1.0f, 0.0f };
  Out.Col = vCol;
 
  // Calculate the normalized incident light ray vector
  float4 vLightRay = normalize( Pos - vObjLightPos );
  
  // Perform dot product to test whether vertex is light-facing
  float fDotResult = dot( Normal, vLightRay );
  float fMask = ( fDotResult < 0.0f ) ? 1.0f : 0.0f;
  
  // Extrude vertex by 50 units depending on fMask and output vertex.
  Out.Pos = mul( Pos + fMask * vLightRay * 300.0f, mClip );
  
  return Out;
}


// One sided stencil operations with 2 shadow volume passes
technique T0
{
  // Shadow volume back face rendering
  pass P0
  {
    // States
    ZWriteEnable        = FALSE;
    StencilEnable       = TRUE;
    ShadeMode           = 1;            // D3DSHADE_FLAT
    StencilRef          = 0x1;
    StencilMask         = 0xffffffff;
    StencilWriteMask    = 0xffffffff;
    StencilFunc         = 8;            // D3DCMP_ALWAYS;
    StencilPass         = 1;            // D3DSTENCILCAPS_KEEP;
    StencilFail         = 1;            // D3DSTENCILCAPS_KEEP;
    StencilZFail        = 7;            // D3DSTENCILOP_INCR;
    CullMode            = 2;            // D3DCULL_CW
    DepthBias           = 0.00001f;     // D3DRS_DEPTHBIAS
    
    // Shaders
    VertexShader = compile vs_1_1 VS();
    PixelShader  = NULL;
  }
  
  // Shadow volume front face rendering
  pass P1
  {
    // States
    StencilZFail        = 8;            // D3DSTENCILOP_INCR;
    CullMode            = 3;            // D3DCULL_CCW

    // Shaders
    VertexShader = compile vs_1_1 VS();
    PixelShader  = NULL;
  } 
}


// Two sided stencil operations with 1 shadow volume pass
technique T1
{
  pass P0
  {
    // States
    ZWriteEnable        = FALSE;
    StencilEnable       = TRUE;
    TwoSidedStencilMode = TRUE;
    
    ShadeMode           = 1;            // D3DSHADE_FLAT
    StencilRef          = 0x1;
    StencilMask         = 0xffffffff;
    StencilWriteMask    = 0xffffffff;

    StencilFunc         = 8;            // D3DCMP_ALWAYS;
    CCW_StencilFunc     = 8;            // D3DCMP_ALWAYS;
    StencilPass         = 1;            // D3DSTENCILCAPS_KEEP;
    StencilFail         = 1;            // D3DSTENCILCAPS_KEEP;
    StencilZFail        = 8;            // D3DSTENCILOP_INCR;
    CCW_StencilPass     = 1;            // D3DSTENCILCAPS_KEEP;
    CCW_StencilFail     = 1;            // D3DSTENCILCAPS_KEEP;
    CCW_StencilZFail    = 7;            // D3DSTENCILOP_INCR;

    DepthBias           = 0.00001f;     // D3DRS_DEPTHBIAS
    CullMode            = 1;            // D3DCULL_NONE

    // Shaders
    VertexShader = compile vs_1_1 VS();
    PixelShader  = NULL;
  
  }
}