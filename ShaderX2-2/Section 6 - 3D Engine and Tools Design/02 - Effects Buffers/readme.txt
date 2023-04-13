Framebuffer effects demo.



This uses HLSL to compile the final layer-combining pixel shader. This allows flexibility, but doesn't produce the best code ever. Obviously in a real game each case that is used would be hand-coded, rather than trying to use a generic solution like this.

The following are known problems:

-PS1.1,PS1.3: compiles very few shaders. The distortion effect could be done using a texbem instruction. This is how we did it in Blade 2, and it works well. However texbem is such a specialised instruction that HLSL can't really use it - again, you would hand-code this. The greyscale effect works fine. For some reason the heat-vision effect doesn't work, which is odd because it is just a bunch of lerps. I suspect HLSL is producing some slightly sub-optimal code.

-PS1.4: compiles most effects. Some of the combinations go beyond the shader length limits. And there's a strange bug where the wrong buffer gets fed to the wrong effect - I'm unsure if this is my bug or HLSL's. The same shader in PS2.0 works fine.

-PS2.0: everything just works.

-PS3.0: not supported by HLSL just yet, but everything should work.



The actual object-drawing code is horrendous, especially the particle system draw. Don't copy it! The main point of this demo is the idea of having multiple effect buffers arranged in the fewest number of actual rendertargets as possible, and managing the mapping from rendertarget to final pixel-shader combiner.

This compiles and runs under VS.net. I haven't tried it under VS6. Should be easy to get it to work though - just throw all the files at it. This project is based around code produced by the DX9AppWizard.


Updated versions of this code will be available at www.tomforsyth.pwp.blueyonder.co.uk.


Tom Forsyth 27/4/03







