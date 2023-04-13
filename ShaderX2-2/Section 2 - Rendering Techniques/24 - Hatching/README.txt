Name: Stroke Rendering Effects

Authors: Kevin Buchin & Maike Walther

Description: stroke_rendering.zip contains demos illustrating the effects
             described in "Hatching, Stroke Styles & Pointillism". The
             demos require ATI RenderMonkey. Extract stroke_rendering.zip
             to the folder "ATI Technologies\Rendermonkey\v1.0". The 
             effect workspace will be extracted to
             "ATI Technologies\Rendermonkey\v1.0\Examples\Dx9\Hlsl\HLSL Stroke Rendering.rfx".

Variables: ("!" marks important variables):

 + Pointillistic Butterfly Effect Group
  - Pointillistic Butterfly HLSL 
    scale            : size of the marks                        !
    body_color       : color of the butterfly

 + Threshold Hatching Effect Group
    lightDir         : light direction
    short_stroke     : single stroke texture for short strokes  !
    long_stroke      : single stroke texture for long strokes   !
    model            : model used ( the torus is also nice )

  - specular highlight lookup
    ambient          : background color 
    diffuse          : stroke color for diffuse lighting
    specular         : stroke color for specular highlights
    texture_scale    : scale of the stroke textures

  - lookup hatching
    texture_scale    : scale of the stroke textures
 
  - threshold hatching

 + Painterly Tree Effect Group
    size             : size of the billboard
    lightDir         : light direction
    world_up         : world_up vector, used as billboarding axis
    billboard_center : position of the billboard

  - shaded tree
  - painterly tree
  - billboarding examples

Have fun!
  