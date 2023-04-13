// c0    : Light position in object space
// c1    : 1, 1, 1, 0
// c2-c5 : Light * View * Proj = LightClip?
// c6-c9 : WorldInvLight matrix
// c10   : Color for exposing the shadow volume

vs.1.1
// Output diffuse color to expose shadow volume to viewer if needed
  mov oD0, c10

// Light to vertex ray in object space
  sub r1, v0, c0

// Transform vertex from object space to worldlight space
// where the light is centered on origin
  mul r4, v0.x, c[6]
  mad r4, v0.y, c[7], r4
  mad r4, v0.z, c[8], r4
  add r9, c[9], r4
  
// Normalize ray computed previously
  dp3 r1.w, r1, r1
  rsq r1.w, r1.w
  mul r1, r1, r1.w
  
  mov r10, c1
  
// Dot ray and normal
  dp3 r10.w, v1, r1
    
// If dot result < 0.0 = face away from light
// Form mask 1,1,1,1 for light facing, OR mask 1,1,1,0 for non light facing
  slt r10, c1.w, r10

// Set w value to 0.0 for infinite extrusion or 1.0 for no extrusion
  mul r9, r9, r10
  
// Transform final vertex to LightClip space
  mul r4, r9.x, c[2] 
  mad r4, r9.y, c[3], r4 
  mad r4, r9.z, c[4], r4
  mad oPos, r9.w, c[5], r4