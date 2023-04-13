// c0    : 0, 0, 0, m_fExtrusionLen
// c1    : Light pos in object space
// c2-c5 : World*View*Proj matrix
// c6    : Color for exposing the shadow volume

vs.1.1
// Output diffuse color to expose shadow volume to viewer if needed
  mov oD0, c6

// Ray from light to pt in object space
  sub r1, v0, c1
  
// Normalize ray
  dp3 r1.w, r1, r1
  rsq r1.w, r1.w
  mul r1, r1, r1.w
  
// Dot ray and normal
  dp3 r10.w, v1, r1
  
// Normal faces away from light if dot result < 0.0
  slt r10.x, r10.w, c0.x
  
// Extrude along ray
  mul r10, r10.x, c0.w
  mad r0, r1, r10.x, v0
  
// Transform to clip space and output pt
  mul r4, r0.x, c[2] 
  mad r4, r0.y, c[3], r4 
  mad r4, r0.z, c[4], r4
  add oPos, c[5], r4