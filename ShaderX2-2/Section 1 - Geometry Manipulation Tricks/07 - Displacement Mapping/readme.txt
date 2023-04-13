
To run the demo:

-You need DX9. If you don't have vertex shader 1.1 in hardware, the program will use the software VS pipeline, but it's a lot less impressive.

-Run the demo.

-Go to the "File" menu, select "Open File".

-Find the file memau.ssm and load it.

-See cute kitty on screen!

-Go to the "Demo Sequence" menu, and in this order, click on:

-"Tessellate a lot" - tessellates the kitty up to about 12k polys. Takes a while, but you get a progress bar.

-"Start Animation" - you won't actually see any animation until you click on the next one:

-"Toggle Animation Rendering" - yay! The kitty will now cycle through a bunch of animations. Note we are still using a very slow drawing method, so speed is less than impressive.

-"Toggle Displacement Compression". This can take a while, and I forgot to put a progress bar in - sorry. It will come back eventually. As a rough guide, it's about 30 seconds on my Athlon 850. So now we're using disp.comp, but still in a very slow drawing method.

-"Toggle optimised draw". This switches to an optimised draw, but also turns on 100 of the little critters, all independently animating.

So now check out the Mtri/sec rating. Compare and contrast with the score from the OptimiseMesh sample. On some machines (Athlon2000 with Radeon 9700) I've had 40Mtri/sec in this demo, compared with 70Mtri/sec for OptimiseMesh, which is pretty good. There are a variety of reasons for speed being lower - this demo seems to suffer from slow AGP writes, which OptimisedMesh doesn't.

Anyway, it's an interesting test. Note that toggling animation on and off doesn't change the framerate by much because it's only animating the ~260 vertices of the base mesh.


You can zoom in and out using cursor up and down keys. Both mouse buttons + mouse orbits the current object, though the buttons seem to "stick" on some systems - it's a bit fiddly. Never did find the problem.


Clicking on any other stuff is done at your own risk!



Tom Forsyth
www.tomforsyth.pwp.blueyonder.co.uk
tomf@muckyfoot.com
tom.forsyth@blueyonder.co.uk

13th March 2003



Here is the VS code used by the demo. It could probably be optimised a bit. The pixel pipeline is just a standard dot3 renderer.

vs_1_1


; C0-3: projection matrix
; C4.xy = displacement scale & offset.
; C4.zw = index scale & offset (= 7 and 3
;  - base mesh starts at C7 and each vertex is 3 items long
;  - could be precalculated on hardware with <256 constants)
; C5.xy = IJ scale & offset
; C5.zw = UV offset scale & offset
; C6 = useful numbers 0.0, 1.0, and two other.
; C7 onwards - base mesh data:
;		float fPosX, fPosY, fPosZ, fU;
;		float fLightX, fLightY, fLightZ, fV;
;		float fDispNormX, fDispNormY, fDispNormZ, padding;


; V0 = index1,index2,index3,disp
; V1 = I,J,U,V
dcl_texcoord0		v0.xyzw
dcl_texcoord1		v1.xyzw

; r0.xyz = indices.
; r0.w = displacement,
mad		r0, v0, c4.zzzx, c4.wwwy
; Calc I,J
mad		r1.xy, v1.xy, c5.x, c5.y
; Calc K=1-I-J
sub		r1.z, c6.y, r1.x
sub		r1.z, r1.z, r1.y

; Load vertex 1.
mov		a0.x, r0.x
; Position and U
mul		r2,     r1.z, c[a0.x  ]
; Light and V
mul		r3,     r1.z, c[a0.x+1]
; Disp.norm
mul		r4.xyz, r1.z, c[a0.x+2].xyz

; Load vertex 2.
mov		a0.x, r0.y
mad		r2,     r1.x, c[a0.x  ]    , r2
mad		r3,     r1.x, c[a0.x+1]    , r3
mad		r4.xyz, r1.x, c[a0.x+2].xyz, r4.xyz

; Load vertex 3.
mov		a0.x, r0.z
mad		r2,     r1.y, c[a0.x  ]    , r2
mad		r3,     r1.y, c[a0.x+1]    , r3
mad		r4.xyz, r1.y, c[a0.x+2].xyz, r4.xyz

; Displace.
mad		r4.xyz, r4.xyz, r0.w, r2.xyz
mov		r4.w, c6.y
; And transform
dp4		oPos.x, r4, c0
dp4		oPos.y, r4, c1
dp4		oPos.z, r4, c2
dp4		oPos.w, r4, c3

; UV scale, offset and offset.
mad		r5.xy, v1.zw, c5.z, c5.w
add		r5.x, r5.x, r2.w
add		r5.y, r5.y, r3.w
mov		r5.zw, c6.xxxy
mov		oT0, r5
mov		oT1, r5
; Light.
mov		oD0.xyz, r3.xyz
mov		oD0.w, c6.x





