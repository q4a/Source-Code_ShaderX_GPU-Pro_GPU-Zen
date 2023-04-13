Readme for Fog Examples
------------------------

Requirements:
-------------
Microsof DirectX9 SDK

Notes:
------
1. All examples are using the DX9 SDK application framework.
Please refer to the SDK documentation for further information on the framework.
2. The source code projects are build for MS VC++ 6.0
3. For ExampleFive you can press 'P' to switch between a moving and a static viewer. 
The static viewer may be more interessing, when checking out the fog animation.



File structure:
---------------
Folder media:
- Contains the terrain heightmap and decal texture, used by all examples

Folder base:
- Contains the needed DX9 SDK application framework files.
- Contains ExampleBase.h/.cpp a base class, that implements functionality common to all fog examples.
- All ExampeBase derived classes used in the examples are supposed to implement the virtual functions:
	- CreateShaders
	- InitializeFog
	- SetShaderConstants
	- PreRender
	- PostRender

Folder ExampleAll:
- Contains a VC++6.0 workspace, to build all examples

Folder ExampleOne
- Example for: Linear Fog
- ExampleOne.h/.cpp contain a class derived from ExampleBase that implements the linear fog technique.
- LinarFog.vsh: vertex shader, used in this example

Folder ExampleTwo
- Example for: Exponential Fog
- ExampleTwo.h/.cpp contain a class derived from ExampleBase that implements the exponential fog technique.
- ExpFog.vsh/.fps: vertex shader/pixel shader, used in this example

Folder ExampleThree
- Example for: Exponential Squared Fog
- ExampleThree.h/.cpp contain a class derived from ExampleBase that implements the exponential squared fog technique.
- Exp2Fog.vsh/.fps: vertex shader/pixel shader, used in this example

Folder ExampleFour
- Example for: Layerd Fog
- ExampleFour.h/.cpp contain a class derived from ExampleBase that implements the layered fog technique.
- LayeredFog.vsh/.fps: vertex shader/pixel shader, used in this example

Folder ExampleFive
- Example for: Animated Fog
- ExampleFive.h/.cpp contain a class derived from ExampleBase that implements the animated fog technique.
- AnimatedFog.vsh: vertex shader, used in this example


Markus Nuebel