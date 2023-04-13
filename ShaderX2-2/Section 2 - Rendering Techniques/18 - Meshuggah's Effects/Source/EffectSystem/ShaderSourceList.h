///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e EffectSystem::SShaderSourceList.
 *
 * @file
 *  ShaderSourceList.h
 * @author
 *  Carsten Wenzel
 * @date
 *  27.11.2001
 * @brief
 *  Declaration of \e EffectSystem::SShaderSourceList.
 */



#ifndef _SHADERSOURCELIST_H_INCLUDED_
#define _SHADERSOURCELIST_H_INCLUDED_

#pragma once


#include <string>
#include <list>



namespace EffectSystem
{



    /**
    * Structure that holds the descriptor for one shader. This structure is used in 
    * EffectSystem::CEffect::LoadShaderSources() to fill an EffectSystem::SShaderSourceList structure
    * containing all shaders for one effect.
    *
    * @author
    *  Carsten Wenzel
    * @brief
    *  Structure that holds the descriptor for one shader.
    */
    struct SShaderDescriptor
    {
        /** 
        * Name of the shader.
        * 
        * @brief
        *  Name of the shader.
        */
        char* m_pcShaderName;
        
        /** 
        * Path to the file containing the vertex shader source code.
        * 
        * @brief
        *  Path to the file containing the vertex shader source code.
        */
        char* m_pcVSFilePath;

        /** 
        * Path to the file containing the pixel shader source code.
        * 
        * @brief
        *  Path to the file containing the pixel shader source code.
        */
        char* m_pcPSFilePath;
    };



    /**
    * Structure that holds the name and sources of an arbitrary number of shaders.
    *
    * @author
    *  Carsten Wenzel
    * @brief
    *  Structure that holds the name and sources of an arbitrary number of shaders.
    */
    struct SShaderSourceList
    {



        /**
        * Structure that holds the name, vertex and pixel shader sources for one shader.
        *
        * @author
        *  Carsten Wenzel
        * @brief
        *  Structure that holds the name, vertex and pixel shader sources for one shader.
        */
        struct SShaderSource
        {
            /**
            * Constructor.
            * 
            * @brief
            *  Constructor.
            * @param strShaderName
            *  String that contains the shader name.
            * @param strVertexCode
            *  String that contains the vertex shader source.
            * @param strPixelCode
            *  String that contains the pixel shader source.
            */
            SShaderSource( const std::string& strShaderName,
                           const std::string& strVertexCode,
                           const std::string& strPixelCode );

            /** 
            * String that contains the shader name.
            * 
            * @brief
            *  String that contains the shader name.
            */
            std::string m_strShaderName;

            /** 
            * String that contains the vertex shader source.
            * 
            * @brief
            *  String that contains the vertex shader source.
            */
            std::string m_strVertexCode;

            /** 
            * String that contains the pixel shader source.
            * 
            * @brief
            *  String that contains the pixel shader source.
            */
            std::string m_strPixelCode;
        };

        /** 
        * Defines a list of shaders.
        * 
        * @brief
        *  Defines a list of shaders.
        */
        typedef std::list< SShaderSource > ShaderSourceList;

        /** 
        * A list containing an arbitrary number of shaders.
        * 
        * @brief
        *  A list containing an arbitrary number of shaders.
        */
        ShaderSourceList m_colEntries;
    };



#include "ShaderSourceList.inl"



} // namespace EffectSystem


#endif // #ifndef _SHADERSOURCELIST_H_INCLUDED_