vs.1.1
;------------------------------------------------------------------------------
; Constants specified by the app
;    c0      = ( 0, 0.5, 1, 0 )
;    c1      = ( 1, 0.5, 0.2, 0.05 )
;    c3      = jitter
;    c4-c7   = world-view-projection matrix
;    c8-c11  = world-view matrix
;    c12-c15 = view matrix
;    c22     = light0 pos
;    c23     = light0 col
;    c24     = light1 pos
;    c25     = light1 col
;    c28     = projection matrix
;
; Vertex components (as specified in the vertex DECL)
;    v0    = Position
;    v3    = Blend weight
;    v4    = Normal
;------------------------------------------------------------------------------

def c18, 2, 0.000001, 1, 1

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

; Projection:
mov r6, r10
mov r6.z, r6.w

; r6.x = (x/z) * 0.5 + 0.5
;      = (x * 0.5 + 0.5 * z) / z
mul r6.x, r6.x, c0.y
mad r6.x, r6.z, c0.y, r6.x

; r6.y = 1 - (y/z) * 0.5 + 0.5
;      = (1 * z - y * 0.5 + 0.5 * z) / z
mul r6.y, r6.y, c0.y
mad r6.y, r6.z, c0.y, -r6.y

; now, find to-light0 vector
sub r4, c22, v0
dp3 r4.w, r4, r4
rsq r4.w, r4.w
mul r4.xyz, r4.xyz, r4.w
dp3 r4, r4, v1
mul r4, r4, c23

; same for light1
sub r5, c24, v0
dp3 r5.w, r5, r5
rsq r5.w, r5.w
mov r7.w, r5.w
mul r5.xyz, r5.xyz, r5.w
dp3 r5, r5, v1
mul r5, r5, c25

; attenuation for light1 is in r7.w
sub r7, c24, v0
dp3 r7.w, r7, r7
mul r7.w, r7.w, c18.y
rcp r7.w, r7.w
min r7.w, r7.w, c18.z
mul r5.xyz, r5.xyz, r7.w

; Store output position
mov oPos, r10
mov oT0, v2
mov oT1, r6
mov oT2, r6
mov oD0, r4 
mov oD1, r5

