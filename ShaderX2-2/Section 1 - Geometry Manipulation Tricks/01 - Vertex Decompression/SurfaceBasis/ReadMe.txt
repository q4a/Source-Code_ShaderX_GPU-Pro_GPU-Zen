Surface Basis Sample
--------------------

6/7/03
------

This is a sample for surface basis function used in displacement compression 
The full technique is described in "Vertex Decompression using Vertex Shaders Part 2".

It shows the techniques for surface evaluation in a vertex shader. 
This allows any vertex shader hardware to evaluate surface patches and displacment maps.

Buttons:
Arrows - spin object

F5 - Control - just normal D3DX Mesh rendering
F6 - Control NPatch - D3DX Software NPatch tesselation
F7 - Barycentric vertices - Vertex stream contains barycentric data. Position data in constant registers
F8 - NPatch basis - Per-vertex NPatch evaluation in a vertex shader via barycentric coordinate in vertex stream
F9 - Linear basis - Linear patch evaluated in vertex shader with a constant Displacement map

1 - 6 For methods with variable tesselation levels, changes the lod level.

IMPLEMENTATION NOTES:
Quantising the barycentric coordinates causing cracks to appear. I suspect this is due to the int->float conversion.
My tesselator is different from D3DX so the same lod level producing different number of triangles. 
The tesselation is purely CPU and can be pre-cached as most of the data is constant across all patches.

LEGAL:
	This sample (all source code etc) comes with no warrenty implied or otherwise. You can
	use and abuse as you wish. It is public domain, completely free in all senses of the word.

Bye,
	Deano

Dean Calver

EMail: deano@rattie.demon.co.uk
