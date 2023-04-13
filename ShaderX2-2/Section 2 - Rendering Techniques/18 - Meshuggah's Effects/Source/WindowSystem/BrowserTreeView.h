///////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Carsten Wenzel
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

/**
 * Declaration of \e WindowSystem::CBrowserTreeView.
 * 
 * @file
 *  BrowserTreeView.h
 * @author
 *  Carsten Wenzel
 * @date
 *  11.10.2001
 * @brief
 *  Declaration of \e WindowSystem::CBrowserTreeView.
 */



#ifndef _BROWSERTREEVIEW_H_INCLUDED_
#define _BROWSERTREEVIEW_H_INCLUDED_

#pragma once



#include <list>
#include <string>



namespace EffectSystem
{
    class CEffectFactory;
}



namespace WindowSystem
{



    class CBrowserRenderView;
    class CBrowserShaderFormView;



    /**
    * A tree view holding all effects.
    * It allows to select an effect that will be show in the render view.
    * 
    * @author
    *  Carsten Wenzel
    * @date
    *  11.10.2001
    * @brief
    *  A tree view holding all effects.
    */
    class CBrowserTreeView : public CTreeView
    {
    public:
        /** 
        * Sets the style of the tree view.
        * 
        * @brief
        *  Sets the style of the tree view.
        */
        virtual void OnInitialUpdate();

        /**
        * Sets the render view. Needed to set a new effect the user selected
        * by clicking on an item in the tree view.
        * 
        * @param pkRenderView
        *  Pointer to render view of browser.
        * @brief
        *  Sets the render view.
        */
        void SetRenderView( CBrowserRenderView* pkRenderView );

        /**
        * Sets the shader view. Needed to update shader sources when user clicked
        * on a different effect in the tree view.
        * 
        * @param pkShaderView
        *  Pointer to shader view of browser.
        * @brief
        *  Sets the shader view.
        */
        void SetShaderView( CBrowserShaderFormView* pkShaderView );

    protected:
        /** 
        * Constructor.
        * 
        * @brief
        *  Constructor.
        */
        CBrowserTreeView();

        /** 
        * Destructor.
        * 
        * @brief
        *  Destructor.
        */
        virtual ~CBrowserTreeView();

        /**
        * Message handler for \c WM_CREATE messages.
        * Creates the window, loads icons and builds the effect tree.
        * 
        * @param lpCreateStruct
        *  Window creation parameters.
        * @return
        *  \b 0 if successful. \b -1 if creation failed.
        * @brief
        *  Message handler for \c WM_CREATE messages.
        */
        afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );

        /** 
        * Handles "Selection of tree view is changing" situations.
        * 
        * @brief
        *  Handles "Selection of tree view is changing" situations.
        */
        afx_msg void OnTvnSelChanging( NMHDR *pNMHDR, LRESULT *pResult );

        /** 
        * Handles "Tree view item expanded" situations. Toggles root icon
        * between #FOLDER_OPEN and #FOLDER_CLOSED.
        * 
        * @brief
        *  Handles "Tree view item expanded" situations.
        */
        afx_msg void OnTvnItemExpanded( NMHDR *pNMHDR, LRESULT *pResult );

        DECLARE_DYNCREATE( CBrowserTreeView )

    #ifdef _DEBUG
        /** 
        * MFC debug hook. Only available in debug builds.
        * 
        * @brief
        *  MFC debug hook. Only available in debug builds.
        */
        virtual void AssertValid() const;

        /** 
        * MFC debug hook. Only available in debug builds.
        * 
        * @brief
        *  MFC debug hook. Only available in debug builds.
        */
        virtual void Dump( CDumpContext& dc ) const;
    #endif

        DECLARE_MESSAGE_MAP()

    private:
        /** 
        * A type holding a list of effect factories.
        * 
        * @brief
        *  A type holding a list of effect factories.
        */
        typedef std::list< EffectSystem::CEffectFactory* > EffectFactoryList;

    private:
        /** 
        * List of icon IDs.
        * 
        * @brief
        *  List of icon IDs.
        */
        enum EImageListID
        {
            /** 
            * <em>Folder closed</em> icon.
            */
            FOLDER_CLOSED,

            /** 
            * <em>Folder open</em> icon.
            */
            FOLDER_OPEN,

            /** 
            * \e Effect icon.
            */
            EFFECT,

            /** 
            * <em>Effect currently playing</em> icon.
            */
            EFFECT_PLAYING,

            /** 
            * Number of previously defined IDs for image list.
            */
            NUM_IMAGES
        };

        /**
        * Inserts root node.
        * 
        * @return
        *  Handle to root node of tree view.
        * @brief
        *  Inserts root node.
        */
        HTREEITEM InsertRoot();

        /**
        * Inserts an effect into the tree.
        * 
        * @param stdEffectName
        *  Name of effect. Used to label the effect's tree item.
        * @param pkEffectFactory
        *  Factory to create the effect when selected.
        * @param hRoot
        *  Root node of tree.
        * @brief
        *  Inserts an effect into the tree.
        */
        void InsertEffect( const std::string& stdEffectName, 
                           EffectSystem::CEffectFactory* pkEffectFactory, 
                           HTREEITEM hRoot );

        /** 
        * Pointer to render view of browser.
        * 
        * @brief
        *  Pointer to render view of browser.
        */
        CBrowserRenderView* m_pkRenderView;

        /** 
        * Pointer to shader view of browser.
        * 
        * @brief
        *  Pointer to shader view of browser.
        */
        CBrowserShaderFormView* m_pkShaderView;

        /** 
        * Image list for all loaded icons.
        * 
        * @brief
        *  Image list for all loaded icons.
        */
        CImageList m_kImageList;

        /** 
        * List of effect factories. One for each effect.
        * 
        * @brief
        *  List of effect factories.
        */
        EffectFactoryList m_colEffectFactories;
    };



} // namespace WindowSystem



#endif // #ifndef _BROWSERTREEVIEW_H_INCLUDED_
