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

dcl_position0 v0
dcl_normal0 v1
dcl_texcoord0 v2

; incoming vertex position in r0
mov r0, v0

;------------------------------------------------------------------------------
; Test E: move the shadow volume vertices towards the light
;------------------------------------------------------------------------------
;mov r1, c20
;mov r2, c26
;sub r2, c20, r2
;dp3 r2.w, r2, r2
;rsq r2.w, r2.w
;mul r2.xyz, r2.xyz, r2.w
;mad r0.xyz, r2.xyz, c3.x, r0.xyz

;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

; Transform to view space (world matrix is identity)
m4x4 r9, r0, c12

; Transform to projection space
m4x4 r10, r9, c28

; Store output position
mov oPos, r10
