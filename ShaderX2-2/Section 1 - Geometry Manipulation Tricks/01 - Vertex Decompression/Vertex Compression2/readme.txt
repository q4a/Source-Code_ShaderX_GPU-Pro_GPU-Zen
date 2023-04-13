Vertex Compression Sample 2
---------------------------

6/7/03
------

This is a port of the sample I wrote to accompany the chapter "Vertex Decompression in a Shader" in ShaderX book to Dx9. 
The workspace/solution is now Visual Studio.NET. 
This has several new types as described in the "Vertex Decompression using Vertex Shaders Part 2" part 2.
These new 2 types cover the new packed 10 10 10 bit formats.

This has working examples of the various vertex compression methods from both articles. 

Buttons:
Arrows - spin object
Tab - show original uncompressed version

1 - Float position + quantised normal
2 - Scaled and offset 8 bit position + quantised normal
3 - Compression transform 8 bit position + quantised normal
4 - Scaled and offset 8 bit position + quantised normal
5 - Compression transform 16 bit position + quantised normal
6 - Sliding compression transform 24 bit position + quantised normal
7 - Compression transform using DEC3N with 10 bits per channel + quantised normal
8 - Compression transform 12 bits in the dominant axis, 10 bits in others + quantised normal	

F5 - Single Teapot
F6 - Single Cube
F7 - A row of 10 teapots (seperated alot to simulate a large object) (rest are off at an angle)
F8 - A row of 1500 cubes (seperated alot to simulate a HUGE object) (rest are off at an angle)

IMPLEMENTATION NOTES:
The compression transform methods push precision alot on huge objects, I spent a whole morning
fixing a bug that didn't exist because of this. Thats why the code for the sliding compression 
transforms doesn't match the pseudo code exactly (its the same method, just rearranged while I
was debugging). Getting sliding compression to work, was quite hard due to float only having
23 bit mantissa, so the actual generation code was losing a bit! I chickened out and reduced 
the method to 23 bit accuracy bit stored as a 24 bit integer (thats why its divided by 32767).

One important point that I didn't touch in the book chapter was the use of w, while in thoery
position.w is unused, you either have to set it to 1 in the vertex shader (possible costing a
cycle) or leave a compressed version in the input stream. In the examples all except sliding
compression transform method set the correct value in the stream data so that a 1 will appear.

I haven't optimised the vertex shader code at all, its designed to show the method rather than 
performance. In almost all cases you can concanate the compression matrix with the world 
matrix, this assume the w is set to 1 before the matrix multiply. Also almost all the scales 
can be either added to matrices or a different type can be used.

A few cycles are lost due to only 1 constant or 1 vertex register is allowed per instruction.
Hope this minor restriction gets lifted for a future version.

Method 7 and 8 used DEC3N which isn't supported on all cards.
Method 8 (10,10,12 bit) uses overlapping stream offsets which aren't supported on all cards

LEGAL:
	This sample (all source code etc) comes with no warrenty implied or otherwise. You can
	use and abuse as you wish. It is public domain, completely free in all senses of the word.

Bye,
	Deano

Dean Calver

EMail: deano@rattie.demon.co.uk
