///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e Effects::CSoftObject.
 *
 * @file
 *  SoftObject.h
 * @author
 *  Carsten Wenzel
 * @date
 *  08.03.2002
 * @brief
 *  Declaration of \e Effects::CSoftObject.
 */



#ifndef _SOFTOBJECT_H_INCLUDED_
#define _SOFTOBJECT_H_INCLUDED_

#pragma once



namespace Effects
{



    /**
    * A class modeling a simple blob.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  08.03.2002
    * @brief
    *  A class modeling a simple blob.
    */
    class CSoftObject
    {
    public:
        enum ENumAnimKeys
        {
            /**
            * Number of animation key.
            */
            NUM_ANIMKEYS = 32 // power of two!
        };

        /**
        * Returns iso value at \e vPos.
        * 
        * @param vPos
        *  Position for which iso value should be calculated.
        * @return
        *  Iso value at \e vPos.
        * @brief
        *  Returns iso value at \e vPos.
        */
        float GetIsoValue( const D3DXVECTOR3& vPos ) const;

        /**
        * Gets normal at \e vPos.
        * 
        * @param vPos
        *  Position for which normal should be calculated.
        * @param vNormal
        *  Reference to a vector receiving the normal at \e vPos.
        * @brief
        *  Gets normal at \e vPos.
        */
        void  GetIsoNormal( const D3DXVECTOR3& vPos, D3DXVECTOR3& vNormal ) const;

        /**
        * Returns current position of blob.
        * 
        * @return
        *  Current position of iso object.
        * @brief
        *  Returns current position of blob.
        */
        const D3DXVECTOR3& GetPosition() const;

        /**
        * Sets position of one animation key.
        * 
        * @param iIndex
        *  Index of animation key. <tt>0 <= iIndex < #NUM_ANIMKEYS</tt>
        * @param vKey
        *  Position.
        * @brief
        *  Sets position of one animation key.
        */
        void SetAnimKey( unsigned int iIndex, const D3DXVECTOR3& vKey );

        /**
        * Updates blob position for given time.
        * 
        * @param fTime
        *  Animation time (in seconds).
        * @param fAttractor
        *  Position attractor. Only to synch effect to music in demo mode.
        * @brief
        *  Updates blob position for given time.
        */
        void UpdatePosition( float fTime, float fAttractor );

    private:
        /** 
        * Array of animaytion keys.
        * 
        * @brief
        *  Array of animaytion keys.
        */
        D3DXVECTOR3 m_pvKeys[ NUM_ANIMKEYS ];

        /** 
        * Current blob position.
        * 
        * @brief
        *  Current blob position.
        */
        D3DXVECTOR3 m_vCurrentPos;
    };



#include "SoftObject.inl"



} // namespace Effects



#endif // #ifndef _SOFTOBJECT_H_INCLUDED_