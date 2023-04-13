///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Shared::CGeometry.
 *
 * @file
 *  Geometry.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  27.02.2002
 * @brief
 *  Implementation of \e Shared::CGeometry.
 */



#include "StdAfx.h"
#include "Geometry.h"
#include "math.h"



using namespace Shared;



bool
CGeometry::CreateSphere( float fRadius, unsigned int uiRings, unsigned int uiSections,
                         IDirect3DDevice8* pD3DDev, ID3DXMesh*& pMesh,
                         float fScaleU, float fScaleV )
{
    struct Vertex
    {
        float   fX,
                fY,
                fZ;

        float   fU,
                fV;
    };

    unsigned int uiNumTriangles( uiRings * uiSections * 2 );
    unsigned int uiNumIndices( uiNumTriangles * 3 );
    unsigned int uiNumVertices( ( uiRings + 1 ) * ( uiSections + 1 ) );

    if( FAILED( D3DXCreateMeshFVF( uiNumTriangles,
                                   uiNumVertices,
                                   D3DXMESH_MANAGED | D3DXMESH_WRITEONLY,
                                   D3DFVF_XYZ | D3DFVF_TEX1,
                                   pD3DDev,
                                   &pMesh
                                   ) ) )
    {
        return( false );
    }

    IDirect3DVertexBuffer8* pkVB;
    pMesh->GetVertexBuffer( &pkVB );

    Vertex* pVertices;
    pkVB->Lock( 0, 0, (BYTE **) &pVertices, 0 );

    IDirect3DIndexBuffer8* pkIB;
    pMesh->GetIndexBuffer( &pkIB );

    WORD* pIndices;
    pkIB->Lock( 0, 0, (BYTE **) &pIndices, 0 );


    float fSectionSlice( D3DXToRadian( 360.0f / (float) uiSections ) );
    float fRingSlice( D3DXToRadian( 180.0f / (float) uiRings ) );


    unsigned int uiIndex( 0 );
    for( unsigned int a( 0 ); a <= uiRings; ++a )
    {
        float w = sinf( a * fRingSlice );
        for( unsigned int i( 0 ); i <= uiSections; ++i )
        {
            pVertices[ uiIndex ].fX = fRadius * cosf( i * fSectionSlice ) * w;
            pVertices[ uiIndex ].fY = fRadius * cosf( a * fRingSlice );
            pVertices[ uiIndex ].fZ = fRadius * sinf( i * fSectionSlice ) * w;
            pVertices[ uiIndex ].fU = fScaleV * (float) i / (float) uiSections;
            pVertices[ uiIndex ].fV = fScaleU * (float) a / (float) uiRings;

            ++uiIndex;
        }
    }

    uiIndex = 0;
    for( unsigned int a( 0 ); a < uiRings; ++a )
    {
        for( unsigned int i( 0 ); i < uiSections; ++i )
        {
            pIndices[ uiIndex++ ] = (WORD) ( a * ( uiSections + 1 ) + i );
            pIndices[ uiIndex++ ] = (WORD) ( a * ( uiSections + 1 ) + i + 1 );
            pIndices[ uiIndex++ ] = (WORD) ( ( a + 1 ) * ( uiSections + 1 ) + i + 1 );

            pIndices[ uiIndex++ ] = (WORD) ( ( a + 1 ) * ( uiSections + 1 ) + i + 1 );
            pIndices[ uiIndex++ ] = (WORD) ( ( a + 1 ) * ( uiSections + 1 ) + i );
            pIndices[ uiIndex++ ] = (WORD) ( a * ( uiSections + 1 ) + i );
        }
    }

    pkVB->Unlock();
    pkVB->Release();

    pkIB->Unlock();
    pkIB->Release();

    //// optimize mesh
    //DWORD *pdwAdjInfo = new DWORD[ uiNumTriangles * 3 ];
    //pMesh->GenerateAdjacency( 0.0f, pdwAdjInfo );
    //pMesh->OptimizeInplace(
    //                        D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
    //                        pdwAdjInfo,
    //                        0,
    //                        0,
    //                        0 );
    //delete [] pdwAdjInfo;

    return( true );
}