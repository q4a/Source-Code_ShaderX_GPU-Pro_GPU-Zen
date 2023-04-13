//-----------------------------------------------------------------------------
// Topic:   Stencil Shadow volumes - DepthPassCPU Sample 
// Book:    ShaderX2
// Site:    www.shaderx2.com
// Author:  Hun Yen Kwoon 
//          2003 March
// Contact: ykhun@PacketOfMilk.com
//          To prevent your mails from being junked, please precede your mail 
//          subject with the text "ShaderX2".
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Description
// DepthPass stencil shadow algorithm suffers from the problem of near plane
// clipping. Fly the camera to a location where the shadow volume will cut the
// near plane. Artifacts will appear with erroneous shadowing. On the other 
// hand, the DepthFail stencil shadow algorithm rectify this near plane 
// clipping problem. However, the tradeoff is the need to render capping 
// geometries for the shadow volume, which made DepthFail more expensive in 
// terms of computation and rasterization resources.
// Press "N" or "B" to expose the shadow volume faces for easy navigation.
// Press "F1" for more control keys.
//
// This sample is based on DirectX8.1.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Implementation Notes
// Take note of the instantiation of the CShadow class. It takes in a number 
// which is a hint of the amount of memory to allocate for internal arrays used
// during shadow volume construction. If you encounter memory access errors 
// while using different models, set a higher value to allocate more memory.
// Typical size should be: (num of vertices of model * 3.5)
//
// The sample takes advantage of welded models that are constructed from the 
// original models using the DirectX utility MeshViewer.
//-----------------------------------------------------------------------------

Addenda:

On some hardware, the camera does not behave as expected in the example programs. This problem could be solved by simply by adding the following code in the inherited CD3DApplication class constructor within the main .cpp source file:

ZeroMemory( &m_matOrientation, sizeof( D3DXMATRIX ) );