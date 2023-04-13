///
///		Texture object
///
///
///		Copyright (c) 2003 F. Brebion (f.brebion@vrcontext.com)
///
///		Licence/disclaimer: This demo is provided 'as-is', without any express or implied
///		warranty. In no event can i be held liable for any damages arising from the use
///		of that code or this demo.
///
///		Permission is granted to anyone to use the code for any purpose, including
///		commercial usage, and to alter it and redistribute it freely, subject to the
///		following restrictions:
///
///		1. Proper credits must be given to the author for his contribution to the code
///		if the code is totally or partially reused. If you implement the described technic
///		yourself, credits are appreciated but not required. I'd also appreciate it
///		if you send me an email saying how you're going to use the code for.
///
///		2. The 3D models and textures in this demo belong to Daniel Cornibert
///		(danielcornibert@hotmail.com) and are included in it as pure curtesy from his
///		part. PERMISSION TO USE THIS ART IS NOT GRANTED by this license.
///

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <d3d9.h>
#include <d3dx9.h> 

#include "ijl.h"
#include "CTexture.h"
#include "CUtils.h"

c_texMgr *c_texMgr::ms_mgr = NULL;

///
///     Creates a texture from a TGA file
///
c_texture::c_texture(IDirect3DDevice9 *a_device, const char *a_fName)
{
    m_lpDevice = a_device;
    m_obj = NULL;
    strcpy(m_name, a_fName);

    if (_stricmp(a_fName + strlen(a_fName) - 4, ".tga") == 0)
    {
        /// Reading TGA file

        FILE *f = fopen(a_fName, "rb");
        if (f == NULL)
        {
            char str[256];
            sprintf(str, "Texture '%s' not found, please reinstall the demo!", a_fName);
            MessageBox(NULL, str, "Orthanc Demo", MB_OK);
            exit(1);
        }

        TGAHeader tgaHeader;
        fread(&tgaHeader, sizeof(TGAHeader), 1, f);

        if (tgaHeader.imagetype != 2 || (tgaHeader.imgspec.bpp != 24 &&
            tgaHeader.imgspec.bpp != 32))
        {
            /// can't create the image
            MessageBox(NULL, "Invalid TGA type", "Error", MB_OK);
            fclose(f);
            return;
        }

        m_width = tgaHeader.imgspec.w;
        m_height = tgaHeader.imgspec.h;
        m_bpp = tgaHeader.imgspec.bpp / 8;
        m_data = new char[m_width * m_height * m_bpp];

        fread(m_data, m_width * m_height * m_bpp, 1, f);

        fclose(f);
    }
    else if (_stricmp(a_fName + strlen(a_fName) - 4, ".jpg") == 0)
    {
        /// Reading JPG file

        IJLERR jerr;
        JPEG_CORE_PROPERTIES jcprops;
   
        jerr = ijlInit(&jcprops);
        if (jerr != IJL_OK)
        {
            MessageBox(NULL, "IJLInit problem", "Error", MB_OK);
            return;
        }

        jcprops.JPGFile = a_fName;
        jerr = ijlRead(&jcprops, IJL_JFILE_READPARAMS);
        if (jerr != IJL_OK)
        {
            char str[256];
            sprintf(str, "Error reading JPEG parameters (%s)", a_fName);
            MessageBox(NULL, str, "Error", MB_OK);
            ijlFree(&jcprops);
            return;
        }

        if (jcprops.JPGChannels != 3)
        {
            MessageBox(NULL, "Invalid JPG type", "Error", MB_OK);
            ijlFree(&jcprops);
            return;
        }

        m_width = jcprops.JPGWidth;
        m_height = jcprops.JPGHeight;
        m_bpp = 3;
        m_data = new char[m_width * m_height * 3];
		char *tmp = new char[m_width * m_height * 3];

        jcprops.DIBWidth = m_width;
        jcprops.DIBHeight = m_height;
        jcprops.DIBChannels = 3;
        jcprops.DIBColor = IJL_BGR;
        jcprops.DIBPadBytes = IJL_DIB_PAD_BYTES(jcprops.JPGWidth, 3);
        jcprops.DIBBytes = reinterpret_cast<BYTE*>(tmp);

        //Set the JPG color space ... this will always be somewhat of an
        //educated guess at best because JPEG is "color blind" (i.e.,
        //nothing in the bit stream tells you what color space the data was
        //encoded from. However, in this example we assume that we are
        //reading JFIF files which means that 3 channel images are in the
        //YCbCr color space and 1 channel images are in the Y color space.
        switch(jcprops.JPGChannels)
        {
            case 1: jcprops.JPGColor = IJL_G;
                break;
      
            case 3: jcprops.JPGColor = IJL_YCBCR;
                break;
      
            default:
                //This catches everything else, but no color twist will be
                //performed by the IJL.
                jcprops.DIBColor = (IJL_COLOR)IJL_OTHER;
                jcprops.JPGColor = (IJL_COLOR)IJL_OTHER;
                break;
        }

        jerr = ijlRead(&jcprops, IJL_JFILE_READWHOLEIMAGE);
        if (jerr != IJL_OK)
        {
            MessageBox(NULL, "Can't read JPG data", "Error", MB_OK);
            ijlFree(&jcprops);
            return;
        }

        ijlFree(&jcprops);

		/// The image is inverted, we must flip it now..
		char *tmpSrc;
		char *tmpDst;

		for (int j = 0; j < m_height; j++)
		{
			tmpSrc = tmp + j * m_width * 3;
			tmpDst = m_data + (m_height - j - 1) * m_width * 3;
			memcpy(tmpDst, tmpSrc, m_width * 3);
		}

		delete [] tmp;
    }
    else
    {
        char str[256];
        sprintf(str, "Texture %s: unknown extension type, only support .tga .bmp and .jpg !",
            a_fName);
        MessageBox(0, str, "Error", MB_OK);
    }

    if (m_width != 1 && m_width != 2 && m_width != 4 && m_width != 8 && m_width != 16
        && m_width != 32 && m_width != 64 && m_width != 128 && m_width != 256 &&
        m_width != 512 && m_width != 1024)
    {
        char str[256];
        sprintf(str, "Texture %s: width is not a valid power of 2 !",
            a_fName);
        MessageBox(0, str, "Warning", MB_OK);
    }

    if (m_height != 1 && m_height != 2 && m_height != 4 && m_height != 8 && m_height != 16
        && m_height != 32 && m_height != 64 && m_height != 128 && m_height != 256 &&
        m_height != 512 && m_height != 1024)
    {
        char str[256];
        sprintf(str, "Texture %s: height is not a valid power of 2 !",
            a_fName);
        MessageBox(0, str, "Warning", MB_OK);
    }

    HRESULT hRes = a_device->CreateTexture(m_width, m_height, 0,
        D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
        &m_obj, NULL);

    D3DLOCKED_RECT lRect;
    hRes = m_obj->LockRect(0, &lRect, NULL, 0);

    if (m_bpp == 3)
    {
        char *data = (char *)lRect.pBits;
        char *odata = m_data;
        for (int j = 0; j < m_height; j++)
        {
            for (int i = 0; i < m_width; i++)
            {
                data[0] = odata[0];
                data[1] = odata[1];
                data[2] = odata[2];
                data[3] = (char)255;
                data += 4;
                odata += 3;
            }
        }
    }
    else
    {
        char *data = (char *)lRect.pBits;
        char *odata = m_data;
        for (int j = 0; j < m_height; j++)
        {
            for (int i = 0; i < m_width; i++)
            {
                data[0] = odata[0];
                data[1] = odata[1];
                data[2] = odata[2];
                data[3] = odata[3];
                data += 4;
                odata += 4;
            }
        }
    }

    m_obj->UnlockRect(0);

    c_texMgr::ms_mgr->addTexture(this);
}

///
///     Binds the texture to the given TMU
///
void c_texture::bind(const int tmu)
{
    HRESULT hRes = m_lpDevice->SetTexture(tmu, m_obj);
    if (FAILED(hRes))
        return;

    m_lpDevice->SetSamplerState(tmu, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_lpDevice->SetSamplerState(tmu, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_lpDevice->SetSamplerState(tmu, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}

///
///     Sets the texture operation(s)
///
void c_texture::modulate(const int tmu)
{
    m_lpDevice->SetTextureStageState(tmu, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_lpDevice->SetTextureStageState(tmu, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_lpDevice->SetTextureStageState(tmu, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_lpDevice->SetTextureStageState(tmu, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_lpDevice->SetTextureStageState(tmu, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_lpDevice->SetTextureStageState(tmu, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
}

void c_texture::replace(const int tmu)
{
    m_lpDevice->SetTextureStageState(tmu, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_lpDevice->SetTextureStageState(tmu, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_lpDevice->SetTextureStageState(tmu, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_lpDevice->SetTextureStageState(tmu, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
}

///
///     Destroys the texture
///
c_texture::~c_texture()
{
    delete [] m_data;
}

///
///     Default constructor
///
c_texMgr::c_texMgr()
{
    m_nbTex = 0;
    ms_mgr = this;
}

///
///     Default destructor
///
c_texMgr::~c_texMgr()
{
    ms_mgr = NULL;
}

///
///     Look for a texture with the given name
///
c_texture *c_texMgr::getTexture(const char *name)
{
    for (int i = 0; i < m_nbTex; i++)
    {
        if (m_tex[i] == NULL)
            continue;

        if (stricmp(m_tex[i]->getName(), name) == 0)
            return(m_tex[i]);
    }

    return(NULL);
}

///
///     Adds a texture to the manager
///
void c_texMgr::addTexture(c_texture *tex)
{
    m_tex[m_nbTex++] = tex;
}
