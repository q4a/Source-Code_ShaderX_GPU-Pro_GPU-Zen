/******************************************************************************

 @File         PVRTPrint3DShaders.h

 @Title        OGLES2\PVRTPrint3DShaders

 @Version      

 @Copyright    Copyright (C)  Imagination Technologies Limited.

 @Platform     ANSI compatible

 @Description  The shaders used by Print3D. Created by Filewrap 1.0. DO NOT EDIT.

******************************************************************************/

// This file was created by Filewrap 1.0
// DO NOT EDIT

// using 32 bit to guarantee alignment. Adds endianness dependency.
#ifndef A32BIT
 #define A32BIT static const unsigned int
#endif

// ******** Start: Print3DFragShader.fsh ********

// File data
static const char _Print3DFragShader_fsh[] =
	"uniform sampler2D\tsampler2d;\r\n"
	"\r\n"
	"varying lowp vec4\t\tvarColour;\r\n"
	"varying mediump vec2\ttexCoord;\r\n"
	"\r\n"
	"void main()\r\n"
	"{\r\n"
	"\tgl_FragColor = varColour * texture2D(sampler2d, texCoord);\r\n"
	"}\r\n";

static int _Print3DFragShader_fsh_size = 177;

// ******** End: Print3DFragShader.fsh ********

// ******** Start: Print3DVertShader.vsh ********

// File data
static const char _Print3DVertShader_vsh[] =
	"attribute highp vec4\tmyVertex;\r\n"
	"attribute mediump vec2\tmyUV;\r\n"
	"attribute lowp vec4\t\tmyColour;\r\n"
	"\r\n"
	"uniform highp mat4\t\tmyMVPMatrix;\r\n"
	"\r\n"
	"varying lowp vec4\t\tvarColour;\r\n"
	"varying mediump vec2\ttexCoord;\r\n"
	"\r\n"
	"void main()\r\n"
	"{\r\n"
	"\tgl_Position = myMVPMatrix * myVertex;\r\n"
	"\ttexCoord = myUV.st;\r\n"
	"\tvarColour = myColour;\r\n"
	"}\r\n";

static int _Print3DVertShader_vsh_size = 302;

// ******** End: Print3DVertShader.vsh ********

// This file was created by Filewrap 1.0
// DO NOT EDIT

// using 32 bit to guarantee alignment. Adds endianness dependency.
#ifndef A32BIT
 #define A32BIT PVRTuint32
#endif

// ******** Start: Print3DFragShader.fsc ********

// File data
A32BIT _Print3DFragShader_fsc[] = {
0x10fab438,0x1f3f40cd,0x35050100,0x901,0xf121629f,0x0,0x0,0xb8020000,
0x1000000,0x4000100,0x0,0x18000000,0x2a0204,0xa080100,0x201a200,0xa1201,
0x100007e,0x1c010000,0x55535020,0x17,0x110,0x1,0x0,0x48,
0x0,0x3,0x6d,0x1,0x0,0x0,0xffffffff,0x0,
0x770009,0xffff,0x40005,0x0,0x2,0x0,0x0,0x0,
0x0,0xfffc0000,0x3,0x10004,0x0,0x110000,0xffffffff,0x0,
0x56780000,0x40000,0x10000,0x10004,0x0,0x2,0x40000,0x110000,
0x5,0x1,0x80018001,0x80018001,0x0,0x0,0x0,0xf000f,
0x10688,0x10001,0x1,0x20001,0x10003,0x10001,0x1,0x0,
0x10000,0x20004,0x20002,0x30000,0x80010000,0x80018001,0x8001,0x0,
0x0,0x100012,0x10000030,0xa13006,0xa0040080,0x40801c1a,0xa0040103,0x40801c26,
0x6,0x1c010000,0x55535020,0x17,0x110,0x1,0x0,0x148,
0x0,0x3,0x6d,0x1,0x0,0x0,0xffffffff,0x0,
0x770009,0xffff,0x40005,0x0,0x2,0x0,0x0,0x0,
0x0,0xfffc0000,0x1,0x10004,0x0,0x110000,0xffffffff,0x0,
0x56780000,0x40000,0x10000,0x10004,0x0,0x2,0x40000,0x110000,
0x5,0x1,0x80018001,0x80018001,0x0,0x0,0x0,0xf000f,
0x10688,0x10001,0x1,0x20001,0x10003,0x10001,0x1,0x0,
0x10000,0x20004,0x20002,0x30000,0x80010000,0x80018001,0x8001,0x0,
0x0,0x100012,0x10000030,0xa13004,0x40080,0x40801c18,0x40103,0x40801c24,
0x6,0x3000000,0x706d6173,0x3272656c,0x64,0x318,0x2000001,0x10000,
0x74000001,0x6f437865,0x64726f,0x5030000,0x10000,0x4000100,0x30002,0x72617600,
0x6f6c6f43,0x7275,0x50500,0x100,0x4000001,0xf00,0x0,
};

static int _Print3DFragShader_fsc_size = 728;

// ******** End: Print3DFragShader.fsc ********

// ******** Start: Print3DVertShader.vsc ********

// File data
A32BIT _Print3DVertShader_vsc[] = {
0x10fab438,0x322ecb01,0x35050100,0x901,0xf121629f,0x0,0x0,0xdf020000,
0x0,0x4000200,0x0,0x19000000,0x204,0x0,0x2010000,0x0,
0x0,0xaa010000,0x55535020,0x17,0x19e,0x1,0x0,0xc,
0x0,0x3,0x6d,0x1,0x8,0x0,0xffffffff,0x0,
0x76000a,0xffff,0xc,0x0,0x110000,0x0,0x0,0x0,
0x0,0xfffc0000,0x0,0x0,0x0,0x110000,0xffffffff,0x110000,
0x5678000c,0x10000,0x0,0x10000,0x10001,0x10000,0x20002,0x10000,
0x30003,0x10000,0x40004,0x10000,0x50005,0x10000,0x60006,0x10000,
0x70007,0x10000,0x80008,0x10000,0x90009,0x10000,0xa000a,0x10000,
0xb000b,0x10000,0xc000c,0x10000,0xd000d,0x10000,0xe000e,0x10000,
0xf000f,0x40000,0x3f000000,0x10,0xf3f,0x4,0x20011,0x20000,
0x80010000,0x80018001,0x8001,0x0,0x0,0x2000000,0x1001a100,0x40028a1,
0x3001a080,0x228a1,0x80000,0x80000000,0x80048001,0x8004,0x0,0x40000,
0x0,0x10001,0x10001,0x4040001,0x1,0x700fa10,0x100260a0,0x50b2883,
0x3205f000,0x60d10a8,0x605f020,0x48113898,0x606f000,0x8a153882,0x606f000,0xcc193882,
0x605f060,0x4d0538b2,0xc005e041,0x638a0,0x14000000,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x3f,0x3f,0x0,
0x0,0x6c670700,0x736f505f,0x6f697469,0x100006e,0x505,0x1000001,0x40000,
0x6d00000f,0x50564d79,0x7274614d,0x7869,0x31600,0x100,0x10000001,0xffff,
0x6556796d,0x78657472,0x5000000,0x1000004,0x10000,0xf000400,0x65740000,0x6f6f4378,
0x6472,0x50300,0x100,0x2040001,0x300,0x5655796d,0x3000000,0x1000004,
0x10000,0x3000404,0x61760000,0x6c6f4372,0x72756f,0x5050000,0x10000,0x100,
0xf0004,0x43796d00,0x756f6c6f,0x72,0x405,0x1000001,0x40800,0xf,
};

static int _Print3DVertShader_vsc_size = 767;

// ******** End: Print3DVertShader.vsc ********


