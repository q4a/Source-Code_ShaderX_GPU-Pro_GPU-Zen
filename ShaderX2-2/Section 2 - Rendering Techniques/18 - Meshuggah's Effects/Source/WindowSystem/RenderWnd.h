///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e WindowSystem::IRenderWnd.
 *
 * @file
 *  RenderWnd.h
 * @author
 *  Carsten Wenzel
 * @date
 *  01.03.2002
 * @brief
 *  Declaration of \e WindowSystem::IRenderWnd.
 */



#ifndef _RENDERWND_H_INCLUDED_
#define _RENDERWND_H_INCLUDED_

#pragma once



namespace WindowSystem
{



    /**
    * A render window interface.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  01.03.2002
    * @brief
    *  A render window interface.
    */
    class IRenderWnd
    {
    public:
        /**
        * Renders a new frame in the window.
        * 
        * @return
        *  \b true if rendering was successful else \b false.
        * @brief
        *  Renders a new frame in the window.
        */
        virtual bool RenderFrame() = 0;
    };



} // namespace WindowSystem



#endif // #ifndef _RENDERWND_H_INCLUDED_