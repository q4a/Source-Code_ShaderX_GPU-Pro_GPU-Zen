vs_3_0
; Constants:
;
;  c0-c3  - View+Projection matrix
;
;  c4.x   - min x
;  c4.y	  - min z
;  c4.z   - max x
;  c4.w	  - max z


defi i0,248,0,0,0

dcl_position v0.xz
dcl_position o0
dcl_color0 o1


mul r11.xz,c9.y,v0.xz		; Range -1.0 to 1.0

;Get the Mandelbrot coordinate
add r10.xy, v0.xz, c8.x ; Range 0 -> 1
lrp r0.xy, r10.xy, c4.zw, c4.xy
mov r10.xy, r0.xy

slt r3.x,r11,r11
slt r0.z,r0,r0

rep i0
	; r1 -> x*x y*y
	mul r1,r0,r0
	dp3 r2,r0,r0
	;if size of vector > 4 break
	break_ge r2.x,c8.y
	; r2 --> next element
	; r2.x --> x*x - y*y + xStart
	add r2.x,r1.x,-r1.y
	; r2.y --> 2*x*y + yStart
	mul r2.y,r0.x,r0.y
	add r2.y,r2.y,r2.y
	add r0.xy,r2,r10.xy
	; r3 iteration counter
	add r3.x,r3,c8.z
endrep

; Decompress position
;mov r11.z, r11.y
mul r11.y, r3.x, c9.w
;sge r11.w,c9.x,c9.x
mov r11.w, c8.w

; Transform position
m4x4 o0, r11,c0

frc r3.x,r3.x
mul r3.x,r3,c9.x


mova a0.x,r3.x
mov o1,c[10+a0.x]
