vs.1.1

;------------------------------------------------------------------------------
; Constants specified by the app
;    c0      = ( 0, 0, 0, 0 )
;    c1      = ( 1, 0.5, 0.2, 0.05 )
;    c3      = jitter
;    c4-c7   = world-view-projection matrix
;    c8-c11  = world-view matrix
;    c12-c15 = view matrix
;    c20     = light position
;    c26     = object center position
;    c28     = projection matrix
;
; Vertex components (as specified in the vertex DECL)
;    v0    = Position
;    v3    = Blend weight
;    v4    = Normal
;------------------------------------------------------------------------------

dcl_position0 v0
dcl_blendweight0 v3
dcl_normal0 v4

; incoming vertex position in r0
mov r0, v0

; jitter light position in r1
mul r1, v4, c3.x
sub r1, c20, r1

; r8 is the to-light vector
sub r8, r0, r1

; normalize r8
dp3 r8.w, r8, r8
rsq r8.w, r8.w
mul r8.xyz, r8.xyz, r8.w

; extrude it by the scale factor c1.x
mul r8, r8, c1.x

; apply weight
mul r8, r8, v3.x

; add back to original vertex position
add r8, r8, r0

; that one is needed to restore the "scaling" factor lost because of extrusion:
; TODO: can collapse that into previous instructions?
mov r8.w, v0.w

; Transform to view space
m4x4 oPos, r8, c4
