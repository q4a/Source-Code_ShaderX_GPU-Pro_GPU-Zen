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

#ifndef TEXTURE_H_
#define TEXTURE_H_

///
///     TGA image header.
///
#pragma pack(1)
typedef struct
{
  unsigned char idlength;
  unsigned char cmaptype;
  unsigned char imagetype;

  struct
  {
    unsigned short firstentryindex;
    unsigned short length;
    unsigned char entrysize;
  } cmapspec;

  struct
  {
    unsigned short x0;
    unsigned short y0;
    unsigned short w;
    unsigned short h;
    unsigned char bpp;
    unsigned char desc;
  } imgspec;

} TGAHeader;
#pragma pack(4)

class c_texture
{
    protected:

    int                 m_width;            // texture width
    int                 m_height;           // texture height
    int                 m_bpp;              // bytes per pixel
    char                *m_data;            // pixels data
    IDirect3DTexture9   *m_obj;             // texture object
    IDirect3DDevice9    *m_lpDevice;        // D3D device
    char                m_name[64];         // texture name

    public:

    ///
    ///     Creates a texture from a TGA file
    ///
    c_texture(IDirect3DDevice9 *a_device, const char *a_fName);

    ///
    ///     Binds the texture to the given TMU
    ///
    virtual void bind(const int tmu);

    ///
    ///     Sets the texture operation(s)
    ///
    virtual void modulate(const int tmu);
    virtual void replace(const int tmu);

    ///
    ///     Destroys the texture
    ///
    virtual ~c_texture();

    ///
    ///     Returns the BPP
    ///
    inline int getBPP() const               { return(m_bpp); }
    inline int getWidth() const             { return(m_width); }
    inline int getHeight() const            { return(m_height); }
    inline const char *getName() const      { return(m_name); }
};

class c_texMgr
{
    protected:

    int                 m_nbTex;            // number of textures
    c_texture           *m_tex[1024];       // all texture in manager

    public:

    static c_texMgr     *ms_mgr;            // texture manager instance

    ///
    ///     Default constructor
    ///
    c_texMgr();

    ///
    ///     Default destructor
    ///
    ~c_texMgr();

    ///
    ///     Look for a texture with the given name
    ///
    c_texture *getTexture(const char *name);

    ///
    ///     Adds a texture to the manager
    ///
    void addTexture(c_texture *tex);
};

#endif
