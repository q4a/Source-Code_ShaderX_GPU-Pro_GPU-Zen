///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Implementation of \e Effects::CSoftObjectsPolygonizer.
 *
 * @file
 *  SoftObjectsPolygonizer.cpp
 * @author
 *  Carsten Wenzel
 * @date
 *  13.03.2002
 * @brief
 *  Implementation of \e Effects::CSoftObjectsPolygonizer.
 */



#include "StdAfx.h"
#include "SoftObjectsPolygonizer.h"
#include "Tables.h"
#include "Random.h"



using namespace Effects;
using namespace Shared;



const float CSoftObjectsPolygonizer::sc_fIsoThresholdDefault( 0.25f );
const unsigned char CSoftObjectsPolygonizer::sc_ucGridSizeDefault( 50 );
const unsigned int CSoftObjectsPolygonizer::sc_uiNumSoftObjectsDefault( 5 );



CSoftObjectsPolygonizer::CSoftObjectsPolygonizer()
: m_pkTriangleContainer( 0 )
, m_ucGridSize( 0 )
, m_fInvGridSize( 0.0f )
, m_fIsoThreshold( sc_fIsoThresholdDefault )
, m_uiNumSoftObjects( sc_uiNumSoftObjectsDefault )
, m_fBlobsAttractor( 1.0f )
, m_colCubesToDo()
, m_colVisitedCubesBits()
{
    SetGridSize( sc_ucGridSizeDefault );
    InitializeSoftObjects();
}



CSoftObjectsPolygonizer::~CSoftObjectsPolygonizer()
{
}



void 
CSoftObjectsPolygonizer::InitializeSoftObjects()
{
    CRandom kRnd;
    for( unsigned int i( 0 ); i < NUM_MAXSOFTOBJECTS; ++i )
    {
        for( unsigned int a( 0 ); a < CSoftObject::NUM_ANIMKEYS; ++a )
        {
            m_pSoftObjects[ i ].SetAnimKey( a, D3DXVECTOR3( (float) kRnd.GetInRange( 0.25, 0.75 ),
                                                            (float) kRnd.GetInRange( 0.25, 0.75 ),
                                                            (float) kRnd.GetInRange( 0.25, 0.75 ) ) );
        }
    }
}



CSoftObjectsPolygonizer::CubeConfig
CSoftObjectsPolygonizer::VisitCube( SCubeIndex sInd )
{
    DBG_ASSERT( 0 != m_pkTriangleContainer );

    // get all grid vertices
    SGridVertex pGridVertex[ 8 ];
    GetGridVertex( pGridVertex[ 0 ], sInd.m_ucX + 0, sInd.m_ucY + 0, sInd.m_ucZ + 0 );
    GetGridVertex( pGridVertex[ 1 ], sInd.m_ucX + 1, sInd.m_ucY + 0, sInd.m_ucZ + 0 );
    GetGridVertex( pGridVertex[ 2 ], sInd.m_ucX + 1, sInd.m_ucY + 1, sInd.m_ucZ + 0 );
    GetGridVertex( pGridVertex[ 3 ], sInd.m_ucX + 0, sInd.m_ucY + 1, sInd.m_ucZ + 0 );
    GetGridVertex( pGridVertex[ 4 ], sInd.m_ucX + 0, sInd.m_ucY + 0, sInd.m_ucZ + 1 );
    GetGridVertex( pGridVertex[ 5 ], sInd.m_ucX + 1, sInd.m_ucY + 0, sInd.m_ucZ + 1 );
    GetGridVertex( pGridVertex[ 6 ], sInd.m_ucX + 1, sInd.m_ucY + 1, sInd.m_ucZ + 1 );
    GetGridVertex( pGridVertex[ 7 ], sInd.m_ucX + 0, sInd.m_ucY + 1, sInd.m_ucZ + 1 );

    // calculate cube configuration
    CubeConfig config( 0 );
    for( unsigned int i( 0 ); i < 8; ++i )
    {
        config |= ( pGridVertex[ i ].fIsoValue > m_fIsoThreshold ) ? ( 1L << i ) : ( 0 );
    }

    // polygonize surface if it's intersecting the cube
    if( 0 != c_piEdgeTable[ config ]  )
    {
        // connectivity of cube vertices
        const int c_piEdgeConnection[ 12 ][ 2 ] =
        {
            { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, 
            { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 }, 
            { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
        };

        m_pkTriangleContainer->Begin( c_piTriTable[ config ][ 0 ] >> 4L, 
                                      c_piTriTable[ config ][ 0 ] & 0x0FL );

        SVertex vertices[ 12 ];
        SVertex* pVertex( &vertices[ 0 ] );
        for( unsigned int i( 0 ); i < 12; ++i )
        {
            // generate a vertex for each edge with its end points 
            // lying on opposite sites of the surface
            if( c_piEdgeTable[ config ] & ( 1 << i ) )
            {
                GetVertex( *pVertex++, 
                           pGridVertex[ c_piEdgeConnection[ i ][ 0 ] ],
                           pGridVertex[ c_piEdgeConnection[ i ][ 1 ] ] );
            }
        }

        m_pkTriangleContainer->AddVertices( &vertices[ 0 ] );
        m_pkTriangleContainer->AddIndices( &c_piTriTable[ config ][ 1 ] );
        m_pkTriangleContainer->End();
    }

    return( config );
}



inline void 
CSoftObjectsPolygonizer::InsertNeighbour( unsigned char ucX, unsigned char ucY, unsigned char ucZ )
{
    // get cube index of neighbour to be inserted to todo list
    SCubeIndex sInd;
    GetCubeIndex( sInd, ucX, ucY, ucZ );

    // determine if neighbour has already been checked
    // that is, it has been visited or at least added to the todo list
    if( false == GetCubeCheckedBit( sInd ) )
    {
        // add cube to list
        m_colCubesToDo.push_back( sInd );
        
        // set cubes "checked" bit
        // now that it's in the list it'll be visited eventually
        // and this keeps us from adding the same cube multiple times
        SetCubeCheckedBit( sInd );
    }
}



void
CSoftObjectsPolygonizer::AddNeighbours( SCubeIndex sInd, CubeConfig config )
{
    // add left neighbour
    // only if surface is going through left side of cube
    if( ( sInd.m_ucX >= 1 ) &&
        ( 0x00 != ( config & 0x99 ) ) && 
        ( 0x99 != ( config & 0x99 ) ) )
    {
        InsertNeighbour( sInd.m_ucX - 1, sInd.m_ucY, sInd.m_ucZ );
    }

    // add right neighbour
    // only if surface is going through right side of cube
    if( ( sInd.m_ucX + 1 < m_ucGridSize ) &&
        ( 0x00 != ( config & 0x66 ) ) && // 
        ( 0x66 != ( config & 0x66 ) ) ) 
    {
        InsertNeighbour( sInd.m_ucX + 1, sInd.m_ucY, sInd.m_ucZ );
    }

    // add back neighbour
    // only if surface is going through right back of cube
    if( ( sInd.m_ucZ >= 1 ) &&
        ( 0x00 != ( config & 0x33 ) ) &&
        ( 0x33 != ( config & 0x33 ) ) )
    {
        InsertNeighbour( sInd.m_ucX, sInd.m_ucY, sInd.m_ucZ - 1 );
    }

    // add front neighbour
    // only if surface is going through front side of cube
    if( ( sInd.m_ucZ + 1 < m_ucGridSize ) &&
        ( 0x00 != ( config & 0xCC ) ) &&
        ( 0xCC != ( config & 0xCC ) ) )
    {
        InsertNeighbour( sInd.m_ucX, sInd.m_ucY, sInd.m_ucZ + 1 );
    }

    // add bottom neighbour
    // only if surface is going through bottom side of cube
    if( ( sInd.m_ucY >= 1 ) &&
        ( 0x00 != ( config & 0x0F ) ) &&
        ( 0x0F != ( config & 0x0F ) ) )
    {
        InsertNeighbour( sInd.m_ucX, sInd.m_ucY - 1, sInd.m_ucZ );
    }

    // add top neighbour
    // only if surface is going through top side of cube
    if( ( sInd.m_ucY + 1 < m_ucGridSize ) &&
        ( 0x00 != ( config & 0xF0 ) ) &&
        ( 0xF0 != ( config & 0xF0 ) ) )
    {
        InsertNeighbour( sInd.m_ucX, sInd.m_ucY + 1, sInd.m_ucZ );
    }
}



void 
CSoftObjectsPolygonizer::RenderSurface( float fTime )
{
    DBG_ASSERT( m_uiNumSoftObjects > 0 && m_uiNumSoftObjects <= NUM_MAXSOFTOBJECTS );

    // update position of soft objects
    for( unsigned int i( 0 ); i < m_uiNumSoftObjects; ++i )
    {
        m_pSoftObjects[ i ].UpdatePosition( fTime, m_fBlobsAttractor );
    }

    // uncheck all cubes
    ClearCubeCheckedBits();

    // polygonize soft objects
    DBG_ASSERT( false != m_colCubesToDo.empty() );
    for( unsigned int i( 0 ); i < m_uiNumSoftObjects; ++i )
    {
        CubeConfig config;

        // get the index of the enclosing cube in which the center 
        // of the soft object is located
        SCubeIndex sInd;
        GetCubeIndex( sInd, m_pSoftObjects[ i ].GetPosition() );
                
        // now since we are inside the soft object 
        // we need to find a way to it's surface by 
        // traceing a line from the center to the surface
        bool bSurfaceFound( false );
        while( false == bSurfaceFound )
        {
            // visit cube and check the resulting parameters
            config = VisitCube( sInd );
            
            // while tracing we should never reach the outside before
            // hitting the surface (else grid size is too low, that is,
            // the soft object is too small and occupies less than one cube
            // - check it's parameters!)
            DBG_ASSERT( config != 0 );

            if( 0xFF == config )
            {
                // we are (still) inside so trace line along x
                GetCubeIndex( sInd, sInd.m_ucX + 1, sInd.m_ucY, sInd.m_ucZ );
            }
            else
            {
                bSurfaceFound = true;
            }            
        }

        // only trace contour of soft object if the cube index we've just found
        // hasn't been visited yet (happens when soft objects penetrate each other)
        if( false == GetCubeCheckedBit( sInd ) )
        {
            // set "checked" bit for initial cube
            SetCubeCheckedBit( sInd );

            // add all its unchecked neighbours to the todo list
            AddNeighbours( sInd, config );

            // visit all remaining cubes 
            while( false == m_colCubesToDo.empty() )
            {
                // get last element is list and remove it
                sInd = m_colCubesToDo.back();
                m_colCubesToDo.pop_back();

                // visit cube
                config = VisitCube( sInd );             

                // add all its unchecked neighbours to the todo list
                if( 0x00 != config && 
                    0xFF != config ) 
                {
                    AddNeighbours( sInd, config );
                }
            }
        }
    }
}
