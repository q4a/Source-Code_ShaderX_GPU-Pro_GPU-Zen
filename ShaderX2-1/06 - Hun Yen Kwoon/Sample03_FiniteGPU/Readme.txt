//-----------------------------------------------------------------------------
// Topic:   Stencil Shadow volumes - FiniteGPU Sample 
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
// This sample showcases the offloading of the entire shadow volume 
// construction to the graphics hardware through the use of vertex shaders.
// The source geometry of the occluder have to be preprocessed with selective
// insertion of degenerate quads amongst shared edges in order to allow the 
// formation of shadow volumes in hardware without the creation of new vertex
// data. This sample extrudes the shadow volume to a finite distance using 
// normal vector manipulation.
// Press "F1" for more control keys.
//
// This sample is based on DirectX8.1 and hand-coded assembly vertex shader.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Implementation Notes
// In debug mode, the preprocessing function CShadow::Create() would create a 
// text file "PreprocessLog.txt", which contains information about the pre and 
// post total vertices count and any errors during preprocessing.
// Tweak the MERGE_COS_ANGLE_PROXIMITY constants between 0.0 and 1.0 to achieve
// the least vertices count without visual artifacts. Values between 0.95 and 
// 0.80 should work very well for most models.
//-----------------------------------------------------------------------------