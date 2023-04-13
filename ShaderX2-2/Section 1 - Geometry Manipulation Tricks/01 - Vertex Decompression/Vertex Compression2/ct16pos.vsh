vs_1_1
dcl_position v0
dcl_normal v3

; position is compressed via the compressed transform 16 bit method
; Input:
;
; v0.xyz - position in the range -32767.f - 32767.f
; v3.xyz - normal
; c0-c3 - world view projection matrix
; c4-c7 - compression matrix
; c8 - <0.5f, 1, 1.f/65535, 32767>
mad r0, v0, c8.z, c8.x				; convert into 0.f to 1.f
m4x4 r1, r0, c4						; decompress position
m4x4 r0, r1, c0					; transform position

mov oPos.xyzw, r0
mov oD0.rgb, v3.xyz					; use normal data as colour
