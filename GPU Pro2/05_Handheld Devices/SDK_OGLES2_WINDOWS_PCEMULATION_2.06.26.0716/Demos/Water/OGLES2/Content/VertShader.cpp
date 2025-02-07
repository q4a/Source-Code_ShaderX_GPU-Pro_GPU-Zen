// This file was created by Filewrap 1.1
// Little endian mode
// DO NOT EDIT

#include "../PVRTMemoryFileSystem.h"

// using 32 bit to guarantee alignment.
#ifndef A32BIT
 #define A32BIT static const unsigned int
#endif

// ******** Start: VertShader.vsh ********

// File data
static const char _VertShader_vsh[] = 
	"attribute highp vec3  inVertex;\r\n"
	"\r\n"
	"uniform highp mat4  ModelViewMatrix;\r\n"
	"uniform highp mat4  MVPMatrix;\r\n"
	"uniform highp vec3  EyePosition;\t\t// Eye (aka Camera) positon in model-space\r\n"
	"uniform mediump vec2 BumpTranslation0;\r\n"
	"uniform mediump vec2 BumpScale0;\r\n"
	"uniform mediump vec2 BumpTranslation1;\r\n"
	"uniform mediump vec2 BumpScale1;\r\n"
	"\r\n"
	"uniform highp float PerturbScale;\r\n"
	" \r\n"
	"varying mediump vec2 BumpCoord0;\r\n"
	"varying mediump vec2 BumpCoord1;\r\n"
	"varying mediump vec3\tWaterToEye;\r\n"
	"varying mediump float\tWaterToEyeLength;\r\n"
	"\r\n"
	"void main()\r\n"
	"{\r\n"
	"\t// Convert each vertex into projection-space and output the value\r\n"
	"\thighp vec4 vInVertex = vec4(inVertex, 1.0);\r\n"
	"\tgl_Position = MVPMatrix * vInVertex;\r\n"
	"\r\n"
	"\t// The texture coordinate is calculated this way to reduce the number of attributes needed\r\n"
	"\tmediump vec2 vTexCoord = inVertex.xz;\r\n"
	"\r\n"
	"\t// Scale and translate texture coordinates used to sample the normal map - section 2.2 of white paper\r\n"
	"\tBumpCoord0 = vTexCoord.xy * BumpScale0;\r\n"
	"\tBumpCoord0 += BumpTranslation0;\r\n"
	"\t\r\n"
	"\tBumpCoord1 = vTexCoord.xy * BumpScale1;\r\n"
	"\tBumpCoord1 += BumpTranslation1;\r\n"
	"\t\r\n"
	"\t/* \t\r\n"
	"\t\tThe water to eye vector is used to calculate the Fresnel term\r\n"
	"\t\tand to fade out perturbations based on distance from the viewer\r\n"
	"\t*/\r\n"
	"\tWaterToEye = EyePosition - inVertex;\r\n"
	"\tWaterToEyeLength = length(WaterToEye);\r\n"
	"}\r\n";

// Register VertShader.vsh in memory file system at application startup time
static CPVRTMemoryFileSystem RegisterFile_VertShader_vsh("VertShader.vsh", _VertShader_vsh, 1311);

// ******** End: VertShader.vsh ********

// This file was created by Filewrap 1.1
// Little endian mode
// DO NOT EDIT

#include "../PVRTMemoryFileSystem.h"

// using 32 bit to guarantee alignment.
#ifndef A32BIT
 #define A32BIT static const unsigned int
#endif

// ******** Start: VertShader.vsc ********

// File data
A32BIT _VertShader_vsc[] = {
0x10fab438,0x90c801ab,0x30050100,0x2101,0xa9142cc2,0x0,0x0,0xab040000,0x0,0x4000000,0x0,0x79000000,0x2020203,0x0,0x2020000,0x202,0x0,0xae020000,0x55535020,0x17,0x2a2,0x1,0x0,0x80c,0x0,0x2,0x79,0x0,0x8,0x0,0xffffffff,0x0,0x76000a,0xffff,0x20003,0x0,0x1b0000,0x0,0x0,0x0,0x0,0xfffc0000,0x0,0x0,0x0,0x20000,0xffffffff,0x190004,0x20003,0x14,0x20000,0x15,0x20010,0x10016,0x20000,0x10017,0x20010,0x20018,0x20000,0x20019,0x20010,0x3001a,0x20000,0x3001b,
0x10010,0x40001,0x10000,0x50002,0x10000,0x60003,0x10000,0x70004,0x10000,0x80005,0x10000,0x90006,0x10000,0xa0007,0x10000,0xb0008,0x10000,0xc0009,0x10000,0xd000a,0x10000,0xe000b,0x10000,0xf000c,0x10000,0x10000d,0x10000,0x11000e,0x10000,0x12000f,0x10000,0x130010,0x10000,0x140011,0x10000,0x150012,0x10000,0x160013,0x5800000,0x1bbea420,0x5804081,0x1bbea162,0x6804081,0x1bbea440,0x6804081,0x1bbea1a2,0x74081,0x40000,0x20000,0x2,0x1000f,0x80018001,0x80018001,0x0,0x0,0x4,0x10001,0x10001,0x10001,0x10001,0x10001,0x10001,0x10001,0x0,
0x1080fd10,0x1003e000,0x1a0e28a9,0x20857080,0x20400a2,0x10005000,0x2801080,0x10006020,0x42802881,0x10004000,0x3000080,0x10006020,0x43002881,0x10004000,0x80,0x12000000,0x880,0x10010160,0x2a580880,0x205f0e0,0x89238a2,0x605f120,0x4a963898,0x604f000,0x8c9a3882,0x604f000,0x1a1d3882,0x1001701f,0x200a2,0x20000,0x80010001,0x80028002,0x8001,0x0,0x40000,0x2010000,0x102,0x580fa10,0x1003e000,0x228a9,0x60000,0x80010001,0x80008001,0x8001,0x0,0x50000,0x10001,0x10001,0x10001,0x101,0xaad9fa10,0x205f100,0x99438a2,0x605f140,0x4b983898,0x606f000,0x8d9c3882,0x606f000,0x1a1f3882,0x1001705f,0x600a2,0x1c000000,0x803f,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x6e690c00,0x74726556,0x7865,0x40400,0x100,0x4000001,0x700,0x505f6c67,0x7469736f,0x6e6f69,0x5050100,0x10000,0x100,0xf0004,0x50564d00,0x7274614d,0x7869,0x31600,0x100,0x10040001,0xffff,0x706d7542,0x726f6f43,0x3064,0x50300,0x100,0x2040001,0x300,0x706d7542,0x6c616353,0x3065,0x30300,0x100,0x2140001,0x300,0x706d7542,0x6e617254,0x74616c73,
0x306e6f69,0x3000000,0x1000003,0x10000,0x3000216,0x75420000,0x6f43706d,0x3164726f,0x3000000,0x1000005,0x10000,0x3000208,0x75420000,0x6353706d,0x31656c61,0x3000000,0x1000003,0x10000,0x3000218,0x75420000,0x7254706d,0x6c736e61,0x6f697461,0x316e,0x30300,0x100,0x21a0001,0x300,0x50657945,0x7469736f,0x6e6f69,0x3040000,0x10000,0x1000100,0x70003,0x74615700,0x6f547265,0x657945,0x5040000,0x10000,0x100,0x70003,0x74615700,0x6f547265,0x4c657945,0x74676e65,0x68,0x502,0x1000001,0x10c00,0x1,
};

// Register VertShader.vsc in memory file system at application startup time
static CPVRTMemoryFileSystem RegisterFile_VertShader_vsc("VertShader.vsc", _VertShader_vsc, 1227);

// ******** End: VertShader.vsc ********

