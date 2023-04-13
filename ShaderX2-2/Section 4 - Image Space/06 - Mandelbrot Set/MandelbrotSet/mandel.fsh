ps.2.0

dcl		t0
dcl_2d	s0

def		c0, 0.0, 1.0, 8.0, 0.0

mad		r2.xy, t0.x, t0, t0
mad		r1.x, -t0.y, t0.y, r2.x
mad		r1.y,  t0.x, t0.y, r2.y

mad		r2.xy, r1.x, r1, t0
mad		r0.x, -r1.y, r1.y, r2.x
mad		r0.y,  r1.x, r1.y, r2.y

mad		r2.xy, r0.x, r0, t0
mad		r1.x, -r0.y, r0.y, r2.x
mad		r1.y,  r0.x, r0.y, r2.y

mad		r2.xy, r1.x, r1, t0
mad		r0.x, -r1.y, r1.y, r2.x
mad		r0.y,  r1.x, r1.y, r2.y

mad		r2.xy, r0.x, r0, t0
mad		r1.x, -r0.y, r0.y, r2.x
mad		r1.y,  r0.x, r0.y, r2.y

mad		r2.xy, r1.x, r1, t0
mad		r0.x, -r1.y, r1.y, r2.x
mad		r0.y,  r1.x, r1.y, r2.y

mad		r2.xy, r0.x, r0, t0
mad		r1.x, -r0.y, r0.y, r2.x
mad		r1.y,  r0.x, r0.y, r2.y

mad		r2.xy, r1.x, r1, t0
mad		r0.x, -r1.y, r1.y, r2.x
mad		r0.y,  r1.x, r1.y, r2.y

mad		r2.xy, r0.x, r0, t0
mad		r1.x, -r0.y, r0.y, r2.x
mad		r1.y,  r0.x, r0.y, r2.y

mad		r2.xy, r1.x, r1, t0
mad		r0.x, -r1.y, r1.y, r2.x
mad		r0.y,  r1.x, r1.y, r2.y

mad		r2.xy, r0.x, r0, t0
mad		r1.x, -r0.y, r0.y, r2.x
mad		r1.y,  r0.x, r0.y, r2.y

mad		r2.xy, r1.x, r1, t0
mad		r0.x, -r1.y, r1.y, r2.x
mad		r0.y,  r1.x, r1.y, r2.y

mad		r2.xy, r0.x, r0, t0
mad		r1.x, -r0.y, r0.y, r2.x
mad		r1.y,  r0.x, r0.y, r2.y

mad		r2.xy, r1.x, r1, t0
mad		r0.x, -r1.y, r1.y, r2.x
mad		r0.y,  r1.x, r1.y, r2.y

mad		r2.xy, r0.x, r0, t0
mad		r1.x, -r0.y, r0.y, r2.x
mad		r1.y,  r0.x, r0.y, r2.y

mad		r2.xy, r1.x, r1, t0
mad		r0.x, -r1.y, r1.y, r2.x
mad		r0.y,  r1.x, r1.y, r2.y

mad		r2.xy, r0.x, r0, t0
mad		r1.x, -r0.y, r0.y, r2.x
mad		r1.y,  r0.x, r0.y, r2.y

mad		r2.xy, r1.x, r1, t0
mad		r0.x, -r1.y, r1.y, r2.x
mad		r0.y,  r1.x, r1.y, r2.y

mad		r2.xy, r0.x, r0, t0
mad		r1.x, -r0.y, r0.y, r2.x
mad		r1.y,  r0.x, r0.y, r2.y
mov		r1.z, c0.x

dp3_sat	r0, r1, r1
mul		r0.x, r0.x, c0.z

exp		r0.x, -r0.x
sub		r0, c0.y, r0

texld	r0, r0, s0

mov		oC0, r0
