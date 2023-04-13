vs_2_sw
dcl_position0       v0      
dcl_color0          v1      
dcl_normal0         v2      
defb       b0      , true
defb       b1      , false
defi       i0      , 9, 1, -2, 0
setp_ne    p0      ,   c0          ,   c0          
( p0  )add r0      ,   c0          ,   c0          
sincos     r1.xy   ,   r0.y        ,   c16         ,   c17         
if_le        c43[a0.z].y ,   v1.y        
if           p0.z        
loop         aL          ,   i0          
abs        r1      ,   c2[aL]      
break_ge     c1.y        ,   c1.w        
break      
endloop    
endif      
call         l0          
rep          i4          
endrep     
endif      
ret        
label        l0          
if           b1          
endif      
ret        
