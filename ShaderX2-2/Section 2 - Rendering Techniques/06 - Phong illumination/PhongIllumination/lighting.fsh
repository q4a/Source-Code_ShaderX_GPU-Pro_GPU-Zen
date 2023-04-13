ps.2.0

dcl t0.xy
dcl t1
dcl t2
dcl t3
dcl t4
dcl t5
dcl t6

dcl_2d s0
dcl_2d s1
dcl_2d s2
dcl_cube s3

def c0, 2.0, 1.0, 24.0, 0.0   // (2.0, 1.0, specular exponent, 0.0)
def c1, 0.95, 0.25, 0.0, 0.0  // (depthbias, averaging factors)
// c2 = (constant attenuation, quadratic attenuation, ambient, 0)


texld	r0, t0, s0                // r0 = base
texld	r1, t0, s1                // r1 = bump
texld	r2, t0, s2                // r2 = gloss
texld	r8, t3, s3                // r8 = shadow map

texld	r9,  t4, s3                // r9  = shadow map
texld	r10, t5, s3                // r10 = shadow map
texld	r11, t6, s3                // r11 = shadow map

mad		r1.xyz, r1, c0.x, -c0.y   // bump[0..1] => bump[-1..1]

dp3		r7.w, r1, r1
rsq		r7.w, r7.w
mul		r1.xyz, r1, r7.w          // r1 = post-filter normalized bumpmap

dp3		r7, t1, t1
rsq		r7.w, r7.w
mul		r3, t1, r7.w              // r3 = normalized light vector
mul		r8.y, r7.x, c1.x          // r8.y = lengthSqr(light vector) * biasfactor

mad		r5, c2.y, r7, c2.x
rcp		r5, r5.x                  // r5 = attenuation

dp3_sat	r4, r3, r1                // r4 = diffuse


dp3		r7, t2, t2
rsq		r7.w, r7.x
mul		r6, t2, r7.w              // r6 = normalized view vector

dp3		r7, r3, r1
mul		r7, r7, c0.x
mad		r3, r7, r1, -r3           // r3 = reflection vector

dp3_sat	r3, r3, r6
pow		r3, r3.x, c0.z            // r3 = specular

mul		r3, r3, r2                // r3 = specular * gloss



mad		r4, r4, r0, r3            // r4 = base * diffuse + specular * gloss
mul		r4, r4, r5                // r4 = (base * diffuse + specular * gloss) * attenuation

sub		r8.x,  r8.x,  r8.y
sub		r9.x,  r9.x,  r8.y
sub		r10.x, r10.x, r8.y
sub		r11.x, r11.x, r8.y

cmp		r8.x,  r8.x,  c1.y, c1.z  // shadow comparison
cmp		r9.x,  r9.x,  c1.y, c1.z
cmp		r10.x, r10.x, c1.y, c1.z
cmp		r11.x, r11.x, c1.y, c1.z

add		r8.x, r8.x, r9.x
add		r8.x, r8.x, r10.x
add		r8.x, r8.x, r11.x         // r8 = shadow
mul		r4, r4, r8.x              // r4 = (base * diffuse + specular * gloss) * attenuation * shadow

mad		r4, r0, c2.z, r4          // r4 = (base * diffuse + specular * gloss) * attenuation * shadow + ambient * base

mov		oC0, r4
