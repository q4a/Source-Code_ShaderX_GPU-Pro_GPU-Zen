///////////////////////////////////////////////////////////////////
//
// project:     Pluto Game Engine
// class:       pgIImageTool
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgIImageTool.cpp,v 1.1.1.1 2002/10/20 09:33:01 daniel Exp $
///////////////////////////////////////////////////////////////////

#include <pgLib/Base/pgIImageTool.h>
#include <pgLib/Base/pgImage.h>

#include <pgLib/Math/pgVec3n.h>

#include <stdio.h>


pgImage*
pgIImageTool::createLightmapFromHeightmap(const pgImage* nHeightmap, const pgVec3n& nDir,
										 float nAmbient, float nDiffuse)
{
	if(nHeightmap==0)
		return 0;

	const pgImage			*hm = nHeightmap;
	pgImage					*lm = new pgImage(hm->getWidth(), hm->getHeight(), pgImage::P8,
									  pgStringEx("Lightmap from %s", hm->getName()));
	int						w = hm->getWidth(),
							h = hm->getHeight(),
							x,y;
	unsigned char*			data = lm->getData();
	const unsigned char*	sdata = hm->getData();
	float					light;
	pgVec3					normal;

	memset(data, 0, w*h);

	for(y=1; y<h-1; y++)
		for(x=1; x<w-1; x++)
		{
			int				idx = x + y*w;
			int				//p0 = sdata[idx],
							pt = sdata[idx-w],
							pb = sdata[idx+w],
							pl = sdata[idx-1],
							pr = sdata[idx+1];

			normal.x = (pl-pr) / (2.0f*255.0f) * w;
			normal.y = 1.0f;
			normal.z = (pt-pb) / (2.0f*255.0f) * h;
			normal.normalize();

			float f = normal * nDir;

			if(f<0.0f)
				f = 0.0f;

			light = nAmbient + nDiffuse * f;
			assert(light<1.0f);

			data[x + y*w] = (unsigned char)(light * 255.0f);
			/*if(data[x + y*w]==251)
			{
				int i = 1;
			}*/
		}


	// simply copy values onto the borders...
	//
	data[0] = data[1+w];
	data[w-1] = data[w+w-2];

	data[(h-1)*w] = data[(h-2)*w+1];
	data[(h-1)*w+w-1] = data[(h-2)*w+w-2];

	for(x=1; x<w-1; x++)
	{
		data[x] = data[x+w];
		data[x+(h-1)*w] = data[x+(h-2)*w];
	}

	for(y=1; y<h-1; y++)
	{
		data[y*w] = data[y*w+1];
		data[y*w+w-1] = data[y*w+w-2];
	}

	FILE* fp = fopen("d:\\tst.raw", "wb");
	fwrite(data, 1, w*h, fp);
	fclose(fp);

	return lm;
}
