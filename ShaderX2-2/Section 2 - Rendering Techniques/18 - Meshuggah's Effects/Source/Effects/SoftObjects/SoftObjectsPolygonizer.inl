///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CSoftObjectsPolygonizer (inline methods).
 *
 * @file
 *  SoftObjectsPolygonizer.inl
 * @author
 *  Carsten Wenzel
 * @date
 *  13.03.2002
 * @brief
 *  Implementation of \e Effects::CSoftObjectsPolygonizer (inline methods).
 */



#ifndef _SOFTOBJECTSPOLYGONIZER_INL_INCLUDED_
#define _SOFTOBJECTSPOLYGONIZER_INL_INCLUDED_

#pragma once



inline void
CSoftObjectsPolygonizer::GetCubeIndex( SCubeIndex& sCubeIndex, 
                                       unsigned char ucX, 
                                       unsigned char ucY, 
                                       unsigned char ucZ ) const
{
    DBG_ASSERT( ucX < m_ucGridSize );
    DBG_ASSERT( ucY < m_ucGridSize );
    DBG_ASSERT( ucZ < m_ucGridSize );

    sCubeIndex.m_ucX = ucX;
    sCubeIndex.m_ucY = ucY;
    sCubeIndex.m_ucZ = ucZ;
}



inline void
CSoftObjectsPolygonizer::GetCubeIndex( SCubeIndex& sCubeIndex,
                                       const D3DXVECTOR3& vPos ) const
{
    DBG_ASSERT( vPos.x >= 0.0f && vPos.x < 1.0f );
    DBG_ASSERT( vPos.y >= 0.0f && vPos.y < 1.0f );
    DBG_ASSERT( vPos.z >= 0.0f && vPos.z < 1.0f );

    GetCubeIndex( sCubeIndex,
                  (unsigned char) ( m_ucGridSize * vPos.x ),
                  (unsigned char) ( m_ucGridSize * vPos.y ),
                  (unsigned char) ( m_ucGridSize * vPos.z ) );
}



inline void 
CSoftObjectsPolygonizer::SetTriangleContainer( ITriangleContainer* pkTriangleContainer )
{
    m_pkTriangleContainer = pkTriangleContainer;
}



inline float 
CSoftObjectsPolygonizer::IndexToVolumeCoord( unsigned char ucXYZ ) const
{
    DBG_ASSERT( ucXYZ <= m_ucGridSize );
    return( (float) ucXYZ * m_fInvGridSize );
}



inline void
CSoftObjectsPolygonizer::SetGridSize( unsigned char ucGridSize )
{
    // make sure the grid size is less than 255 so that we can use
    // an efficient encoding scheme for cube and vertex indices
    DBG_ASSERTM( 256 > ucGridSize + 1, "Grid size is set too high!" );

    DBG_ASSERTM( 10 <= ucGridSize, "Grid size is set too low!" );

    if( m_ucGridSize != ucGridSize )
    {
        m_ucGridSize = ucGridSize;
        m_fInvGridSize = 1.0f / (float) ucGridSize;

        // reallocate bit array for visited cubes
        VisitedCubesBits temp( 1 + ( ucGridSize * ucGridSize * ucGridSize >> 3 )  );
        m_colVisitedCubesBits.swap( temp );
    }
}



inline void 
CSoftObjectsPolygonizer::SetIsoThreshold( float fIsoThreshold )
{
    m_fIsoThreshold = fIsoThreshold;
}



inline void 
CSoftObjectsPolygonizer::SetNumSoftObjects( unsigned int uiNumSoftObjects )
{
    DBG_ASSERT( uiNumSoftObjects > 0 && uiNumSoftObjects <= NUM_MAXSOFTOBJECTS );
    m_uiNumSoftObjects = uiNumSoftObjects;
}



inline float
CSoftObjectsPolygonizer::CalcIsoFunction( const D3DXVECTOR3& vPosition ) const
{
    float fIsoValue( 0.0f );
    for( unsigned int i( 0 ); i < m_uiNumSoftObjects; ++i )
    {
        fIsoValue += m_pSoftObjects[ i ].GetIsoValue( vPosition );
    }

    return( fIsoValue );
}



inline void
CSoftObjectsPolygonizer::CalcIsoNormal( D3DXVECTOR3& vNormal, const D3DXVECTOR3& vPosition ) const
{
    vNormal.x = vNormal.y = vNormal.z = 0;
    for( unsigned int i( 0 ); i < m_uiNumSoftObjects; ++i )
    {
        D3DXVECTOR3 vTemp;
        m_pSoftObjects[ i ].GetIsoNormal( vPosition, vTemp );
        vNormal += vTemp;
    }
}



inline void
CSoftObjectsPolygonizer::GetGridVertex( SGridVertex& sGridVertex, 
                                        unsigned char ucX, 
                                        unsigned char ucY, 
                                        unsigned char ucZ )
{
    sGridVertex.vPos.x = IndexToVolumeCoord( ucX );
    sGridVertex.vPos.y = IndexToVolumeCoord( ucY );
    sGridVertex.vPos.z = IndexToVolumeCoord( ucZ );

    // calculate the iso value
    sGridVertex.fIsoValue = CalcIsoFunction( sGridVertex.vPos );
}



inline void 
CSoftObjectsPolygonizer::GetVertex( SVertex& sEdgeVertex, 
                                    const SGridVertex& sGridVertexA, 
                                    const SGridVertex& sGridVertexB )
{
    // linear interpolate the position of the edge vertex
    float fLerp( ( m_fIsoThreshold - sGridVertexA.fIsoValue ) / 
                  ( sGridVertexB.fIsoValue - sGridVertexA.fIsoValue ) );

    D3DXVec3Lerp( &sEdgeVertex.vPos, &sGridVertexA.vPos, &sGridVertexB.vPos, fLerp );

    // Calculate the surface normal (not normalized!).
    // This version is faster than the code below
    // since we know the soft objects iso formula which
    // allows us to directly calculate the partial derivatives.
    CalcIsoNormal( sEdgeVertex.vNormal, sEdgeVertex.vPos );

    // get iso value for edge vertex
    float fIsoValue( CalcIsoFunction( sEdgeVertex.vPos ) );

    //// calculate the surface normal (not normalized!)
    //const float c_fEpsilon( 1e-4f );
    //sEdgeVertex.vNormal.x = fIsoValue - 
    //    CalcIsoFunction( D3DXVECTOR3( sEdgeVertex.vPos.x + c_fEpsilon, sEdgeVertex.vPos.y, sEdgeVertex.vPos.z ) );
    //sEdgeVertex.vNormal.y = fIsoValue - 
    //    CalcIsoFunction( D3DXVECTOR3( sEdgeVertex.vPos.x, sEdgeVertex.vPos.y + c_fEpsilon, sEdgeVertex.vPos.z ) );
    //sEdgeVertex.vNormal.z = fIsoValue - 
    //    CalcIsoFunction( D3DXVECTOR3( sEdgeVertex.vPos.x, sEdgeVertex.vPos.y, sEdgeVertex.vPos.z + c_fEpsilon ) );
}



inline void 
CSoftObjectsPolygonizer::SetCubeCheckedBit( const SCubeIndex& sCubeIndex )
{
    unsigned int 
        uiCubeBitAddr( ( ( sCubeIndex.m_ucZ * m_ucGridSize ) + sCubeIndex.m_ucY ) * m_ucGridSize + sCubeIndex.m_ucX );

    m_colVisitedCubesBits[ uiCubeBitAddr >> 3 ] |= 1 << ( uiCubeBitAddr & 0x7L );
}



inline bool 
CSoftObjectsPolygonizer::GetCubeCheckedBit( const SCubeIndex& sCubeIndex )
{
    unsigned int 
        uiCubeBitAddr( ( ( sCubeIndex.m_ucZ * m_ucGridSize ) + sCubeIndex.m_ucY ) * m_ucGridSize + sCubeIndex.m_ucX );

    return( ( m_colVisitedCubesBits[ uiCubeBitAddr >> 3 ] & 
            ( 1 << ( uiCubeBitAddr & 0x7L ) ) ) ? true : false );
}



inline void 
CSoftObjectsPolygonizer::ClearCubeCheckedBits()
{
    memset( &m_colVisitedCubesBits[ 0 ], 0, m_colVisitedCubesBits.size() );
}



inline void
CSoftObjectsPolygonizer::SetBlobsAttractor( float fBlobsAttractor )
{
    m_fBlobsAttractor = fBlobsAttractor;
}



#endif // #ifndef _SOFTOBJECTSPOLYGONIZER_INL_INCLUDED_