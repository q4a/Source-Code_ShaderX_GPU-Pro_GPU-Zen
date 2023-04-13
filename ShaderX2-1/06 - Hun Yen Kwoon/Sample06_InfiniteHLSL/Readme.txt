//-----------------------------------------------------------------------------
// Topic:   Stencil Shadow volumes - InfiniteHLSL Sample ( DX9.0 HLSL )
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
// data. This sample extrudes the shadow volume to an infinite distance using 
// homogenous coordinates with w = 0.
// Press "F1" for more control keys.
// 
// This sample is based on DirectX9.0 and hence it is recommended to run it 
// on hardware that fully supports DirectX9 or above. The 2 required caps are
// the depth bias and 2-sided stencil mode caps. The effects file 
// HLSL_ShadowVolume.fx hold 2 techniques, one implements the old 1-sided
// stencil mode while the other implements the new 2-sided stencil mode.
// Press "K" to toggle the stencil mode, if both are supported.
//
// The sample requires the device to support the depth bias and 2-sided stencil
// mode. In the event the device is SW or REF, each frame may take a couple of
// seconds to render. For best results, use a Radeon9700 and above or GeForceFX
// and above graphics card. Make sure you have the latest drivers installed as
// some older drivers have problems emulating DX9.0 depth-biasing.
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