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
;    c28     = projection matrix
;
; Vertex components (as specified in the vertex DECL)
;    v0    = Position
;    v3    = Blend weight
;    v4    = Normal
;------------------------------------------------------------------------------

def c0, 1, 1, 1, 1
def c1, 0.5, 0.5, 0.5, 0.5

dcl_position0 v0
dcl_normal0 v1
dcl_texcoord0 v2

;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

mov r0, v0
;mad r0, v1, c3.x, r0
;mov r0.w, v0.w

; light 0, store color in r5
sub r5, c20, v0
dp3 r5.w, r5, r5
rsq r5.w, r5.w
mul r5.xyz, r5.xyz, r5.w
dp3 r5, r5, v1
;mad r5, r5, c1, c1

; Transform to view space (world matrix is identity)
m4x4 r9, r0, c12

; Transform to projection space
m4x4 r10, r9, c28

; Store output position
mov oPos, r10
mov oD0, r5
mov oT0, v2
