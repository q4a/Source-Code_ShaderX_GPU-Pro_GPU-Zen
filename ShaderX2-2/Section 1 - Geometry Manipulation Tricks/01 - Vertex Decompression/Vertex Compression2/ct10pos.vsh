vs_1_1
dcl_position v0
dcl_normal v3

; position is compressed via the compressed transform 10 bit method
; Input:
;
; v0.xyz - position in the range -1.f - 1.f
; v3.xyz - normal
; c0-c3 - world view projection matrix
; c4-c7 - compression matrix
; c8 - <0.5f, 1, ?, ?>
mad r0, v0, c8.x, c8.x				; convert into 0.f to 1.f
m4x4 r1, r0, c4						; decompress position
m4x4 r0, r1, c0					; transform position

mov oPos.xyzw, r0
mov oD0.rgb, v3.xyz					; use normal data as colour
