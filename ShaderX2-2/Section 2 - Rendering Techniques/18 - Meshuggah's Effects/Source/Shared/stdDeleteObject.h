///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration and implementation of \e Shared::TDeleteObject.
 *
 * @file
 *  stdDeleteObject.h
 * @author
 *  Carsten Wenzel
 * @date
 *  28.02.2002
 * @brief
 *  Declaration and implementation of \e Shared::TDeleteObject.
 */



#ifndef _STDDELETEOBJECT_H_INCLUDED_
#define _STDDELETEOBJECT_H_INCLUDED_

#pragma once



namespace Shared
{



    /**
    * A structure that can be used in conjuction with stl's \c for_each algorithm
    * to free entries of containers <tt>container< type* ></tt>.
    *
    * @author
    *  Carsten Wenzel
    * @date
    *  28.02.2002
    * @brief
    *  A structure that can be used in conjuction with stl's \c for_each algorithm
    *  to free entries of containers <tt>container< type* ></tt>.
    */
    struct TDeleteObject
    {
        /**
        * Frees the memory of a given instance of type \e T.
        * 
        * @brief
        *  Frees the memory of a given instance of type \e T.
        */
        template< typename T > 
        void operator() ( const T* pkObj ) const
        {
            delete pkObj;
        }
    };



} // namespace std



#endif // #ifndef _STDDELETEOBJECT_H_INCLUDED_