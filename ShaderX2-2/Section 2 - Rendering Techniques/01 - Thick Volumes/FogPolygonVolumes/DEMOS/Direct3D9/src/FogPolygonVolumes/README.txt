

FogPolygonVolumes

This demo renders ordinary polygon objects as thick volumes of material.  The objects are rendered using a few passes of rendering to offscreen texture render targets.  The rendering is based on the depths of each face of the object, and the result is a calculation of the thickness through the objects at each pixel.  At each pixel, The thickness is multiplied by a scale factor and used as a texture coordinate into a color ramp.  The color ramp maps object thickness to the color used in blending the volume objects ("fog") to the scene, so it is easy to change the look and behavior of the rendering.  The color ramp can be created in a paint program or you could extend the code to compute the color ramp from a mathematical description of light scattering or attenuation.

The technique is an extension of Dan Baker's VolumeFog demo in the Microsoft DirectX(tm) SDK.  

There are many comments in the demo code, and more information about the technique will be presented in a future article.


Some highlights of this particular demo are:
- Code and shaders for ps.1.3 hardware and ps.2.0 hardware
- Dithering of depth information to break up depth aliasing artifacts.
- Variable depth precision of 9, 12, 15, 18, or 21 bits using RGBA8 (8 bits per color component) render targets and an elegant method to encode and decode the high precision values.
- The high precision allows a deep frustum of volume objects to be processed at once.
- Ability to acumulate total thickness information using few rendering passes.  Additive blending is used instead of depth peeling or many passes of rendering.
- Elegant handling of solid object intersection and camera position.  The camera can penetrate the fog volumes and solid objects with no special code or artifacts.
- It is trivial to animate the volume objects.
- Shadow volume objects are used for 'positive' and 'negative' thickness contributions to get the effect of shafts of light and shadow.  These are the same objects you would use for stencil shadow rendering.


Some limitations are:
- Density of the fog is constant.  Color depends only on object thickness with no handling of light attenuation as a function of distance from a source.  Attenuation is probably possible but hasn't been explored here.
- "negative" thickness objects, or shafts of shadow, may overlap or extend outside of 'positive' volumes, making those areas too dark.  A solution to this is Cass Everitt's method of depth peeling (see http:\\developer.nvidia.com), but this would require many more passes over the scene.  The effects of the over-subtraction or over-darkening are not severe in this demo or in scenes with a similar layout.  You can spot them if you know what to look for, but they do not cause any harsh artifacts.
- One color ramp applies to all volumes rendered in one pass through the entire multipass technique.  To render volumes using different color ramps, you must do separate passes for each color ramp.  The code does not show this, but you can re-use the result of the first pass (which renders the depths of solid objects) if you need to render with several color ramps.   


Hit "F1" or "H" in the demo to bring up a menu of keyboard controls.

