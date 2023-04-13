///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgMatrix
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgMatrix.cpp,v 1.3 2003/01/05 15:10:34 markus Exp $
///////////////////////////////////////////////////////////////////


#include "pgMatrix.h"

#include <pgLib/Math/pgVec3.h>
#include <pgLib/Math/pgVec3n.h>
#include <pgLib/Math/pgVec4.h>

#include <D3DX8.h>
#include <assert.h>
#include <math.h>


void sincos(pgRad a, float& s, float& c)
{
	s = (float)sin(a);
	c = (float)cos(a);
}


pgMatrix::pgMatrix()
{
	makeIdent();
}


pgMatrix::pgMatrix(const float* nElements)
{
	memcpy(&m00, nElements, 16*sizeof(float));
}


pgMatrix::pgMatrix(const D3DXMATRIX& nMat)
{
	*getD3DMatrix() = nMat;
}


void
pgMatrix::makeIdent()
{
    m00 = m11 = m22 = m33 = 1.0f;
    m01 = m02 = m03 =
    m10 = m12 = m13 =
    m20 = m21 = m23 =
    m30 = m31 = m32 = 0.0f;
}


void
pgMatrix::makeProjection(float nFovY, float nAspect, float nNear, float nFar)
{
	float h = (float)(cos(nFovY/2) / sin(nFovY/2));
	float w = h / nAspect;

	m00 = w; //2*nNear/w;
	m01 = m02 = m03 = m10 = 0.0f;

	m11 = h; //2*nNear/h;
	m12 = m13 = m20 = m21 = 0.0f;
	m22 = nFar/(nFar-nNear);
	m23 = 1.0f;
	m30 = m31 = 0.0f;
	m32 = nNear*nFar/(nNear-nFar);
	m33 = 0.0f;
}


void
pgMatrix::makeRotZXY(pgRad nHead, pgRad nPitch, pgRad nRoll)
{
    float sinY;
    float cosY;
    sincos(nHead, sinY, cosY);
    float sinX;
    float cosX;
    sincos(nPitch, sinX, cosX);
    float sinZ;
    float cosZ;
    sincos(nRoll, sinZ, cosZ);

    float cosYxcosZ = cosY*cosZ;
    float sinYxsinZ = sinY*sinZ;
    float sinYxcosZ = sinY*cosZ;
    float cosYxsinZ = cosY*sinZ;
    m00 = sinX*sinYxsinZ + cosYxcosZ;
    m01 = - cosX*sinZ;
    m02 = sinX*cosYxsinZ - sinYxcosZ;
    m10 = cosYxsinZ - sinX*sinYxcosZ;
    m11 = cosX*cosZ;
    m12 = - sinX*cosYxcosZ - sinYxsinZ;
    m20 = cosX*sinY;
    m21 = sinX;
    m22 = cosX*cosY;
    m03 = m13 = m23 = m30 = m31 = m32 = 0.0f;
    m33 = 1.0f;
}


void 
pgMatrix::makeTrans(const pgVec3& nTrans)
{
    m00 = m11 = m22 = m33 = 1.0f;
    m01 = m02 = m03 =
    m10 = m12 = m13 =
    m20 = m21 = m23 = 0.0f;
    m30 = nTrans[0];
    m31 = nTrans[1];
    m32 = nTrans[2];
}


bool
pgMatrix::makeLookat(const pgVec3n& nDir, const pgVec3n& nUp)
{
    pgVec3 vec1, vec2;
    vec1.cross( nUp, nDir );
    vec2.cross( nDir, vec1 );
    if (vec1.normalize()==0.0f || vec2.normalize()==0.0f)
        return false;

    m00 = vec1[0];
    m01 = vec1[1];
    m02 = vec1[2];

    m10 = vec2[0];
    m11 = vec2[1];
    m12 = vec2[2];

    m20 = nDir[0];
    m21 = nDir[1];
    m22 = nDir[2];

    m03 = m13 = m23 = m30 = m31 = m32 = 0.0f;
    m33 = 1.0f;
	return true;
}


void
pgMatrix::makeScale(const pgVec3& scale)
{
    m00 = scale[0];
	m11 = scale[1];
	m22 = scale[2];
    m01 = m02 = m03 =
    m10 = m12 = m13 =
    m20 = m21 = m23 =
    m30 = m31 = m32 = 0.0f;
    m33 = 1.0f;
}


void 
pgMatrix::makeScale(float s)
{
    m00 = m11 = m22 = s;
    m01 = m02 = m03 =
    m10 = m12 = m13 =
    m20 = m21 = m23 =
    m30 = m31 = m32 = 0.0f;
    m33 = 1.0f;
}


bool 
pgMatrix::invert()
{
    pgMatrix tmp(*this);

    // Gauss-Jordan elimination with partial pivoting
    makeIdent();
    int i, j, i1;

    // Loop over cols of a from left to right, eliminating above and below diag
    for (j = 0; j<4; j++)
    {
        // Find largest pivot in column j among rows j..3
        i1 = j;       // Row with largest pivot candidate
        for (i = j+1; i<4; i++)
            if (fabs(tmp[i][j])>fabs(tmp[i1][j]))
                i1 = i;

        // Swap rows i1 and j in tmp and this to put pivot on diagonal
        pgVec4 swap(tmp[i1]);
        tmp.rowVec4(i1) = tmp.rowVec4(j);
        tmp.rowVec4(j) = swap;
        swap = rowVec4(i1);
        rowVec4(i1) = rowVec4(j);
        rowVec4(j) = swap;

        // Scale row j to have a unit diagonal
        if (tmp[j][j]==0.f)
            return false;

        rowVec4(j) /= tmp[j][j];
        tmp.rowVec4(j) /= tmp[j][j];

        // Eliminate off-diagonal elems in col j of tmp, doing identical ops to this
        for (i = 0; i<4; i++)
            if (i!=j) 
            {
                rowVec4(i) -= tmp[i][j]*rowVec4(j);
                tmp.rowVec4(i) -= tmp[i][j]*tmp.rowVec4(j);
            }
    }

    return true;
}


void 
pgMatrix::preMult(const pgMatrix& nLf)
{
    assert(this!=&nLf);

    float r0,r1,r2;

    r0 = m00; r1 = m10; r2 = m20;
    m00 = nLf.m00*r0+nLf.m01*r1+nLf.m02*r2+nLf.m03*m30;
    m10 = nLf.m10*r0+nLf.m11*r1+nLf.m12*r2+nLf.m13*m30;
    m20 = nLf.m20*r0+nLf.m21*r1+nLf.m22*r2+nLf.m23*m30;
    m30 = nLf.m30*r0+nLf.m31*r1+nLf.m32*r2+nLf.m33*m30;

    r0 = m01; r1 = m11; r2 = m21;
    m01 = nLf.m00*r0+nLf.m01*r1+nLf.m02*r2+nLf.m03*m31;
    m11 = nLf.m10*r0+nLf.m11*r1+nLf.m12*r2+nLf.m13*m31;
    m21 = nLf.m20*r0+nLf.m21*r1+nLf.m22*r2+nLf.m23*m31;
    m31 = nLf.m30*r0+nLf.m31*r1+nLf.m32*r2+nLf.m33*m31;

    r0 = m02; r1 = m12; r2 = m22;
    m02 = nLf.m00*r0+nLf.m01*r1+nLf.m02*r2+nLf.m03*m32;
    m12 = nLf.m10*r0+nLf.m11*r1+nLf.m12*r2+nLf.m13*m32;
    m22 = nLf.m20*r0+nLf.m21*r1+nLf.m22*r2+nLf.m23*m32;
    m32 = nLf.m30*r0+nLf.m31*r1+nLf.m32*r2+nLf.m33*m32;

    r0 = m03; r1 = m13; r2 = m23;
    m03 = nLf.m00*r0+nLf.m01*r1+nLf.m02*r2+nLf.m03*m33;
    m13 = nLf.m10*r0+nLf.m11*r1+nLf.m12*r2+nLf.m13*m33;
    m23 = nLf.m20*r0+nLf.m21*r1+nLf.m22*r2+nLf.m23*m33;
    m33 = nLf.m30*r0+nLf.m31*r1+nLf.m32*r2+nLf.m33*m33;
}


void 
pgMatrix::postMult(const pgMatrix& nRt)
{
    assert(this!=&nRt);

    float l0, l1, l2;

    l0 = m00; l1 = m01; l2 = m02;
    m00 = l0*nRt.m00 + l1*nRt.m10 + l2*nRt.m20 + m03*nRt.m30;
    m01 = l0*nRt.m01 + l1*nRt.m11 + l2*nRt.m21 + m03*nRt.m31;
    m02 = l0*nRt.m02 + l1*nRt.m12 + l2*nRt.m22 + m03*nRt.m32;
    m03 = l0*nRt.m03 + l1*nRt.m13 + l2*nRt.m23 + m03*nRt.m33;

    l0 = m10; l1 = m11; l2 = m12;
    m10 = l0*nRt.m00 + l1*nRt.m10 + l2*nRt.m20 + m13*nRt.m30;
    m11 = l0*nRt.m01 + l1*nRt.m11 + l2*nRt.m21 + m13*nRt.m31;
    m12 = l0*nRt.m02 + l1*nRt.m12 + l2*nRt.m22 + m13*nRt.m32;
    m13 = l0*nRt.m03 + l1*nRt.m13 + l2*nRt.m23 + m13*nRt.m33;

    l0 = m20; l1 = m21; l2 = m22;
    m20 = l0*nRt.m00 + l1*nRt.m10 + l2*nRt.m20 + m23*nRt.m30;
    m21 = l0*nRt.m01 + l1*nRt.m11 + l2*nRt.m21 + m23*nRt.m31;
    m22 = l0*nRt.m02 + l1*nRt.m12 + l2*nRt.m22 + m23*nRt.m32;
    m23 = l0*nRt.m03 + l1*nRt.m13 + l2*nRt.m23 + m23*nRt.m33;

    l0 = m30; l1 = m31; l2 = m32;
    m30 = l0*nRt.m00 + l1*nRt.m10 + l2*nRt.m20 + m33*nRt.m30;
    m31 = l0*nRt.m01 + l1*nRt.m11 + l2*nRt.m21 + m33*nRt.m31;
    m32 = l0*nRt.m02 + l1*nRt.m12 + l2*nRt.m22 + m33*nRt.m32;
    m33 = l0*nRt.m03 + l1*nRt.m13 + l2*nRt.m23 + m33*nRt.m33;
}


void 
pgMatrix::multVector(pgVec4& dst, const pgVec4& src) const 
{
    dst[0] = src[0]*m00 + src[1]*m10 + src[2]*m20 + src[3]*m30;
    dst[1] = src[0]*m01 + src[1]*m11 + src[2]*m21 + src[3]*m31;
    dst[2] = src[0]*m02 + src[1]*m12 + src[2]*m22 + src[3]*m32;
    dst[3] = src[0]*m03 + src[1]*m13 + src[2]*m23 + src[3]*m33;
}


void 
pgMatrix::multVector(pgVec4& dst, const pgVec3& src) const 
{
    dst[0] = src[0]*m00 + src[1]*m10 + src[2]*m20 + m30;
    dst[1] = src[0]*m01 + src[1]*m11 + src[2]*m21 + m31;
    dst[2] = src[0]*m02 + src[1]*m12 + src[2]*m22 + m32;
    dst[3] = src[0]*m03 + src[1]*m13 + src[2]*m23 + m33;
}


void 
pgMatrix::multVector(pgVec3& dst, const pgVec3& src) const
{
    dst[0] = src[0]*m00 + src[1]*m10 + src[2]*m20 + m30;
    dst[1] = src[0]*m01 + src[1]*m11 + src[2]*m21 + m31;
    dst[2] = src[0]*m02 + src[1]*m12 + src[2]*m22 + m32;
}


pgMatrix&
pgMatrix::operator=(const D3DXMATRIX& nMat)
{
	*getD3DMatrix() = nMat;
	return *this;
}


bool
pgMatrix::operator==(const pgMatrix& nMat) const
{
	return  m00==nMat.m00 && m01==nMat.m01 && m02==nMat.m02 && m03==nMat.m03 &&
			m10==nMat.m10 && m11==nMat.m11 && m12==nMat.m12 && m13==nMat.m13 &&
			m20==nMat.m20 && m21==nMat.m21 && m22==nMat.m22 && m23==nMat.m23 &&
			m30==nMat.m30 && m31==nMat.m31 && m32==nMat.m32 && m33==nMat.m33;
}



