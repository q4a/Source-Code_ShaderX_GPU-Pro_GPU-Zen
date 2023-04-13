vs.2.0
; Constants:
;
;  c0-c3  - View+Projection matrix
;
;  c4.x   - min x
;  c4.y	  - min z
;  c4.z   - max x
;  c4.w	  - max z


dcl_position v0.xyzw

mul r11, c9.y, v0 ; Range -1.0 to 1.0


;Get the Mandelbrot coordinate
add r10, v0, c8.x ; Range 0 -> 1
lrp r0.x, r10.x, c4.z, c4.x
lrp r0.y, r10.y, c4.w, c4.y
mov r10.xy, r0.xy

slt r3,r11,r11 ; All zeroes
sge r7,r11,r11 ; All ones

slt r0.z,r0,r0 ; Zero
loop aL, i0
	; r1 -> x*x y*y
	mul r1,r0,r0
	dp3 r2,r0,r0
	;if size of vector > 4 break
	slt r8,r2.x,c8.y
	mul r8,r8,r7
	; r2 --> next element
	; r2.x --> x*x - y*y + xStart
	add r2.x,r1.x,-r1.y
	; r2.y --> 2*x*y + yStart
	mul r2.y,r0.x,r0.y
	add r2.y,r2.y,r2.y
	add r0.xy,r2,r10
	; r3 iteration counter
	mul r4,r8,c8.z
	add r3,r3,r4
endloop


; Decompress position
mov r9.xz, r11.xy
mul r9.y, r3.x, c9.w
mov r9.w, c8.w       ; 1


; Transform position
m4x4 oPos, r9,c0

frc r3,r3.x
mul r3,r3,c9.x
mova a0.x,r3
mov oD0,c[10+a0.x]
