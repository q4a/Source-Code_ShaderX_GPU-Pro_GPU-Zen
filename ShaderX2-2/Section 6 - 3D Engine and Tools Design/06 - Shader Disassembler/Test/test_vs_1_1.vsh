vs_1_1
def        c20     , 255.002, 0, 0, 0
dcl_position0       v0      
dcl_blendindices0   v1      
dcl_color0          v2      
dcl_texcoord0       v3      
mul        r0.w    ,   v1.z        ,   c20.x       
mov        a0.x    ,   r0.w        
add        r7      ,   v0          ,   c0[a0.x]    
m4x4       oPos    ,   r7          ,   c16         
mov        oD0     ,   v2          
mov        oT0.xy  ,   v3          
