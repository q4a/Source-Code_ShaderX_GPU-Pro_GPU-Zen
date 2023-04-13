///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgMatrix
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgMatrix.h,v 1.4 2003/01/05 15:10:34 markus Exp $
///////////////////////////////////////////////////////////////////


#ifndef __MATRIX_HEADERFILE__
#define __MATRIX_HEADERFILE__

#include <pgLib/pgLib.h>


class pgVec2;
class pgVec3;
class pgVec3n;
class pgVec4;
struct D3DXMATRIX;


#pragma warning (push)
#pragma warning (disable: 4201)		// disable warning about nameless struct/union

class PGLIB_API pgMatrix
{
public:
	pgMatrix();
	pgMatrix(const float* nElements);
	pgMatrix(const D3DXMATRIX& nMat);

	const D3DXMATRIX* getD3DMatrix() const  {  return reinterpret_cast<const D3DXMATRIX*>(this);  }

	D3DXMATRIX* getD3DMatrix()  {  return reinterpret_cast<D3DXMATRIX*>(this);  }


	void makeIdent();

	void makeRotZXY(pgRad nHead, pgRad nPitch, pgRad nRoll);

	void makeTrans(const pgVec3& nTrans);

	bool makeLookat(const pgVec3n& nDir, const pgVec3n& nUp);

	void makeScale(const pgVec3& scale);

	void makeScale(float s);

	void makeProjection(float nFovY, float nAspect, float nNear, float nFar);

	bool invert();


    void preMult(const pgMatrix& lf);

    void postMult(const pgMatrix& rt);

	void multVector(pgVec4& dst, const pgVec4& src) const;

	void multVector(pgVec4& dst, const pgVec3& src) const;

	void multVector(pgVec3& dst, const pgVec3& src) const;

	//accessing functions
    typedef float float4[4];

    const float4&
		operator[](unsigned nIndex) const  {  return reinterpret_cast<const float4*>(&m00)[nIndex];  }

    float4&
		operator[](unsigned nIndex)  {  return reinterpret_cast<float4*>(&m00)[nIndex];  }

    const pgVec4& 
		rowVec4(unsigned nIndex) const  {  return reinterpret_cast<const pgVec4&>((&m00)[nIndex*4]);  }

    pgVec4& 
		rowVec4(unsigned nIndex)  {  return reinterpret_cast<pgVec4&>((&m00)[nIndex*4]);  }


    const pgVec3& 
		rowVec3(unsigned nIndex)  const  {  return reinterpret_cast<const pgVec3&>((&m00)[nIndex*4]);  }

    pgVec3& 
		rowVec3(unsigned nIndex)  {  return reinterpret_cast<pgVec3&>((&m00)[nIndex*4]);  }

	pgMatrix&
		operator=(const D3DXMATRIX& nMat);

	bool
		operator==(const pgMatrix& nMat) const;


protected:
    union
	{
        struct
		{
            float	m00, m01, m02, m03;
            float	m10, m11, m12, m13;
            float	m20, m21, m22, m23;
            float	m30, m31, m32, m33;
        };
        float m[4][4];
    };


    friend class pgVec2;
    friend class pgVec3;
    friend class pgVec4;
	friend class pgQuat;
	friend class pgTransform;
};

#pragma warning (pop)

#endif //__MATRIX_HEADERFILE__
