Displacment Compression Sample
--------------------

6/7/03
------

This is a sample for displacement compression 
The full technique is described in "Vertex Decompression using Vertex Shaders Part 2".

Using the linear basis it adds in a displacement map, the displacement map is a spikey fractal.
Spherical texture coordinate produce a spikey ball.

Buttons:
Arrows - spin object

1 - 6 For methods with variable tesselation levels, changes the lod level.

IMPLEMENTATION NOTES:
Quantising the barycentric coordinates causing cracks to appear. I suspect this is due to the int->float conversion.
In reality a high res model to displacement map tool would be used. ATI and NVIDIA have free tools for this.
Tom Forysth has wrote loads about the generation process and Charles Bloom has auto UV generation code.

LEGAL:
	This sample (all source code etc) comes with no warrenty implied or otherwise. You can
	use and abuse as you wish. It is public domain, completely free in all senses of the word.

Bye,
	Deano

Dean Calver

EMail: deano@rattie.demon.co.uk
