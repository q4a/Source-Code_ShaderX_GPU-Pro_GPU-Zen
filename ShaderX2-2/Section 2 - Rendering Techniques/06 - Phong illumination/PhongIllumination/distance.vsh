vs.2.0

dcl_position  v0

// c0-c3 = mvp matrix
// c4    = camera position
// c5    = light position

// Transform position
m4x4	oPos, v0, c0

sub		oT0, c5, v0       // oT0 = light vector
