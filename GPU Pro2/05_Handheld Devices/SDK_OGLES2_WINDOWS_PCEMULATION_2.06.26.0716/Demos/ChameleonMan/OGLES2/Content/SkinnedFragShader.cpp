// This file was created by Filewrap 1.1
// Little endian mode
// DO NOT EDIT

#include "../PVRTMemoryFileSystem.h"

// using 32 bit to guarantee alignment.
#ifndef A32BIT
 #define A32BIT static const unsigned int
#endif

// ******** Start: SkinnedFragShader.fsh ********

// File data
static const char _SkinnedFragShader_fsh[] = 
	"uniform sampler2D sTexture;\r\n"
	"uniform sampler2D sNormalMap;\r\n"
	"uniform bool bUseDot3;\r\n"
	"\r\n"
	"varying mediump vec2 TexCoord;\r\n"
	"varying mediump vec3 Light;\r\n"
	"\r\n"
	"void main()\r\n"
	"{\r\n"
	"\tif(bUseDot3)\r\n"
	"\t{\r\n"
	"\t\t/*\r\n"
	"\t\t\tNote:\r\n"
	"\t\t\tIn the normal map red = y, green = x, blue = z which is why when we get the normal\r\n"
	"\t\t\tfrom the texture we use the swizzle .grb so the colours are mapped to the correct\r\n"
	"\t\t\tco-ordinate variable.\r\n"
	"\t\t*/\r\n"
	"\r\n"
	"\t\tmediump vec3 fNormal = texture2D(sNormalMap, TexCoord).grb;\r\n"
	"\t\tmediump float fNDotL = dot((fNormal - 0.5) * 2.0, Light);\r\n"
	"\t\t\r\n"
	"\t\tgl_FragColor = texture2D(sTexture, TexCoord) * fNDotL;\r\n"
	"    }\r\n"
	"    else\r\n"
	"\t\tgl_FragColor = texture2D(sTexture, TexCoord) * Light.x;\r\n"
	"}\r\n";

// Register SkinnedFragShader.fsh in memory file system at application startup time
static CPVRTMemoryFileSystem RegisterFile_SkinnedFragShader_fsh("SkinnedFragShader.fsh", _SkinnedFragShader_fsh, 672);

// ******** End: SkinnedFragShader.fsh ********

// This file was created by Filewrap 1.1
// Little endian mode
// DO NOT EDIT

#include "../PVRTMemoryFileSystem.h"

// using 32 bit to guarantee alignment.
#ifndef A32BIT
 #define A32BIT static const unsigned int
#endif

// ******** Start: SkinnedFragShader.fsc ********

// File data
A32BIT _SkinnedFragShader_fsc[] = {
0x10fab438,0x81ee0ce4,0x30050100,0x2101,0xa9142cc2,0x0,0x0,0xe5060000,0x1000000,0x4000000,0x0,0x18000000,0x203,0x0,0x2020000,0x0,0x0,0x12030000,0x55535020,0x17,0x306,0x1,0x0,0x848,0x0,0x2,0x79,0x0,0x8,0x0,0xffffffff,0x0,0x770009,0xffff,0x60004,0x0,0x30003,0x0,0x0,0x0,0x0,0xfffc0000,0x20003,0x10004,0x20001,0x20000,0xffffffff,0x20000,0x60000,0x10000,0x20003,0x0,0x2,0x0,0x20001,0x0,0x20002,0x0,0x30002,0x100000,0x1,0x1,0x4,0x20002,
0x20000,0x80010001,0x80018001,0x8001,0x0,0x40000,0x10000,0x0,0x530fd10,0x1281d000,0x348a9,0x0,0x400000,0x3fd00,0x5,0x10001,0x80018001,0x80018001,0x0,0x0,0x0,0x70007,0x10688,0x10001,0x1,0x20001,0x10003,0x10001,0x1,0x0,0x10000,0x20006,0x20002,0x20000,0x80010001,0x40a140a1,0x8001,0x0,0x40000,0xa1010000,0x601a1,0xfa10,0x20002060,0x228a1,0x20000,0x80010001,0x80008001,0x8000,0x0,0x40000,0x0,0x101,0xe953fa10,0x2220f000,0x200e0,0x20000,0x80000001,0x80018001,0x8001,0x0,0x40000,0x1010000,0x1,0x40fa10,
0x20002000,0x510e0,0x10000,0x80000001,0x80018001,0x8001,0x0,0x0,0xf0001,0x688000f,0x30003,0x30003,0x0,0x10001,0x20002,0x20002,0x20000,0x20000,0x60001,0x20002,0x2,0x10004,0x80018000,0x80018001,0x0,0x0,0x130000,0x110000,0xa0002041,0x38ab0602,0xf0440001,0x40831c3e,0xa0202143,0x38ab0602,0xf0440001,0x40831c26,0x3,0x0,0xf8000040,0x40004,0x30000,0x5,0x10001,0x80018001,0x80018001,0x0,0x0,0x10000,0xf000f,0x30688,0x30003,0x20003,0x30002,0x20003,0x20002,0x2,0x2,0x10002,0x20006,0x20002,0x40000,0x80010001,0x80018001,0x8001,
0x0,0x0,0x13,0x22450011,0x606a050,0x138ab,0x1c3ef044,0x23474083,0x606a070,0x138ab,0x1c26f044,0x44083,0x40000,0x2,0x1,0x80018001,0x80018001,0x0,0x0,0x1010004,0x101,0x6fa10,0x0,0x50201203,0x175553,0x3060000,0x10000,0x0,0x9480000,0x0,0x20000,0x790000,0x0,0x80000,0x0,0xffff0000,0xffff,0x90000,0xffff0077,0x40000,0x6,0x30000,0x3,0x0,0x0,0x0,0x0,0x1fffc,0x40000,0x1,0x1,0xffff0002,0xffff,0x2,0x4,0x30001,0x2,0x20000,0x0,0x10000,0x2,0x20000,0x2,0x20000,
0x3,0x10010,0x10000,0x40000,0x20000,0x2,0x10002,0x80018001,0x80018001,0x0,0x0,0x4,0x1,0xfd100000,0xd0000530,0x48a91281,0x3,0x0,0xfd000040,0x50003,0x10000,0x80010001,0x80018001,0x8001,0x0,0x0,0x70000,0x6880007,0x10001,0x10001,0x10000,0x30002,0x10001,0x10001,0x0,0x0,0x60001,0x20002,0x2,0x10002,0x40a18001,0x800140a1,0x0,0x0,0x4,0x1a1a101,0xfa100006,0x20600000,0x28a12000,0x2,0x10002,0x80018001,0x80008000,0x0,0x0,0x4,0x1010000,0xfa100000,0xf000e953,0xe02220,0x2,0x10002,0x80018000,0x80018001,
0x0,0x0,0x4,0x10101,0xfa100000,0x20400040,0x10e02000,0x5,0x10001,0x80018000,0x80018001,0x0,0x0,0x10000,0xf000f,0x10688,0x10001,0x1,0x10000,0x20001,0x20002,0x2,0x2,0x10002,0x20006,0x20002,0x40000,0x80000001,0x80018001,0x8001,0x0,0x0,0x13,0xa0410011,0x6000000,0x138ab,0x1c3cf004,0xa1434083,0x6000020,0x138ab,0x1c24f004,0x34083,0x0,0x400000,0x4f800,0x4,0x50003,0x10000,0x80010001,0x80018001,0x8001,0x0,0x0,0xf0001,0x688000f,0x10001,0x10001,0x0,0x10001,0x20002,0x20002,0x20000,0x20000,0x60001,
0x20002,0x2,0x10004,0x80018001,0x80018001,0x0,0x0,0x130000,0x110000,0x102041,0x38ab0604,0xf0040001,0x40831c3c,0x302143,0x38ab0604,0xf0040001,0x40831c24,0x4,0x20004,0x10000,0x80010000,0x80018001,0x8001,0x0,0x40000,0x1010101,0xfa100000,0x6,0x400,0x0,0x3f0000,0x400000,0x5000000,0x65735562,0x33746f44,0xa000000,0x1000003,0x10000,0x1000100,0x4e730000,0x616d726f,0x70614d6c,0x18000000,0x1000003,0x20000,0x1000100,0x65540000,0x6f6f4378,0x6472,0x50300,0x100,0x2040001,0x300,0x6867694c,0x74,0x504,0x1000001,0x30000,0x73000007,0x74786554,0x657275,0x3180000,0x10000,0x1000200,
0x10001,0x0,
};

// Register SkinnedFragShader.fsc in memory file system at application startup time
static CPVRTMemoryFileSystem RegisterFile_SkinnedFragShader_fsc("SkinnedFragShader.fsc", _SkinnedFragShader_fsc, 1797);

// ******** End: SkinnedFragShader.fsc ********

