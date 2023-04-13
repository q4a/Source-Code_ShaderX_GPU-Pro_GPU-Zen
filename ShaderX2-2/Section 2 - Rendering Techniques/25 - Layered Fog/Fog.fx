// Sample effect file

// 1st pass - render the castle with a texture
technique tec0
{
    pass p0
    {
		VertexShader = asm
		{
			vs.1.1
		
			dcl_position0	v0
			dcl_normal0		v3
			dcl_texcoord0	v7
			
			m4x4 oPos, v0, c0	
			mov oT0.xy, v7
		};		
		
		
		PixelShader = asm
		{
			ps.1.1
			
			tex t0
			
			mov r0, t0
		};
    }
}


// 2nd pass - render the fog layer
technique tec1
{
    pass p0
    {
		VertexShader = asm
		{
			vs.1.1
			
			// c0 - c3 : world * view * projection matrix
			// c4 - c7 : world matrix
			
		
			dcl_position0	v0
			dcl_normal0		v3
			dcl_texcoord0	v7
			
			// world view proj
			m4x4 oPos, v0, c0
			
			// texcoord			
			mov oT0.xy, v7
			
			// world y position
			dp4 r0, v0, c5
			
			// constant 8 = (FogHighRange ,InvFogDelta, 0.0f , 1.0f)
			sub r0, c8.xxxx, r0  	// (high - y)
			mul oT1, r0, c8.yyyy 	// output = (high - y) * InvFogDelta

			
		};		
		
		
		PixelShader = asm
		{
			ps.1.1
			
			tex t0
			texcoord t1
						
			// c2 = red mask (1.0f , 0.0f, 0.0f, 0.0f)
			dp3		r0, t1, c2	// copy the red component everywhere
			// r0 = (vd, vd, vd, vd)

			// cd = camera density
			// c1 = (cd, cd, cd, cd)
			add_sat	r0, r0, c1	// VertexDensity + CameraDensity

			// c0 = fog color
			mul		r0, r0, c0	// c0.a = fog density
			mul		r0, r0, t0	// diffuse texture opacity

			mov		r0.rgb, c0	// output 
								// r0.rgb 	= fog color
								// r0.a	= fog density
			
		};
    }
}

