/////////////////////////////////////////////////////////////////////////////////////////
// Copyright (C) Jakub Klarowicz, 2003.
// ShaderX 2
// All rights reserved
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute.
//
// Author: Jakub Klarowicz
// Email: kuba@techland.pl
//
/////////////////////////////////////////////////////////////////////////////////////////

texture base_tex;
texture bump_rgb;
texture bump_dxtc;
texture bump_dxtc_gina;

float4x4 combined;

float4 eye_pos_model;
float4 light_pos_model;

sampler s_state = sampler_state
{
    addressu  = clamp;
    addressv  = clamp;
    mipfilter = none;
    minfilter = linear;
    magfilter = linear;
};

// one vertex shader to rule them all
vertexshader v_shader = asm
{
    vs_1_1

    dcl_position    v0
    dcl_texcoord    v6

    // c0-c4 model*view*projection
    // c4    light position in model space

    def     c10, 1,0, 0,0    // tangent
    def     c11, 0,0,-1,0    // normal
    def     c12, 0,1, 0,0    // binormal

    def     c13, 0.5,0,0,0


    m4x4    oPos, v0, c0

    mov     oT0, v6
    mov     oT1, v6

    // to_the_light vector
    sub     r1, c4, v0
    dp3     r1.w, r1, r1
    rsq     r1.w, r1.w
    mul     r1, r1, r1.w

    // to_the_eye vector
    sub     r2, c5, v0
    dp3     r2.w, r2, r2
    rsq     r2.w, r2.w
    mul     r2, r2, r2.w

    // half vector
    add     r3, r1, r2
    dp3     r3.w, r3, r3
    rsq     r3.w, r3.w
    mul     r3, r3, r3.w

    dp3     r0.x, r1, c10
    dp3     r0.y, r1, c12
    dp3     r0.z, r1, c11
    mad     oD0, r0.xyz, c13.x, c13.x

    dp3     r0.x, r3, c10
    dp3     r0.y, r3, c12
    dp3     r0.z, r3, c11
    mad     oD1, r0.xyz, c13.x, c13.x
};


technique ShowBumpRGB
{
    pass p0
    {
        cullmode = none;

        texture[0] = (bump_rgb);
        sampler[0] = (s_state);

        vertexshaderconstantf[0] = (combined);
        vertexshader = (v_shader);

        pixelshader = asm
        {
            ps_1_1
            tex t0
            mov r0, t0
        };
    }
}

technique ShowBumpDXTC
{
    pass p0
    {
        cullmode = none;

        texture[0] = (bump_dxtc);
        sampler[0] = (s_state);

        vertexshaderconstantf[0] = (combined);
        vertexshader = (v_shader);

        pixelshader = asm
        {
            ps_1_1
            tex t0
            mov r0, t0
        };
    }
}

technique ShowBumpGINA
{
    pass p0
    {
        cullmode = none;

        texture[0] = (bump_dxtc_gina);
        sampler[0] = (s_state);

        vertexshaderconstantf[0] = (combined);
        vertexshader = (v_shader);

        pixelshader = asm
        {
            ps_1_1

            def c0, 0,1,0,0

            tex t0

            mad r0, t0.a, c0, t0
        };
    }
}

technique ShowDiffDXTC
{
    pass p0
    {
        cullmode = none;

        texture[0] = (bump_dxtc);
        texture[1] = (bump_rgb);
        sampler[0] = (s_state);
        sampler[1] = (s_state);

        vertexshaderconstantf[0] = (combined);
        vertexshader = (v_shader);

        pixelshader = asm
        {
            ps_1_1

            tex t0
            tex t1

            sub_x4_sat  r0, t0, t1
            sub_x4_sat  r1, t1, t0
            add_x4      r0, r0, r1
        };
    }
}

technique ShowDiffGINA
{
    pass p0
    {
        cullmode = none;

        texture[0] = (bump_dxtc_gina);
        texture[1] = (bump_rgb);
        sampler[0] = (s_state);
        sampler[1] = (s_state);

        vertexshaderconstantf[0] = (combined);
        vertexshader = (v_shader);

        pixelshader = asm
        {
            ps_1_1

            def c0, 0,1,0,0

            tex t0
            tex t1

            mad         t0, t0.a, c0, t0
            sub_x4_sat  r0, t0, t1
            sub_x4_sat  r1, t1, t0
            add_x4      r0, r0, r1
        };
    }
}

technique ShowLightRGB
{
    pass p0
    {
        cullmode = none;

        texture[0] = (bump_rgb);
        texture[1] = (base_tex);
        sampler[0] = (s_state);
        sampler[1] = (s_state);

        vertexshaderconstantf[0] = (combined);
        vertexshaderconstantf[4] = (light_pos_model);
        vertexshaderconstantf[5] = (eye_pos_model);
        vertexshader = (v_shader);

        pixelshader = asm
        {
            ps_1_1

            def     c0, 0,0,0,0.5

            tex     t0
            tex     t1

            dp3_sat r0, v0_bx2, t0_bx2  // diffuse
            mul     r0, r0, t1          // diffuse*texture

            dp3_sat r1, v1_bx2, t0_bx2  // specular
            mul     r1, r1, r1
            mul     r1, r1, r1
            mad     r0, r1, c0.a, r0    // diffuse*texture + 0.5*specular
        };
    }
}

technique ShowLightDXTC
{
    pass p0
    {
        cullmode = none;

        texture[0] = (bump_dxtc);
        texture[1] = (base_tex);
        sampler[0] = (s_state);
        sampler[1] = (s_state);

        vertexshaderconstantf[0] = (combined);
        vertexshaderconstantf[4] = (light_pos_model);
        vertexshaderconstantf[5] = (eye_pos_model);
        vertexshader = (v_shader);

        pixelshader = asm
        {
            ps_1_1

            def     c0, 0,0,0,0.5

            tex     t0
            tex     t1

            dp3_sat r0, v0_bx2, t0_bx2  // diffuse
            mul     r0, r0, t1          // diffuse*texture

            dp3_sat r1, v1_bx2, t0_bx2  // specular
            mul     r1, r1, r1
            mul     r1, r1, r1
            mad     r0, r1, c0.a, r0
        };
    }
}

technique ShowLightGINA
{
    pass p0
    {
        cullmode = none;

        texture[0] = (bump_dxtc_gina);
        texture[1] = (base_tex);
        sampler[0] = (s_state);
        sampler[1] = (s_state);

        vertexshaderconstantf[0] = (combined);
        vertexshaderconstantf[4] = (light_pos_model);
        vertexshaderconstantf[5] = (eye_pos_model);
        vertexshader = (v_shader);

        pixelshader = asm
        {
            ps_1_1

            def     c0, 0,1,0,0.5

            tex     t0
            tex     t1

            mad     t0, t0.a, c0, t0
            dp3_sat r0, v0_bx2, t0_bx2  // diffuse
            dp3_sat r1, v1_bx2, t0_bx2  // specular

            mul     r0.rgb, r0, t1      // diffuse*texture
            +mul    r1.a, r1.b, r1.b

            mul     r1, r1.a, r1.a
            mad     r0, r1, c0.a, r0    // diffuse*texture + 0.5*specular
        };
    }
}
