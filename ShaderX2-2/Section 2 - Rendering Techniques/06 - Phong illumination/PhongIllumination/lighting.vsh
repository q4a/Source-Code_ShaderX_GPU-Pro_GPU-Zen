vs.2.0

dcl_position  v0
dcl_texcoord0 v1   // TexCoord
dcl_texcoord1 v2   // uVec
dcl_texcoord2 v3   // vVec
dcl_texcoord3 v4   // normal

// c0-c3 = mvp matrix
// c4    = camera position
// c5    = light position

def c8,   1.0,  2.0, -1.0, 0.0
def c9,   2.0, -1.0,  1.0, 0.0
def c10, -1.0,  1.0,  2.0, 0.0

// Transform position
m4x4	oPos, v0, c0

// Output texcoord
mov		oT0, v1

sub		r0, c5, v0         // r0 = light vector
dp3		oT1.x, r0, v2
dp3		oT1.y, r0, v3
dp3		oT1.z, r0, v4      // oT1 = light vector in tangent space

sub		r1, c4, v0         // r1 = view vector
dp3		oT2.x, r1, v2
dp3		oT2.y, r1, v3
dp3		oT2.z, r1, v4      // oT2 = view vector in tangent space

mov		oT3, -r0           // oT3-oT6 = shadow map sample locations
sub		oT4, c8,  r0
sub		oT5, c9,  r0
sub		oT6, c10, r0
