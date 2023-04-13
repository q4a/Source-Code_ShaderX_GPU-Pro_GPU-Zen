///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgIResourceManager
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIResourceManager.cpp,v 1.2 2002/10/23 19:39:36 daniel Exp $
///////////////////////////////////////////////////////////////////


#include "pgIResourceManager.h"

#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgLog.h>
#include <pgLib/Base/pgISettings.h>
#include <pgLib/Base/pgIStringTool.h>

#include <pgLib/Game/pgCharacter.h>

#include <pgLib/Math/pgPathLinear.h>

#include <pgLib/FrameWork/D3DUtil.h>

#include <pgLib/Render/pgAnimated.h>
#include <pgLib/Render/pgMeshUtil.h>
#include <pgLib/Render/pgParticleSystem.h>
#include <pgLib/Render/pgTexture.h>

#include <direct.h>


static LPDIRECT3DDEVICE8			pd3dDevice = NULL;

static pgTexture*					textures[pgIResourceManager::MAX_TEXTURES];

static pgPtrList<pgImage>			imageList;

static pgPtrList<pgBaseMesh>		baseMeshList;
static pgList<pgString>				baseMeshNames;

static pgPtrList<pgMesh>			meshList;
static pgList<pgString>				meshNames;

static pgPtrList<pgAnimated>		animatedList;
static pgList<pgString>				animatedNames;

static pgPtrList<pgParticleSystem>	particleList;
static pgList<pgString>				particleNames;

static pgPtrList<pgPath>			pathList;
static pgList<pgString>				pathNames;

static pgPtrList<pgCharacter>		characterList;

static pgTexture*					texNotFound = NULL;



pgTexture* getLoadedTexture(int nID);
pgTexture* getLoadedTexture(const pgString& nName);

int getLoadedTextureID(const pgString& nName);
const pgString& getLoadedTextureName(int nID);
const pgString& getLoadedTextureName(const pgTexture* nTex);

pgBaseMesh* getLoadedBaseMesh(const pgString& nName);
pgMesh* getLoadedMesh(const pgString& nName);
pgAnimated* getLoadedAnimated(const pgString& nName);
pgParticleSystem* getLoadedParticleSystem(const pgString& nName);
pgPath* getLoadedPath(const pgString& nName);


void
pgIResourceManager::init()
{
	for(int i=0; i<MAX_TEXTURES; i++)
		textures[i] = NULL;

	pd3dDevice = pgIDirectX::getDevice();

	if(pd3dDevice==NULL)
		pgLog::error("pgIResourceManager::init failed: d3d device not available");

	texNotFound = getTexture("texture_not_found", SOURCE_STD);
}


pgImage*
pgIResourceManager::getRawImage(const pgString& nFileName, int nWidth, int nHeight, 
							   pgImage::FORMAT nFormat, SOURCE nSource)
{
	char			imgName[256];
	int				size = nWidth * nHeight * pgImage::getPixelSize(nFormat), read;
	FILE*			fp;
	unsigned char*	data;
	pgImage*		img;
	pgString		fullName;

	pgIStringTool::removeExtension(nFileName, imgName);

	switch(nSource)
	{
		case SOURCE_BSP:
			fullName = pgISettings::tmpFullBspTexPath(nFileName, ".raw");
			break;
		case SOURCE_MD2:
			fullName = pgISettings::tmpFullMD2Path(nFileName, ".raw");
			break;
		case SOURCE_OBJ:
			fullName = pgISettings::tmpFullOBJPath(nFileName, ".raw");
			break;
		case SOURCE_STD:
			fullName = pgISettings::tmpFullTexturePath(nFileName, ".raw");
			break;
		default:
			return NULL;
	}

	if(size==0)
	{
		pgLog::error("can not load raw image '%s': illegal image properties", imgName);
		return NULL;
	}

	fp=fopen(fullName, "rb");
	if(fp==NULL)
	{
		pgLog::error("can not load raw image '%s': file not found", imgName);
		return NULL;
	}

	data = new unsigned char[size];
	read = fread(data, 1, size, fp);

	if(read!=size)
	{
		pgLog::error("loading raw image '%s' with %d x %d pixels (%d bytes) failed: file not large enough", imgName, nWidth, nHeight, size);
		fclose(fp);
		return NULL;
	}

	img = new pgImage(nWidth, nHeight, nFormat, imgName, data, false);
	imageList.addTail(img);
	fclose(fp);

	pgLog::info("raw image '%s' (%dx%d %dbits) loaded", imgName, nWidth, nHeight, 8*pgImage::getPixelSize(nFormat));

	return img;
}


pgTexture*
pgIResourceManager::getTexture(const pgString& nFileName, SOURCE nSource, pgImage::FORMAT nFormat)
{
	pgString	fName0, fName1, fName2, fName3;
	char		texName[256];
	int			i;

	pgIStringTool::removeExtension(nFileName, texName);

	if(pd3dDevice==NULL)
	{
		pgLog::error("can not load texture '%s': d3d device not set yet", texName);
		return NULL;
	}

	pgTexture* tex = getLoadedTexture(texName);
	if(tex!=NULL)								// already loaded ???
		return tex;

	for(i=0; i<MAX_TEXTURES; i++)
		if(textures[i] == NULL)
		{
			LPDIRECT3DTEXTURE8 tex = NULL;
			D3DXIMAGE_INFO imgInfo;

			switch(nSource)
			{
				case SOURCE_BSP:
					fName0 = pgISettings::tmpFullBspTexPath(texName, ".dds");
					fName1 = pgISettings::tmpFullBspTexPath(texName, ".tga");
					fName2 = pgISettings::tmpFullBspTexPath(texName, ".jpg");
					fName3 = pgISettings::tmpFullBspTexPath(texName, ".bmp");
					break;

				case SOURCE_MD2:
					fName0 = pgISettings::tmpFullMD2Path(texName, ".dds");
					fName1 = pgISettings::tmpFullMD2Path(texName, ".tga");
					fName2 = pgISettings::tmpFullMD2Path(texName, ".jpg");
					fName3 = pgISettings::tmpFullMD2Path(texName, ".bmp");
					break;

				case SOURCE_OBJ:
					fName0 = pgISettings::tmpFullOBJPath(texName, ".dds");
					fName1 = pgISettings::tmpFullOBJPath(texName, ".tga");
					fName2 = pgISettings::tmpFullOBJPath(texName, ".jpg");
					fName3 = pgISettings::tmpFullOBJPath(texName, ".bmp");
					break;

				case SOURCE_STD:
					fName0 = pgISettings::tmpFullTexturePath(texName, ".dds");
					fName1 = pgISettings::tmpFullTexturePath(texName, ".tga");
					fName2 = pgISettings::tmpFullTexturePath(texName, ".jpg");
					fName3 = pgISettings::tmpFullTexturePath(texName, ".bmp");
					break;

				default:
					return NULL;
			}

			D3DFORMAT fmt = pgImage::getD3DFormat(nFormat);
			if(FAILED(D3DUtil_CreateTexture(pd3dDevice, fName0, &tex, fmt, &imgInfo)))
				if(FAILED(D3DUtil_CreateTexture(pd3dDevice, fName1, &tex, fmt, &imgInfo)))
					if(FAILED(D3DUtil_CreateTexture(pd3dDevice, fName2, &tex, fmt, &imgInfo)))
						if(FAILED(D3DUtil_CreateTexture(pd3dDevice, fName3, &tex, fmt, &imgInfo)))
						{
							pgLog::error("texture '%s' not found: using 'texture_not_found'", texName);
							return texNotFound;
						}

			textures[i] = new pgTexture(texName, tex, imgInfo.Width, imgInfo.Height,
										(pgImage::FORMAT)imgInfo.Format);

			pgLog::info("texture '%s' (%dx%d %s) loaded", texName, imgInfo.Width, imgInfo.Height, pgImage::getFormatString((pgImage::FORMAT)imgInfo.Format));
			return textures[i];
		}

	pgLog::error("can not load texture '%s': too many textures in memory (%d)", texName, MAX_TEXTURES);
	return NULL;
}



pgImage*
pgIResourceManager::getRawImage(const pgString& nFileName, int nWidth, int nHeight, pgImage::FORMAT nFormat)
{
	char			imgName[256];
	int				size = nWidth * nHeight * pgImage::getPixelSize(nFormat), read;
	FILE*			fp;
	unsigned char*	data;
	pgImage*		img;
	pgString		fullName;

	pgIStringTool::removeExtension(nFileName, imgName);
	fullName = pgISettings::tmpFullPath(nFileName, ".raw");

	if(size==0)
	{
		pgLog::error("can not load raw image '%s': illegal image properties", imgName);
		return NULL;
	}

	fp=fopen(fullName, "rb");
	if(fp==NULL)
	{
		pgLog::error("can not load raw image '%s': file not found", imgName);
		return NULL;
	}

	data = new unsigned char[size];
	read = fread(data, 1, size, fp);

	if(read!=size)
	{
		pgLog::error("loading raw image '%s' with %d x %d pixels (%d bytes) failed: file not large enough", imgName, nWidth, nHeight, size);
		fclose(fp);
		return NULL;
	}

	img = new pgImage(nWidth, nHeight, nFormat, imgName, data, false);
	imageList.addTail(img);
	fclose(fp);

	pgLog::info("raw image '%s' (%dx%d %dbits) loaded", imgName, nWidth, nHeight, 8*pgImage::getPixelSize(nFormat));

	return img;
}

pgTexture*
pgIResourceManager::getTexture(const pgString& nFileName, pgImage::FORMAT nFormat)
{
	pgString	fName0, fName1, fName2, fName3;
	char		texName[256];
	int			i;

	pgIStringTool::removeExtension(nFileName, texName);

	if(pd3dDevice==NULL)
	{
		pgLog::error("can not load texture '%s': d3d device not set yet", texName);
		return NULL;
	}

	pgTexture* tex = getLoadedTexture(texName);
	if(tex!=NULL)								// already loaded ???
		return tex;

	for(i=0; i<MAX_TEXTURES; i++)
		if(textures[i] == NULL)
		{
			LPDIRECT3DTEXTURE8 tex = NULL;
			D3DXIMAGE_INFO imgInfo;

			fName0 = pgISettings::tmpFullPath(nFileName, ".dds");
			fName1 = pgISettings::tmpFullPath(nFileName, ".tga");
			fName2 = pgISettings::tmpFullPath(nFileName, ".jpg");
			fName3 = pgISettings::tmpFullPath(nFileName, ".bmp");

			D3DFORMAT fmt = pgImage::getD3DFormat(nFormat);
			if(FAILED(D3DUtil_CreateTexture(pd3dDevice, fName0, &tex, fmt, &imgInfo)))
				if(FAILED(D3DUtil_CreateTexture(pd3dDevice, fName1, &tex, fmt, &imgInfo)))
					if(FAILED(D3DUtil_CreateTexture(pd3dDevice, fName2, &tex, fmt, &imgInfo)))
						if(FAILED(D3DUtil_CreateTexture(pd3dDevice, fName3, &tex, fmt, &imgInfo)))
						{
							pgLog::error("texture '%s' not found: using 'texture_not_found'", texName);
							return texNotFound;
						}

			textures[i] = new pgTexture(texName, tex, imgInfo.Width, imgInfo.Height,
										(pgImage::FORMAT)imgInfo.Format);

			pgLog::info("texture '%s' (%dx%d %s) loaded", texName, imgInfo.Width, imgInfo.Height, pgImage::getFormatString((pgImage::FORMAT)imgInfo.Format));
			return textures[i];
		}

	pgLog::error("can not load texture '%s': too many textures in memory (%d)", texName, MAX_TEXTURES);
	return NULL;
}


pgBaseMesh*
pgIResourceManager::getBaseMeshMD2(const pgString& nFileName, bool nLighting)
{
	char	name[256];
	pgBaseMesh*	mesh;

	pgIStringTool::removeExtension(nFileName, name);

	mesh = getLoadedBaseMesh(nFileName);
	if(mesh)
		return mesh;

	mesh = pgMeshUtil::loadMD2(nFileName, nLighting);

	baseMeshList.addTail(mesh);
	baseMeshNames.addTail(name);

	return mesh;
}


pgMesh*
pgIResourceManager::getMeshOBJ(const pgString& nFileName, bool nLighting, bool nCreateNew)
{
	char	name[256];
	pgMesh*	mesh;

	pgIStringTool::removeExtension(nFileName, name);

	if(!nCreateNew)
	{
		mesh = getLoadedMesh(nFileName);
		if(mesh)
			return mesh;
	}

	mesh = pgMeshUtil::loadOBJ(nFileName, nLighting);

	meshList.addTail(mesh);
	meshNames.addTail(name);

	return mesh;
}


pgAnimated*
pgIResourceManager::getAnimated(const pgString& nFileName)
{
	char		name[256];
	pgAnimated*	anim;

	pgIStringTool::removeExtension(nFileName, name);

	anim = getLoadedAnimated(name);
	if(anim)
		return anim;

	anim = new pgAnimated();
	anim->load(nFileName);

	animatedList.addTail(anim);
	animatedNames.addTail(name);

	return anim;
}


pgParticleSystem*
pgIResourceManager::getParticleSystem(const pgString& nFileName, bool nCreateNew)
{
	char				name[256];
	pgParticleSystem*	particles;

	pgIStringTool::removeExtension(nFileName, name);

	if(!nCreateNew)
	{
		particles = getLoadedParticleSystem(name);
		if(particles)
			return particles;
	}

	particles = new pgParticleSystem();
	particles->load(nFileName);

	particleList.addTail(particles);
	particleNames.addTail(name);

	return particles;
}


pgPathLinear*
pgIResourceManager::getLinearPath(const pgString& nFileName)
{
	char			name[256];

	pgIStringTool::removeExtension(nFileName, name);

	pgPath *p = getLoadedPath(name);
	if(p)
	switch(p->getType())
	{
	case pgPath::TYPE_LINEAR:
		return reinterpret_cast<pgPathLinear*>(p);

	default:
		break;
	}

	pgPathLinear *path = new pgPathLinear();
	path->load(nFileName);

	pathList.addTail(path);
	pathNames.addTail(name);

	return path;
}


pgTexture*
pgIResourceManager::getTexNotFound()
{
	return texNotFound;
}



pgAnimated*
getLoadedAnimated(const pgString& nName)
{
	for(int i=0; i<animatedNames.getSize(); i++)
		if(animatedNames[i]==nName)
			return animatedList[i];

	return NULL;
}


pgParticleSystem*
getLoadedParticleSystem(const pgString& nName)
{
	for(int i=0; i<particleNames.getSize(); i++)
		if(particleNames[i]==nName)
			return particleList[i];

	return NULL;
}


pgPath*
getLoadedPath(const pgString& nName)
{
	for(int i=0; i<pathNames.getSize(); i++)
		if(pathNames[i]==nName)
			return pathList[i];

	return NULL;
}


pgTexture*
getLoadedTexture(int nID)
{
	if(nID>=pgIResourceManager::MAX_TEXTURES || nID<0)
		return NULL;

	return textures[nID];
}


pgTexture*
getLoadedTexture(const pgString& nName)
{
	return getLoadedTexture(getLoadedTextureID(nName));
}


int
getLoadedTextureID(const pgString& nName)
{
	for(int i=0; i<pgIResourceManager::MAX_TEXTURES; i++)
		if(textures[i]!=NULL && textures[i]->getName()==nName)
			return i;

	return -1;

}


const pgString&
getLoadedTextureName(int nID)
{
	return textures[nID]->getName();
}


const pgString&
getLoadedTextureName(const pgTexture* nTex)
{
	for(int i=0; i<pgIResourceManager::MAX_TEXTURES; i++)
		if(textures[i]==nTex)
			return textures[i]->getName();

	return texNotFound->getName();
}


pgBaseMesh*
getLoadedBaseMesh(const pgString& nName)
{
	for(int i=0; i<baseMeshNames.getSize(); i++)
		if(baseMeshNames[i]==nName)
			return baseMeshList[i];

	return NULL;
}


pgMesh*
getLoadedMesh(const pgString& nName)
{
	for(int i=0; i<meshNames.getSize(); i++)
		if(meshNames[i]==nName)
			return meshList[i];

	return NULL;
}
