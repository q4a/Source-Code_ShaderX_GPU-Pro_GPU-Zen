//---------------------------------------------------------------------------
/**
\class CTexture

\brief The main class for texture loading. It also allows some simple 
       operations on alpha channel.

 */
//---------------------------------------------------------------------------
#ifndef __CTEXTURE__
#define __CTEXTURE__
//---------------------------------------------------------------------------
#define TEXTURE_NAME_SIZE 256
#define TEXTURE_EPSILON 0.0001
//---------------------------------------------------------------------------
#include "CException.h"
#include <math.h>
//---------------------------------------------------------------------------
#ifndef ABS
#define ABS(x) ((x)>0.0?(x):-(x))
#endif
//---------------------------------------------------------------------------
class CTexture
{
protected:

  char	          m_szName[TEXTURE_NAME_SIZE];
  unsigned char  *m_Data;
  int		  m_iWidth,m_iHeight;
  bool		  m_bLoaded;
  bool		  m_bAlpha;
  bool            m_bDataOwner;
//  bool          m_bLuminance;
  
  unsigned char *CTexture::computeNormals(double scale);

public:

  class alpha_functor
  {
  public:
    virtual unsigned char computeAlpha(unsigned char r,unsigned char g,unsigned char b) const =0;
  };

  class alpha_zero : public alpha_functor
  {
  public:
    unsigned char computeAlpha(unsigned char r,unsigned char g,unsigned char b) const
      {return (0);}
  };
  
  class alpha_one : public alpha_functor
  {
  public:
    unsigned char computeAlpha(unsigned char r,unsigned char g,unsigned char b) const
      {return (255);}
  };

  class alpha_const : public alpha_functor
  {
    unsigned char m_ucAlpha;
  public:
    alpha_const(double c) {m_ucAlpha=(unsigned char)(c*255);}
    unsigned char computeAlpha(unsigned char r,unsigned char g,unsigned char b) const
      {return (m_ucAlpha);}
  };
  
  class alpha_color : public alpha_functor
  {
  private:
    int m_iTolerence;
    int m_iR;
    int m_iG;
    int m_iB;
  public:
    alpha_color(unsigned char r,unsigned char g,unsigned char b,int t=0)
      {
	m_iR=(int)r;
	m_iG=(int)g;
	m_iB=(int)b;
	m_iTolerence=t;
      }
    unsigned char computeAlpha(unsigned char r,unsigned char g,unsigned char b) const
      {
	if (   ABS((int)r-m_iR) <= m_iTolerence 
	    && ABS((int)g-m_iG) <= m_iTolerence 
	    && ABS((int)b-m_iB) <= m_iTolerence)
	  return (0);
	else
	  return (255);
      }
  };

public:

  CTexture();
  CTexture(const char *name,int w,int h,bool alpha,unsigned char *data);
  CTexture(const CTexture&);
  CTexture(CTexture *);
  virtual ~CTexture();
  
  virtual void            load(){};
  virtual void            unload();
  void                    save(const char *n);

  /// returns texture width
  int		  getWidth() const {return (m_iWidth);}
  /// returns texture height
  int     	  getHeight() const {return (m_iHeight);}
  // <b>true</b> if texture is loaded in memory
  bool	          isLoaded() const {return (m_bLoaded);}
  // <b>true</b> if texture is in RGBA format
  bool            isAlpha() const {return (m_bAlpha);}
  // returns texture data (see isAlpha to test if the texture format is RGB or RGBA)
  unsigned char  *getData() const {return (m_Data);}
  // returns the name of the texture (filename in most cases)
  const char     *getName() const {return (m_szName);}
  /**
     Convert texture to a normal texture. Luminance is used as an height field to compute normals
     by finite differencing. The scale factor applies to the height map.
  */
  void convertToNormal(double scale=1.0);
  /**
     Convert a texture to RGBA format by putting alpha to zero where the texture 
     color is [r,g,b] and one everywhere else. 
     It is usefull to make areas of a particular color transparent.
     The tolerence parameter is the tolerence used when comparing colors.
     Note that the same result can be obtained by using computeAlpha and alpha_color 
  */
  virtual void	convert2Alpha(unsigned char r,
			      unsigned char g,
			      unsigned char b,
			      unsigned char tolerence);
  /**
     Load an alpha map from the red component of another CTexture
  */
  virtual void            loadAlphaMap(CTexture *);
  /**
     Load an alpha map from the red component of an image file
  */
  void                    loadAlphaMap(const char *);
  /** 
      Compute alpha map from an alpha_functor (see alpha_zero and alpha_one)
  */
  void                    computeAlpha(const alpha_functor& af);
  /**
     Load a CTexture from an image file. The format is automaticaly deduced from extension.
  */
  static CTexture        *loadTexture(const char *);

  static void             saveTexture(CTexture *,const char *);

  /**
     Returns the size of texture data in memory.
  */
  int                 memsize() const;

  /** 
      Apply an horizontal flip.
  */
  void                flipY();

  /** 
      Apply a vertical flip.
  */
  void                flipX();

  void                setDataOwner(bool b) {m_bDataOwner=b;}

//  virtual void            setLuminance(bool b){m_bLuminance=b;}

//---------------------------------------------------------------------------
private:

  // simple vertex for normal computation
  class vertex
  {
  public:
    double x,y,z;
    
    vertex ()
      {
	x=0;
	y=0;
	z=0;
      }

    vertex(double _x,double _y,double _z)
      {
	x=_x;
	y=_y;
	z=_z;
      }

    void normalize()
      {
	double l=x*x+y*y+z*z;
	if (l < TEXTURE_EPSILON*TEXTURE_EPSILON)
	  CException("trying to normalize null vertex !");
	l=sqrt(l);
	x/=l;
	y/=l;
	z/=l;
      }

    vertex cross(const vertex& v)
      {
	return (vertex(y*v.z - z*v.y,z*v.x - x*v.z,x*v.y - y*v.x));
      }
  };

};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
