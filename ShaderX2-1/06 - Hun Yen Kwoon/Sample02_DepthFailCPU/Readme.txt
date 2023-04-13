//-----------------------------------------------------------------------------
// Topic:   Stencil Shadow volumes - DepthFailCPU Sample 
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
// DepthFail stencil shadow algorithm is a more robust way of doing shadow 
// volumes because it does not suffer from artifacts when the camera is in 
// shadow. However, it requries a closed shadow volume with cappings at both 
// front and back ends. Such shadows volumes always contains more polygons and 
// hence increases the cost of rendering.
// Efficient implementations should actively test and switch to the cheaper 
// DepthPass algorithm whenever possible.
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