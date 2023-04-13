///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e EffectSystem::CEffect.
 * 
 * @file
 *  Effect.h
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Declaration of \e EffectSystem::CEffect.
 */



#ifndef _EFFECT_H_INCLUDED_
#define _EFFECT_H_INCLUDED_

#pragma once



namespace ScrambledArchive
{
    class CScrambledArchive;
}

namespace Shared
{
    class CTextureManager;
}



namespace EffectSystem
{



    struct SShaderSourceList;
    struct SShaderDescriptor;
    
    class  CEffectTransition;



    /** 
    * IDs for mouse button states.
    * 
    * @brief
    *  IDs for mouse button states.
    */
    enum EButtonID
    {
        /** 
        * Left mouse button down.
        */
        MOUSE_LEFTBUTTON_DOWN,

        /** 
        * Left mouse button up.
        */
        MOUSE_LEFTBUTTON_UP,

        /** 
        * Right mouse button down.
        */
        MOUSE_RIGHTBUTTON_DOWN,

        /** 
        * Right mouse button up.
        */
        MOUSE_RIGHTBUTTON_UP,

        /** 
        * Middle mouse button down.
        */
        MOUSE_MIDDLEBUTTON_DOWN,

        /** 
        * Middle mouse button up.
        */
        MOUSE_MIDDLEBUTTON_UP
    };



    /**
    * Effect interface. This interface contains all methods to show an effect in 
    * Meshuggah's browser mode. It also contains methods to free and re-initialize
    * application managed resources in case of a necessary D3D device reset. 
    * 
    * @author
    *  Carsten Wenzel
    * @date
    *  11.10.2001
    * @brief
    *  Effect interface.
    */
    class IEffect
    {
    public:
        /**
        * Initializes all application managed resources. This method is called after
        * the rendering device was reset in order to recreate dynamic resources
        * (e.g. dynamic vertex buffers, dynamic textures or custom render targets).
        * 
        * @throw EffectSystem::CEffectException
        *  Initialization failed.
        * @brief
        *  Initializes all application managed resources.
        */
        virtual void InitAppManagedResources() = 0;

        /**
        * Frees all application managed resources. This method is called before
        * the rendering device is reset in order to properly release dynamic resources 
        * (e.g. dynamic vertex buffers, dynamic textures or custom render targets).
        * 
        * @brief
        *  Frees all application managed resources.
        */
        virtual void FreeAppManagedResources() = 0;

        /**
        * Renders a new frame.
        * 
        * @brief
        *  Renders a new frame.
        * @param fTime
        *  Current absolute time (it gets converted to effect relative time internally). 
        *  Use default value when running the effect in browser mode.
        */
        virtual void Update( float fTime = -1.0f ) = 0;

        /**
        * Message handler for mouse move events within the render window.
        * 
        * @brief
        *  Message handler for mouse move events within the render window.
        * @param uiMouseX
        *  X coordinate of new mouse position.
        * @param MouseY
        *  Y coordinate of new mouse position.
        */
        virtual void OnMouseMove( int uiMouseX, int MouseY ) = 0;

        /**
        * Message handler for mouse button events within the render window.
        * 
        * @brief
        *  Message handler for mouse button events within the render window.
        * @param eButton
        *  Identifies the button that was just pressed or released.
        * @param uiMouseX
        *  X coordinate of new mouse position.
        * @param MouseY
        *  Y coordinate of new mouse position.
        */
        virtual void OnMouseButton( EButtonID eButton, int uiMouseX, int MouseY ) = 0;

        /**
        * Message handler for keyboard events within the render window.
        * 
        * @brief
        *  Message handler for keyboard events within the render window.
        * @param uiKey
        *  Identifies the key that was pressed/released.
        * @param uiFlags
        *  Informs about the state of shift, caps lock and ctrl.
        * @param bDown
        *  Tells if key was just pressed or released.
        */
        virtual void OnKey( unsigned int uiKey, unsigned int uiFlags, bool bDown ) = 0;

        /**
        * Loads the sources of all shaders of this effect.
        * 
        * @brief
        *  Loads the sources of all shaders of this effect.
        * @param colShaders
        *  Structure that will contain the sources of all shaders of this effect if the method 
        *  returns successfully to the caller.
        * @return
        *  \b true if successful, \b false if shader code could not be extracted from data archive.
        */
        virtual bool GetShaderSources( SShaderSourceList& colShaders ) = 0;

        /**
        * Tells if the effect settings dialog is visible. For use in Meshuggah's browser mode only!
        * 
        * @return
        *  \b true if effect settings dialog is visible else \b false.
        * @brief
        *  Tells if the effect settings dialog is visible.
        */
        virtual bool IsEffectDialogVisible() = 0;

        /**
        * Shows or hides the effect settings dialog. For use in Meshuggah's browser mode only!
        * 
        * @param bShow
        *  Set to \b true if dialog window should be visible. Set to \b false to hide it.
        * @brief
        *  Shows or hides the effect settings dialog.
        */
        virtual void ShowEffectDialog( bool bShow ) = 0;

        /**
        * Destroys the effect. All app managed and effect specific resources will be released. 
        * Static textures loaded into the texture manager will remain.
        * 
        * @brief
        *  Destroys the effect.
        */
        virtual void Destroy() = 0;

        /**
        * Returns the HTML help context ID of the effect.
        * 
        * @return
        *  ID to use when calling \c HtmlHelp() to get effect specific information.
        * @brief
        *  Returns the HTML help context ID of the effect.
        */
        virtual unsigned int GetHTMLHelpID() = 0;
    };



    /**
    * Abstract base class for all effects. Inherits from IEffect interface. Contains
    * additional methods to register and show an effect in a sequencer (e.g. using 
    * EffectSystem::CEffectSequencer in Meshuggah's demo mode).
    * 
    * @author
    *  Carsten Wenzel
    * @date
    *  11.10.2001
    * @brief
    *  Base class for all effects.
    */
    class CEffect : public IEffect
    {
    public:        
        virtual void Destroy();    

        virtual void OnMouseMove( int uiMouseX, int MouseY );

        virtual void OnMouseButton( EButtonID eButton, int uiMouseX, int MouseY );

        virtual void OnKey( unsigned int uiKey, unsigned int uiFlags, bool bDown );

    public:
        /**
        * Constructor.
        * 
        * @brief
        *  Constructor.
        * @param pD3DDev
        *  Pointer to a D3D device interface.
        * @param pkScrambledArchive
        *  Pointer to a data archive. Enables the effect to load additional data.
        * @param pkTextureManager
        *  Pointer to a texture manager. Enables the effect to load/get static textures.
        * @param pkEffectTransition
        *  Pointer to an effect transition manager.
        * @param bBrowserMode
        *  If set to \b true then an effect parameters dialog will be created.
        */
        CEffect( IDirect3DDevice8* pD3DDev, 
                 ScrambledArchive::CScrambledArchive* pkScrambledArchive,
                 Shared::CTextureManager* pkTextureManager,
                 CEffectTransition* pkEffectTransition,
                 bool bBrowserMode );

        /**
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CEffect() = 0;

        /**
        * Returns boolean that indicates if effect is already registered or not.
        * 
        * @brief
        *  Returns boolean that indicates if effect is already registered or not.
        * @return
        *  #m_bRegistered.
        */
        bool IsRegistered() const;

        /**
        * Registers the effect for use in an effect sequencer.
        * 
        * @brief
        *  Registers the effect for use in an effect sequencer.
        * @param fEffectStart
        *  Time when the effect should start.
        * @param fEffectRuntime
        *  Total runtime of the effect.
        * @see
        *  EffectSystem::CEffectSequencer.
        * @return
        *  \b true if effect was registered successfully, else \b false.
        */
        bool Register( float fEffectStart, float fEffectRuntime );

        /**
        * Returns the time (in seconds) when the effect starts.
        * 
        * @brief
        *  Returns the time (in seconds) when the effect starts.
        * @return
        *  #m_fEffectStart.
        */
        float GetEffectStart() const;

        /**
        * Returns the time (in seconds) when the effect ends.
        * 
        * @brief
        *  Returns the time (in seconds) when the effect ends.
        * @return
        *  #m_fEffectEnd.
        */
        float GetEffectEnd() const;

        /**
        * Returns the total runtime (in seconds) of the effect.
        * 
        * @brief
        *  Returns the total runtime (in seconds) of the effect.
        * @return
        *  CEffect::GetEffectEnd() - CEffect::GetEffectStart()
        */
        float GetEffectRuntime() const;

    protected:
        /**
        * Checks if the D3D device pointed to by #m_pD3DDev has all necessary 
        * device capabilities in order to sucessfully run the effect.
        * 
        * @throw EffectSystem::CEffectException
        *  Device is incompatible.
        * @brief
        *  Checks if the D3D device pointed to by #m_pD3DDev has all necessary 
        *  device capabilities in order to sucessfully run the effect.
        */
        virtual void ConfirmRequirements() = 0;

        /**
        * Frees all resources including application managed resources consumed by the effect. 
        * Static textures loaded into the texture manager will remain untouched. The base
        * class implementation calls EffectSystem::CEffect::FreeAppManagedResources().
        *
        * @brief
        *  Frees all effect resources including application managed resources. 
        */
        virtual void FreeAllResources();

        /**
        * Returns the aspect ratio of the current viewport of the D3D device.
        * 
        * @brief
        *  Returns the aspect ratio of the current viewport of the D3D device.
        * @return
        *  Returns viewport width by height.
        */
        float GetAspectRatio();

        /**
        * Calculates the world-view-projection matrix using #m_matWorld, 
        * #m_matView and #m_matProj.
        * 
        * @brief
        *  Calculates the world-view-projection matrix.        
        */
        void GetWorldViewProjection( D3DXMATRIX& matWorldViewProj );

        /**
        * Checks if #m_pD3DDev supports a certain pixel shader version.
        * 
        * @brief
        *  Checks if #m_pD3DDev supports a certain pixel shader version.
        * @param uiPSMajor
        *  Major version number of pixel shader.
        * @param uiPSMinor
        *  Minor version number of pixel shader.
        * @return
        *  \b true if device supports pixel shader version \c uiPSMajor.uiPSMinor. else \b false.
        */
        bool CheckPixelShaderCap( unsigned int uiPSMajor, unsigned int uiPSMinor );

        /**
        * Checks if #m_pD3DDev supports a certain vertex shader version.
        * 
        * @brief
        *  Checks if #m_pD3DDev supports a certain vertex shader version.
        * @param uiVSMajor
        *  Major version number of vertex shader.
        * @param uiVSMinor
        *  Minor version number of vertex shader.
        * @return
        *  \b true if device supports vertex shader version \c uiVSMajor.uiVSMinor. else \b false.
        */
        bool CheckVertexShaderCap( unsigned int uiVSMajor, unsigned int uiVSMinor );

        /**
        * Checks whether a texture format is supported by #m_pD3DDev.
        * 
        * @brief
        *  Checks whether a texture format is supported by #m_pD3DDev.
        * @param eTextureFormat
        *  Requested texture format (see CheckDeviceFormat in DirectX SDK docs).
        * @param eResourceType
        *  Resource type of texture (see CheckDeviceFormat in DirectX SDK docs).
        * @param dwUsage
        *  Usage for texture (see CheckDeviceFormat in DirectX SDK docs).
        * @return
        *  \b true if a texture of format \e eTextureFormat and resource type \e eResourceType
        *  can be created for usage \e dwUsage else \b false.
        */
        bool IsTextureFormatSupported( D3DFORMAT eTextureFormat, D3DRESOURCETYPE eResourceType, DWORD dwUsage = 0 );

        /**
        * Calculates the effect relative time of \e fAbsoluteTime.
        * 
        * @brief
        *  Calculates the effect relative time of \e fAbsoluteTime.
        * @param fAbsoluteTime
        *  Absolute time to convert.
        * @return
        *  Effect relative time.
        */
        float GetRelativeTime( float fAbsoluteTime ) const;

        /**
        * Safely releases a given interface.
        * 
        * @param pkInterface
        *  Reference to a COM interface pointer.
        * @brief
        *  Safely releases a given interface.
        */
        template< typename T >
        void SafeRelease( T*& pkInterface ) const;

        /** 
        * Helper function that get the prefered vertes shader usage flag. Use this when
        * creating vertex shaders to conform to DirectX specifications (according to the SDK).
        * 
        * @brief
        *  Helper function that get the prefered vertes shader usage flag.
        * @return
        *  A flag that can be passed to IDirect3DDevice8::CreateVertexShader().
        */
        unsigned int GetPreferedVertexShaderUsageFlag();

        /**
        * Loads all shaders sources form the descriptor array \e pShaderDesc into \e colShaders.
        * If the file path for a vertex or pixel shader is \b NULL the according source code entry in
        *  \e colShaders will be set to an empty string.
        *
        * @param colShaders
        *  Shader source list.
        * @param pShaderDesc
        *  Shader descriptor array.
        * @param uiNumShaders
        *  Number of shaders to load.
        * @return
        *  \b true if successfull else \b false.
        * @brief
        *  Loads all shaders sources form the descriptor \e pShaderDesc into \e colShaders.
        */
        bool LoadShaderSources( EffectSystem::SShaderSourceList& colShaders, 
                                const EffectSystem::SShaderDescriptor* const pShaderDesc,
                                unsigned int uiNumShaders ) const;

        /**
        * Gets the linearily interpolated value from the specified bass line array.
        * 
        * @param fTime
        *  Effect relative time for which the bass line value should be retrieved.
        * @param pcBassLine
        *  Pointer to bass line array.
        * @param uiBassLineSize
        *  Size of bass line array.
        * @return
        *  Linearily interpolated bass line value.
        * @brief
        *  Gets the linearily interpolated value from the specified bass line array.
        */
        float GetBassLineValue( float fTime, 
                                const unsigned char* const pcBassLine, 
                                unsigned int uiBassLineSize );

        /**
        * Draws a background using texture #pcBackgroundTexture from #m_pkTextureManger.
        * 
        * @param pcBackgroundTexture
        *  Texture to use as a background.
        * @brief
        *  Draws a background using texture #pcBackgroundTexture from #m_pkTextureManger.
        */
        void DrawBackground( const char* const pcBackgroundTexture );

        /** 
        * Indicates whether effect has been registered yet or not.
        * 
        * @brief
        *  Indicates whether effect has been registered yet or not.
        */
        bool m_bRegistered;

        /** 
        * Indicates whether effect was created in browser or demo mode.
        * 
        * @brief
        *  Indicates whether effect was created in browser or demo mode.
        */
        bool m_bBrowserMode;

        /** 
        * Time (in seconds) when effect starts.
        * 
        * @brief
        *  Time (in seconds) when effect starts.
        */
        float m_fEffectStart;

        /** 
        * Time (in seconds) when effect ends.
        * 
        * @brief
        *  Time (in seconds) when effect ends.
        */
        float m_fEffectEnd;

        /** 
        * Direct3D device used for rendering the effect. 
        * 
        * @brief
        *  Direct3D device used for rendering the effect.
        */
        IDirect3DDevice8* m_pD3DDev;

        /** 
        * Pointer to a scrambled data archive. Used to load effect data. It's most likely that
        * several effects share a pointer to the same scrambled archive.
        * 
        * @brief
        *  Pointer to a scrambled data archive.
        */
        ScrambledArchive::CScrambledArchive* m_pkScrambledArchive;

        /** 
        * Pointer to a texture manager. Used as a shared pool of static textures. It's suggested
        * that all effects sharing the same scrambled archive should also share a pointer to a same 
        * texture manager in order to efficiently re-use textures.
        * 
        * @brief
        *  Pointer to a texture manager.
        */
        Shared::CTextureManager* m_pkTextureManager;

        /**
        * An effect transition manager.
        *
        * @brief
        *  An effect transition manager.
        */
        CEffectTransition* m_pkEffectTransition;

        /** 
        * World transform matrix.
        * 
        * @brief
        *  World transform matrix.
        */
        D3DXMATRIX m_matWorld; 

        /** 
        * View transform matrix.
        * 
        * @brief
        *  View transform matrix.
        */
        D3DXMATRIX m_matView;

        /** 
        * Projection matrix.
        * 
        * @brief
        *  Projection matrix.
        */
        D3DXMATRIX m_matProj;
    };



#include "Effect.inl"



} // namespace EffectSystem



#endif // #ifndef _EFFECT_H_INCLUDED_
