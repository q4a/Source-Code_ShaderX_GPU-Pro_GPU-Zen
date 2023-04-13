vs_1_1
dcl_position0	v0
dcl_position1	v1
dcl_normal		v3

; position is compressed via the compressed transform 10,10,12 bit method
; Input:
;
; v0.xyz - position in the range -1.f - 1.f (excludes the top 2 bits of Z)
; v1.w - the top 2 bits of Z
; v3.xyz - normal
; c0-c3 - world view projection matrix
; c4-c7 - compression matrix
; c8 - <1.f/1024.f, 1, 1/64.f, 0.25f>

mul r1.y, v1.w, c8.z				; enc2bit / 64
expp r0.y, r1.y						; first part of floor
add r0.y, r1.y, - r0.y				; second part of floor

mul r1.xyz, v0, c8.x				; load main part and convert into 0.f to 1.f
add r1.z, r1.z, r0.y				; add enc2bit to z
mul r1.z, r1.z, c8.w				; and scale z back into 0 to 1
mov r1.w, c8.y

m4x4 r0, r1, c4						; decompress position
m4x4 r1, r0, c0						; transform position

mov oPos.xyzw, r1
mov oD0.rgb, v3.xyz					; use normal data as colour
