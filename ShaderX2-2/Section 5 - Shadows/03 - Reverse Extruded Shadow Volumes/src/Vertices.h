
#ifndef __VERTICES_H
#define __VERTICES_H

#pragma pack(1)


#define FVF_XYZ D3DFVF_XYZ
struct SVertexXyz {
	D3DXVECTOR3	p;
};

#define FVF_XYZ_NORMAL D3DFVF_XYZ | D3DFVF_NORMAL
struct SVertexXyzNormal {
	D3DXVECTOR3	p;
	D3DXVECTOR3	n;
};

#define FVF_XYZ_TEX1 D3DFVF_XYZ | D3DFVF_TEX1
struct SVertexXyzTex1 {
	D3DXVECTOR3	p;
	float tu, tv;
};

#define FVF_XYZ_DIFFUSE D3DFVF_XYZ | D3DFVF_DIFFUSE
struct SVertexXyzDiffuse {
	D3DXVECTOR3	p;
	DWORD diffuse;
};

#define FVF_XYZ_DIFFUSE_TEX1 D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1
struct SVertexXyzDiffuseTex1 {
	D3DXVECTOR3	p;
	DWORD diffuse;
	float tu, tv;
};

#define FVF_XYZRHW_TEX1 D3DFVF_XYZRHW | D3DFVF_TEX1
struct SVertexXyzRhwTex1 {
	D3DXVECTOR4	p;
	float tu, tv;
};

#define FVF_XYZRHW_DIFFUSE_TEX1 D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1
struct SVertexXyzRhwDiffuseTex1 {
	D3DXVECTOR4	p;
	DWORD diffuse;
	float tu, tv;
};

#define FVF_XYZ_NORMAL_TEX1 D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
struct SVertexXyzNormalTex1 {
	D3DXVECTOR3	p;
	D3DXVECTOR3	n;
	float tu, tv;
};

#define FVF_XYZ_NORMAL_DIFFUSE_TEX1 D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1
struct SVertexXyzNormalDiffuseTex1 {
	D3DXVECTOR3	p;
	D3DXVECTOR3	n;
	DWORD diffuse;
	float tu, tv;
};

#define FVF_XYZ_NORMAL_DIFFUSE D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE
struct SVertexXyzNormalDiffuse {
	D3DXVECTOR3	p;
	D3DXVECTOR3	n;
	DWORD diffuse;
};


#pragma pack()


#endif
