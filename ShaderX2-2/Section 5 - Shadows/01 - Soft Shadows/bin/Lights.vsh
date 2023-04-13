vs.1.1
;------------------------------------------------------------------------------
; Constants specified by the app
;    c0      = ( 0, 0.5, 1, 0 )
;    c1      = ( 20, 20, 20, 20 )
;    c2      = ( 0.001, 0.0001, 0, 0 )
;    c3      = jitter
;    c4-c7   = world-view-projection matrix
;    c8-c11  = world-view matrix
;    c12-c15 = view matrix
;    c20     = light position
;    c22     = (0, 1, 0, 0)
;    c26     = object center
;    c28     = projection matrix
;
; Vertex components (as specified in the vertex DECL)
;    v0    = Position
;    v3    = Blend weight
;    v4    = Normal
;------------------------------------------------------------------------------

; radius & stuff
def c18, 2, 0.000003, 1, 1

; light 0's position, color
def c24, -1007.473, -68.222, 44.616, 1

;def c25, 0.8, 0.8745, 0.9882, 1
def c25, 0.264, 0.291, 0.326, 1

def c16, 1676.803, -68.222, -33.509, 1
def c17, 249.874, -68.222, -29.322, 1

dcl_position0 v0
dcl_normal0 v1
dcl_texcoord0 v2

;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

; Transform to view space (world matrix is identity)
m4x4 r9, v0, c12

; Transform to projection space
m4x4 r10, r9, c28

; Store output position
mov oPos, r10

; light 0, store color in r5
sub r5, c24, v0
dp3 r5.w, r5, r5
rsq r5.w, r5.w
mov r7.w, r5.w
mul r5.xyz, r5.xyz, r5.w
dp3 r5, r5, v1
mul r5, r5, c25

; attenuation for light 0 is in r7.w
sub r7, c24, v0
dp3 r7.w, r7, r7
mul r7.w, r7.w, c18.y
rcp r7.w, r7.w
min r7.w, r7.w, c18.z
mul r5.xyz, r5.xyz, r7.w

; contribution of light 0 in r0
mov r0, r5

; light 1, store color in r5
sub r5, c16, v0
dp3 r5.w, r5, r5
rsq r5.w, r5.w
mov r7.w, r5.w
mul r5.xyz, r5.xyz, r5.w
dp3 r5, r5, v1
mul r5, r5, c25

; attenuation for light 0 is in r7.w
sub r7, c16, v0
dp3 r7.w, r7, r7
mul r7.w, r7.w, c18.y
rcp r7.w, r7.w
min r7.w, r7.w, c18.z
mul r5.xyz, r5.xyz, r7.w

; contribution of light 0 in r0
add r0, r0, r5

; light 2, store color in r5
sub r5, c17, v0
dp3 r5.w, r5, r5
rsq r5.w, r5.w
mov r7.w, r5.w
mul r5.xyz, r5.xyz, r5.w
dp3 r5, r5, v1
mul r5, r5, c25

; attenuation for light 0 is in r7.w
sub r7, c17, v0
dp3 r7.w, r7, r7
mul r7.w, r7.w, c18.y
rcp r7.w, r7.w
min r7.w, r7.w, c18.z
mul r5.xyz, r5.xyz, r7.w

; contribution of light 0 in r0
add r0, r0, r5

mov oT0, v2
mov oD0, r0
