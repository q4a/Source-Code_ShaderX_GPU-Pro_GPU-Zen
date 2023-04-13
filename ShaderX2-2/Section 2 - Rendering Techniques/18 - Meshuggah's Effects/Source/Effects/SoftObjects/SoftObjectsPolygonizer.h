///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Effects::CSoftObjectsPolygonizer.
 *
 * @file
 *  SoftObjectsPolygonizer.h
 * @author
 *  Carsten Wenzel
 * @date
 *  13.03.2002
 * @brief
 *  Declaration of \e Effects::CSoftObjectsPolygonizer.
 */



#ifndef _SOFTOBJECTSPOLYGONIZER_H_INCLUDED_
#define _SOFTOBJECTSPOLYGONIZER_H_INCLUDED_

#pragma once


#include "SoftObject.h"
#include <vector>



namespace Effects
{



    /**
    * A soft object polygonizer class.
    * Uses a specialized version of the marching cubes algorithm to polygonize
    * the iso field of implicit functions (blobs) for a given threshold value.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  13.03.2002
    * @brief
    *  A soft object polygonizer class.
    */
    class CSoftObjectsPolygonizer
    {
    public:
        /** 
        * Structure of a soft object mesh vertex.
        * 
        * @brief
        *  Structure of a soft object mesh vertex.
        */
        struct SVertex
        {
            /** 
            * Position.
            * 
            * @brief
            *  Position.
            */
            D3DXVECTOR3 vPos;

            /** 
            * Normal.
            * 
            * @brief
            *  Normal.
            */
            D3DXVECTOR3 vNormal;
        };

        /**
        * Interface definition of a triangle container which receives all generated vertex and index data. 
        * Keeps the polygonizer independent from any 3D API.
        *
        * @author
        *  Carsten Wenzel
        * @date
        *  13.03.2002
        * @brief
        *  Interface definition of a triangle container.
        */
        class ITriangleContainer
        {
        public:
            /**
            * Begins a new vertex/index block.
            * 
            * @param uiVerticesToAdd
            *  Number of vertices to be added fro this block.
            * @param uiIndicesToAdd
            *  Number of indices to be added fro this block.
            * @brief
            *  Begins a new vertex/index block.
            */
            virtual void Begin( unsigned int uiVerticesToAdd, unsigned int uiIndicesToAdd ) = 0;

            /**
            * Adds vertices. The number of vertices stored in \e pVertices \b must match with the one specified in Begin()!
            * 
            * @param pVertices
            *  Pointer to a vertex array containing generated vertex data.
            * @brief
            *  Adds vertices.
            */
            virtual void AddVertices( const SVertex* const pVertices ) = 0;

            /**
            * Adds indices. The number of indices stored in \e pIndices \b must match with the one specified in Begin()!
            * 
            * @param pIndices
            *  Pointer to a vertex array containing generated index data.
            * @brief
            *  Adds indices.
            */
            virtual void AddIndices( const int* const pIndices ) = 0;

            /**
            * Ends vertex/index block.
            * 
            * @brief
            *  Ends vertex/index block.
            */
            virtual void End() = 0;
        };

    public:
        /** 
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
        CSoftObjectsPolygonizer();
        
        /** 
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        ~CSoftObjectsPolygonizer();

        /**
        * Sets the container that should receive the generated vertex/index data.
        * 
        * @param pkTriangleContainer
        *  Pointer to a \e ITriangleContainer interface.
        * @brief
        *  Sets the container that should receive the generated vertex/index data.
        */
        void SetTriangleContainer( ITriangleContainer* pkTriangleContainer );

        /**
        * Sets new grid size, i.e. the number of cubes in each dimension.
        * Set this to less than 255!
        * 
        * @param ucGridSize
        *  New value for grid size.
        * @brief
        *  Sets new grid size.
        */
        void SetGridSize( unsigned char ucGridSize );

        /**
        * Sets new iso threshold value.
        * 
        * @param fIsoThreshold
        *  New value for iso threshold.
        * @brief
        * Sets new iso threshold value.
        */
        void SetIsoThreshold( float fIsoThreshold );

        /**
        * Sets new number of soft objects used to generate the iso field.
        * 
        * @param uiNumSoftObjects
        *  New number of soft objects.
        * @brief
        * Sets new number of soft objects.
        */
        void SetNumSoftObjects( unsigned int uiNumSoftObjects );

        /**
        * Sets new <em>Blobs attractor</em> value.
        * Used to synch motion of soft objects to the music in demo mode.
        *
        * @param fBlobsAttractor
        *  New <em>Blobs attractor</em> value.
        * @brief
        * Sets new <em>Blobs attractor</em> value.
        */
        void SetBlobsAttractor( float fBlobsAttractor );

        /**
        * Renders the surface of all soft objects at given time.
        * 
        * @param fTime
        *  Time. Used to animate the blobs before generating the iso field.
        * @brief
        *  Renders the surface of all soft objects at given time.
        */
        void RenderSurface( float fTime );

    public:
        /** 
        * Default iso threshold value.
        * 
        * @brief
        *  Default iso threshold value.
        */        
        static const float sc_fIsoThresholdDefault;

        /** 
        * Default grid size.
        * 
        * @brief
        *  Default grid size.
        */        
        static const unsigned char sc_ucGridSizeDefault;

        /** 
        * Default number of soft objects.
        * 
        * @brief
        *  Default number of soft objects.
        */        
        static const unsigned int sc_uiNumSoftObjectsDefault;

    private:
        enum ENumMaxSoftObjects
        {
            /** 
            * Maximum number of soft objects.
            * 
            * @brief
            *  Maximum number of soft objects.
            */
            NUM_MAXSOFTOBJECTS = 10
        };

        /** 
        * Structure of a grid vertex in the iso field.
        * 
        * @brief
        *  Structure of a grid vertex in the iso field.
        */
        struct SGridVertex
        {
            /** 
            * Position.
            * 
            * @brief
            *  Position.
            */
            D3DXVECTOR3 vPos;

            /** 
            * Iso value at \e vPos.
            * 
            * @brief
            *  Iso value at \e vPos.
            */
            float fIsoValue;
        };

        /** 
        * Structure of a cube index.
        * 
        * @brief
        *  Structure of a cube index.
        */
        struct SCubeIndex
        {
            /** 
            * x index.
            * 
            * @brief
            *  x index.
            */
            unsigned char m_ucX;

            /** 
            * y index.
            * 
            * @brief
            *  y index.
            */
            unsigned char m_ucY;

            /** 
            * z index.
            * 
            * @brief
            *  z index.
            */
            unsigned char m_ucZ;
        };

        /** 
        * Type storing a cube configuration.
        * 
        * @brief
        *  Type storing a cube configuration.
        */
        typedef unsigned char CubeConfig;        

        /** 
        * Type storing the indices of <em>to be visited</em> cube.
        * 
        * @brief
        *  Type storing the indices of <em>to be visited</em> cube.
        */
        typedef std::vector< SCubeIndex > ToDoList;

        /** 
        * Type storing a set of bits. 
        * That is, there's one bit for each cube in the iso field indicating
        * whether the cube has already been visited.
        *
        * @brief
        *  Type storing a set of bits.
        */
        typedef std::vector< unsigned char > VisitedCubesBits;

    private:
        /** 
        * Initializes all soft objetcs.
        * 
        * @brief
        *  Initializes all soft objetcs.
        */
        void InitializeSoftObjects();
       
        /**
        * Sets the <em>Cube visited</em> in #m_colVisitedCubesBits bit for a given cube index.
        * 
        * @param sCubeIndex
        *  Index of cube that should be marked as visited.
        * @brief
        *  Sets the <em>Cube visited</em> in #m_colVisitedCubesBits bit for a given cube index.
        */
        void SetCubeCheckedBit( const SCubeIndex& sCubeIndex );

        /**
        * Checks if a cube has already been visited.
        * 
        * @param sCubeIndex
        *  Index of cube that should be checked.
        * @return
        *  \b true if cube has been visted else \b false.
        * @brief
        *  Checks if a cube has already been visited.
        */
        bool GetCubeCheckedBit( const SCubeIndex& sCubeIndex );

        /**
        * Clears the <em>Cube visited</em> bit field.
        * 
        * @brief
        *  Clears the <em>Cube visited</em> bit field.
        */
        void ClearCubeCheckedBits();

        /**
        * Calculates cube index.
        * 
        * @param sCubeIndex
        *  Reference to a structure that should receive the calculated cube index.
        * @param ucX
        *  x index of cube in volume.
        * @param ucY
        *  y index of cube in volume.
        * @param ucZ
        *  z index of cube in volume.
        * @brief
        *  Calculates cube index.
        */
        void GetCubeIndex( SCubeIndex& sCubeIndex, unsigned char ucX, unsigned char ucY, unsigned char ucZ ) const;

        /**
        * Calculates index of the cube surrounding \e vPos.
        * 
        * @param sCubeIndex
        *  Reference to a structure that should receive the calculated cube index.
        * @param vPos
        * Position in volume.
        * @brief
        *  Calculates index of the cube surrounding \e vPos.
        */
        void GetCubeIndex( SCubeIndex& sCubeIndex, const D3DXVECTOR3& vPos ) const;

        /**
        * Maps the x, y or z index of a grid vertex to a volume coordinate.
        * 
        * @param ucXYZ
        *  x, y or z index of grid vertex in volume.
        * @return
        *  x, y or z position of grid vertex in the volume.
        * @brief
        *  Maps the x, y or z index of a grid vertex to a volume coordinate.
        */
        float IndexToVolumeCoord( unsigned char ucXYZ ) const;

        /**
        * Calculates iso value at \e vPosition.
        * 
        * @param vPosition
        *  Position for which iso value should be calculated.
        * @return
        *  Iso value at \e vPosition.
        * @brief
        *  Calculates iso value at \e vPosition.
        */
        float CalcIsoFunction( const D3DXVECTOR3& vPosition ) const;

        /**
        * Calculates iso normal at \e vPosition.
        * 
        * @param vNormal
        *  Reference to a vector that should receive the iso normal at \e vPosition.
        * @param vPosition
        *  Position for which iso value should be calculated.
        * @return
        *  Iso value at \e vPosition.
        * @brief
        *  Calculates iso normal at \e vPosition.
        */
        void CalcIsoNormal( D3DXVECTOR3& vNormal, const D3DXVECTOR3& vPosition ) const;

        /**
        * Calculates the grid vertex for a given index.
        * 
        * @param sGridVertex
        *  Reference to a structure that should receive the calculated grid vertex.
        * @param ucX
        *  x index of vertex in volume grid.
        * @param ucY
        *  y index of vertex in volume grid.
        * @param ucZ
        *  z index of vertex in volume grid.
        * @brief
        *  Calculates the grid vertex for a given index.
        */
        void GetGridVertex( SGridVertex& sGridVertex, unsigned char ucX, unsigned char ucY, unsigned char ucZ );

        /**
        * Calculates a vertex on the iso surface.
        * 
        * @param sVertex
        *  Reference to a structure that should receive the iso vertex.
        * @param sGridVertexA
        *  Grid vertex on either side of the iso surface.
        * @param sGridVertexB
        *  Grid vertex on the opposite side of the iso surface.
        * @brief
        *  Calculates a vertex on the iso surface.
        */
        void GetVertex( SVertex& sVertex, const SGridVertex& sGridVertexA, const SGridVertex& sGridVertexB );

        /**
        * Inserts a neigbour of a cube to the todo list. 
        * 
        * @param ucX
        *  x index of cube in volume.
        * @param ucY
        *  y index of cube in volume.
        * @param ucZ
        *  z index of cube in volume.
        * @brief
        *  Inserts a neigbour of a cube to the todo list.
        */
        void InsertNeighbour( unsigned char ucX, unsigned char ucY, unsigned char ucZ );

        /**
        * Checks the cube configuration and inserts all relevant neighbours to the todo list.
        * 
        * @param sInd
        *  Index of cube that's currently being processed.
        * @param config
        *  Cube's configuration.
        * @brief
        *  Checks the cube configuration and inserts all relevant neighbours to the todo list.
        */
        void AddNeighbours( SCubeIndex sInd, CubeConfig config );

        /**
        * Visits a cube and performs the marching cubes algorithms.
        * 
        * @param sInd
        *  Index of cube to be visited.
        * @return
        *  Cube configuration for the visted cube.
        * @brief
        *  Visits a cube and performs the marching cubes algorithms.
        */
        CubeConfig VisitCube( SCubeIndex sInd );

    private:
        /** 
        * Pointer to an interface of a triangle container.
        * 
        * @brief
        *  Pointer to an interface of a triangle container.
        */
        ITriangleContainer* m_pkTriangleContainer;

        /** 
        * Grid size.
        * 
        * @brief
        *  Grid size.
        */
        unsigned char m_ucGridSize;

        /** 
        * Inverse grid size.
        * 
        * @brief
        *  Inverse grid size.
        */
        float m_fInvGridSize;

        /** 
        * Iso threshold value.
        * 
        * @brief
        *  Iso threshold value.
        */
        float m_fIsoThreshold;

        /** 
        * Number of soft objects used for rendering.
        * 
        * @brief
        *  Number of soft objects used for rendering.
        */
        unsigned int m_uiNumSoftObjects;

        /** 
        * <em>Blobs attractor</em> value.
        * Used to synch motion of soft objects to the music in demo mode.
        * 
        * @brief
        *  <em>Blobs attractor</em> value.
        */
        float m_fBlobsAttractor;

        /** 
        * List of <em>to be visited</em> cubes.
        * 
        * @brief
        *  List of <em>to be visited</em> cubes.
        */
        ToDoList m_colCubesToDo;

        /** 
        * Bit set for visited cubes.
        * 
        * @brief
        *  Bit set for visited cubes.
        */
        VisitedCubesBits m_colVisitedCubesBits;

        /** 
        * Array of soft objects.
        * 
        * @brief
        *  Array of soft objects.
        */
        CSoftObject m_pSoftObjects[ NUM_MAXSOFTOBJECTS ];
    };



#include "SoftObjectsPolygonizer.inl"



} // namespace Effects



#endif // #ifndef _SOFTOBJECTSPOLYGONIZER_H_INCLUDED_