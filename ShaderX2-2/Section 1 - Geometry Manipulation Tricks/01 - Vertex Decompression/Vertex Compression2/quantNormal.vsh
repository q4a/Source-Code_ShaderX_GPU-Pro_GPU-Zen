vs_1_1
dcl_position v0
dcl_normal v3

; Input:
;
; v0.xyz - position
; v3.xyz - normal mapped -1.f -> 0.f, 1.f -> 1.f
; c0-c3 - world view projection matrix
m4x4 r0, v0, c0					; transform position

mov oPos.xyzw, r0
mov oD0.rgb, v3.xyz					; use normal data as colour
