///
///     ASE file loader
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

#include "CObject.h"
#include "CUtils.h"
#include "CASEFile.h"

///
///     Compares 2 normals
///
static int normComp(
    const void          *arg1,              // first entity
    const void          *arg2               // second entity
    )
{
    s_normal *n1 = (s_normal *)arg1;
    s_normal *n2 = (s_normal *)arg2;

    if (n1->x < n2->x)
        return(-1);

    if (n1->x > n2->x)
        return(1);

    if (n1->y < n2->y)
        return(-1);

    if (n1->y > n2->y)
        return(1);

    if (n1->z < n2->z)
        return(-1);

    if (n1->z > n2->z)
        return(1);

    return(0);
}

///
///     Compares 2 tex coords
///
static int texCoordComp(
    const void          *arg1,              // first entity
    const void          *arg2               // second entity
    )
{
    s_texcoord *t1 = (s_texcoord *)arg1;
    s_texcoord *t2 = (s_texcoord *)arg2;

    if (t1->u < t2->u)
        return(-1);

    if (t1->u > t2->u)
        return(1);

    if (t1->v < t2->v)
        return(-1);

    if (t1->v > t2->v)
        return(1);

    return(0);
}

///
///     Compares 2 triplets
///
static int tripletComp(
    const void          *arg1,              // first entity
    const void          *arg2               // second entity
    )
{
    s_triplet *t1 = (s_triplet *)arg1;
    s_triplet *t2 = (s_triplet *)arg2;

    if (t1->pos < t2->pos)
        return(-1);

    if (t1->pos > t2->pos)
        return(1);

    if (t1->tc < t2->tc)
        return(-1);

    if (t1->tc > t2->tc)
        return(1);

    if (t1->norm < t2->norm)
        return(-1);

    if (t1->norm > t2->norm)
        return(1);

    return(0);
}

///
///     Compares 2 faces
///
static int facesComp(
    const void          *arg1,              // first entity
    const void          *arg2               // second entity
    )
{
    s_face *f1 = (s_face *)arg1;
    s_face *f2 = (s_face *)arg2;

    if (f1->m_subID < f2->m_subID)
        return(-1);
    
    if (f1->m_subID > f2->m_subID)
        return(1);

    return(0);
}

///
///     Reads & analyzes a new line from the current ASE file
///
void c_aseFile::analyze()
{
    float x, y, z;
    int index;
    int v0, v1, v2;
    int count;
    static char dummy[1024];
	static char str[256];
    static char cmd[256];
    static char arg[256];
    int l = 0;
    char c = '\0';

    while (c != '\n' && c != '\r')
    {
        if (fread(&c, 1, 1, m_file) != 1 || l > 1020)
            break;

        dummy[l++] = c;
    }

    dummy[l++] = '\0';

    /// Reads the command name
    int ll = 0;
    for (int n = 0; n < l && (dummy[n] == ' ' || dummy[n] == '\t'); n++);
    for (; n < l && dummy[n] != ' ' && dummy[n] != '\t'; n++, ll++)
        cmd[ll] = dummy[n];
    cmd[ll] = '\0';
    ll = 0;
    for (; n < l && (dummy[n] == ' ' || dummy[n] == '\t'); n++);
    for (; n < l; n++, ll++)
        arg[ll] = dummy[n];
    arg[ll] = '\0';
    ll = strlen(arg);
    for (n = ll - 1; n >= 0; n--)
    {
        if (arg[n] != ' ' && arg[n] != '\t' && arg[n] != '\r' && arg[n] != '\n' &&
            arg[n] != 10)
            break;

        arg[n] = '\0';
    }

    /// Read vertex positions

	if (sscanf(dummy, " *TM_ROW0 %f %f %f", &x, &y, &z) == 3 && m_inGeom)
	{
		m_rotMat0[0] = x;
		m_rotMat1[0] = y;
		m_rotMat2[0] = z;
	}

	if (sscanf(dummy, " *TM_ROW1 %f %f %f", &x, &y, &z) == 3 && m_inGeom)
	{
		m_rotMat0[1] = x;
		m_rotMat1[1] = y;
		m_rotMat2[1] = z;
	}

	if (sscanf(dummy, " *TM_ROW2 %f %f %f", &x, &y, &z) == 3 && m_inGeom)
	{
		m_rotMat0[2] = x;
		m_rotMat1[2] = y;
		m_rotMat2[2] = z;
	}

    if (sscanf(dummy, " *MESH_NUMVERTEX %d", &count) == 1)
    {
        /// Number of vertices

        m_nbVPos = count;
        m_VPos = (float *)malloc(count * sizeof(float) * 3);
    }

    if (sscanf(dummy, " *MESH_VERTEX %d %f %f %f", &index, &x, &y, &z) == 4)
    {
        /// We've read a vertex index & position!

        m_VPos[index * 3 + 0] = y;
        m_VPos[index * 3 + 1] = z;
        m_VPos[index * 3 + 2] = x;
    }

    /// Read texture coordinates

    if (sscanf(dummy, " *MESH_NUMTVERTEX %d", &count) == 1)
    {
        /// Number of tex coords

		if (count < 1)
        {
            char str[256];
            sprintf(str, "The element %s has no texture coordinates. Please delete it or texture-map it.",
                m_name);
            MessageBox(NULL, str, "Error while loading mesh", 0);
            exit(3);
        }

        m_nbVTC = count;
        m_VTC = (s_texcoord *)malloc(count * sizeof(s_texcoord));
        if (m_VTC == NULL)
        {
            warning("Can't allocate VTC\n");
        }
    }

    if (sscanf(dummy, " *MESH_TVERT %d %f %f %f", &index, &x, &y, &z) == 4)
    {
        /// We've read the tex coords

        if (m_VTC == NULL)
        {
            warning("Invalid VTC\n");
        }

        m_VTC[index].u = x;
        m_VTC[index].v = y;
        m_VTC[index].id = index;
    }

    /// Read normals

    if (sscanf(dummy, " *MESH_FACENORMAL %d %f %f %f", &index, &x, &y,
        &z) == 4)
    {
        /// A face normal (followed by 3 MESH_VERTEXNORMAL)

        m_normFace = index;
        m_normOffset = 0;
    }

    if (sscanf(dummy, " *MESH_VERTEXNORMAL %d %f %f %f", &index, &x, &y,
        &z) == 4)
    {
        /// Same, but for normals..

        int id = m_normFace * 3 + m_normOffset;

		float v[3];
		v[0] = x;
		v[1] = y;
		v[2] = z;
		x = dot(m_rotMat0, v);
		y = dot(m_rotMat1, v);
		z = dot(m_rotMat2, v);

		float l = sqrtf(x * x + y * y + z * z);
		l = 1.0f / l;

        m_VNorm[id].x = y * l;
        m_VNorm[id].y = z * l;
        m_VNorm[id].z = x * l;
        m_VNorm[id].id = id;
        m_faces[m_normFace].m_norm[m_normOffset] = id;
        m_normOffset++;
    }

    /// Read faces

    if (sscanf(dummy, " *MESH_NUMFACES %d", &count) == 1)
    {
        /// Number of faces

        m_nbVNorm = count * 3;
        m_VNorm = (s_normal *)malloc(m_nbVNorm * sizeof(s_normal));

        m_nbFaces = count;
        m_faces = (s_face *)malloc(count * sizeof(s_face));
    }

    if (sscanf(dummy, " *MESH_FACE %d: A: %d B: %d C: %d", &index,
        &v0, &v1, &v2) == 4)
    {
        /// Reading the position indices

        if (vecEq(m_VPos + v0 * 3, m_VPos + v1 * 3) ||
            vecEq(m_VPos + v1 * 3, m_VPos + v2 * 3) ||
            vecEq(m_VPos + v2 * 3, m_VPos + v0 * 3))
        {
            warning("Degenerated triangle..\n");

            m_faces[index].m_pos[0] = -1;
            m_faces[index].m_pos[1] = -1;
            m_faces[index].m_pos[2] = -1;
            m_faces[index].m_subID = 0;
        }
        else
        {
            m_faces[index].m_pos[0] = v0;
            m_faces[index].m_pos[1] = v1;
            m_faces[index].m_pos[2] = v2;
            m_faces[index].m_subID = 0;

            char *meshMtlID = strstr(dummy, "*MESH_MTLID");
            if (meshMtlID != NULL)
            {
                if (sscanf(meshMtlID, "*MESH_MTLID %d", &v0) == 1)
                    m_faces[index].m_subID = v0;
            }
        }
    }

    if (sscanf(dummy, " *MESH_TFACE %d %d %d %d", &index, &v0, &v1,
        &v2) == 4)
    {
        /// Reading the tex coord indices

        m_faces[index].m_tc[0] = v0;
        m_faces[index].m_tc[1] = v1;
        m_faces[index].m_tc[2] = v2;
    }

    /// Material & general stuff

	if (stricmp(cmd, "*MATERIAL_COUNT") == 0 && sscanf(dummy, " *MATERIAL_COUNT %d", &count) == 1)
	{
		/// Number of textures/materials

        m_mats = new s_material *[256 * count];
        memset(m_mats, NULL, 256 * count * sizeof(s_material *));
	}

	if (stricmp(cmd, "*MATERIAL_REF") == 0 && sscanf(dummy, " *MATERIAL_REF %d", &index) == 1)
	{
		/// Material index

        validateMat();
        validateLight();

		char str[256];
		sprintf(str, "Validating '%s': %d faces with material %d\n",
			m_name, m_nbFaces, index);
		warning(str);

        validate(index);
	}

    if (stricmp(cmd, "*MAP_DIFFUSE") == 0 ||
        stricmp(cmd, "*MAP_SPECULAR") == 0 ||
        stricmp(cmd, "*MAP_BUMP") == 0 ||
        stricmp(cmd, "*MAP_SHINESTRENGTH") == 0 ||
        stricmp(cmd, "*MAP_OPACITY") == 0 ||
        stricmp(cmd, "*MAP_REFLECT") == 0 ||
        stricmp(cmd, "*MAP_SHINE") == 0 ||
        stricmp(cmd, "*MAP_REFRACT") == 0
        )
    {
        m_inDiffuse = false;
        m_inBump = false;
        m_inSpecular = false;
        m_inReflection = false;
        m_inRefraction = false;

        if (stricmp(cmd, "*MAP_DIFFUSE") == 0)
            m_inDiffuse = true;
    }

    if (stricmp(cmd, "*MATERIAL") == 0 && sscanf(dummy, " *MATERIAL %d", &index) == 1)
    {
        /// Material definition ID

        validateLight();
        validateMat();

        m_matID = index;
        m_subMatID = 0;

        m_mat.m_ambient = 0.0f;
        m_mat.m_tex = NULL;
        m_mat.m_recvShadows = true;
    }

    if (sscanf(dummy, " *SUBMATERIAL %d", &index) == 1)
    {
        /// Sub-material ID

        if (m_matID >= 0 && m_subMatID >= 0 && index > 0)
        {
            /// validates previous material..
            s_material *mat = c_scene::ms_scene->addMaterial(&m_mat);
            m_mats[256 * m_matID + m_subMatID] = mat;
            m_subMatID = -1;
        }

        m_subMatID = index;

        m_mat.m_ambient = 0.0f;
        m_mat.m_tex = NULL;
        m_mat.m_recvShadows = true;
    }

    if (stricmp(cmd, "*BITMAP") == 0 && (m_inDiffuse || m_inBump || m_inSpecular ||
        m_inReflection || m_inRefraction))
    {
        char buf[256];
        int l = strlen(arg);

        if (arg[l - 1] == '\"')
            arg[l - 1] = '\0';

        char *texName = strrchr(arg, '\\');
        if (texName == NULL)
            texName = arg;

        if (texName[0] == '"')
            texName++;
        if (texName[0] == '\\')
            texName++;

#ifdef CHECKERS
        strcpy(texName, "checkers.jpg");
#endif
        c_texture *tex = c_texMgr::ms_mgr->getTexture(texName);
        if (tex != NULL)
        {
            if (m_inDiffuse)
                m_mat.m_tex = tex;
            return;
        }
        char *ext = strrchr(texName, '.');

        sprintf(buf, "Texture %d: %s\n", m_matID, texName);
        warning(buf);

        if (texName == NULL || strlen(texName) < 4 || ext == NULL)
        {
            char str[256];
            sprintf(str, "Error on line %s: invalid path to bitmap file..",
                dummy);
            MessageBox(NULL, str, "Ignoring problem..", 0);
            return;
        }

        FILE *f = fopen(texName, "rb");
        if (f != NULL)
            fclose(f);
        else
        {
            /// tries to locate the file on disk..

            char ext1[10];
            strncpy(ext1, ext, 8);
            sprintf(ext, "*%s", ext1);

            for (int n = 0; n < (int)strlen(texName); n++)
            {
                if (texName[n] == ' ')
                    texName[n] = '*';
            }

            while (true)
            {
                bool rep = false;
                for (int n = 0; n < (int)strlen(texName); n++)
                {
                    if (texName[n] == '*' && texName[n + 1] == '*')
                    {
                        rep = true;
                        for (int k = n; k < (int)strlen(texName); k++)
                            texName[k] = texName[k + 1];
                        break;
                    }
                }
                if (!rep)
                    break;
            }

           	WIN32_FIND_DATA fd;
        	HANDLE h = FindFirstFile(texName, &fd);
            if (h == INVALID_HANDLE_VALUE)
            {
                /// try with another extension..
                ext = strrchr(texName, '.');
                strcpy(ext, ".JPG");
            	HANDLE h = FindFirstFile(texName, &fd);
                if (h == INVALID_HANDLE_VALUE)
                {
                    char str[256];
                    sprintf(str, "Texture file %s not found.. click OK will ignore.", texName);
                    warning(str);
                    MessageBox(NULL, str, "Warning", MB_OK);
                    strcpy(texName, "clear.tga");
                }
                else
                    strcpy(texName, fd.cFileName);
            }
            else
                strcpy(texName, fd.cFileName);
        }

        char str[256];
        sprintf(str, "Loading texture %s\n", texName);
        warning(str);

        if (m_inDiffuse)
        {
            c_texture *texObj = new c_texture(m_lpDevice, texName);
            m_mat.m_tex = texObj;
        }
    }

    if (sscanf(dummy, " *MATERIAL_SELFILLUM %f", &x) == 1)
        m_mat.m_ambient = x;

    if (stricmp(cmd, "*NODE_NAME") == 0 && sscanf(dummy, " *NODE_NAME %s", str) == 1)
    {
		/// Node name

        char *ps = dummy;
        while (*ps != 34 && *ps != 0)
            ps++;
        int n = 0;
        if (*ps == 34)
            ps++;
        while (*ps != 34 && *ps != 0)
        {
            str[n++] = *ps;
            ps++;
        }
        str[n] = '\0';

        if (strlen(m_gname) > 0)
        {
            if (m_inLight)
                sprintf(m_light.m_name, "%s_%s", str, m_gname);
            else
                sprintf(m_name, "%s_%s", m_gname, str);
        }
        else
        {
            if (m_inLight)
                strcpy(m_light.m_name, str);
            else
                strcpy(m_name, str);
        }
    }

    if (stricmp(cmd, "*GEOMOBJECT") == 0)
    {
        validateLight();
        validateMat();

        m_inDiffuse = false;
        m_inGeom = true;
        m_inLight = false;
        m_inBump = false;
        m_inSpecular = false;
        m_inReflection = false;
        m_inRefraction = false;
    }

    if (stricmp(cmd, "*LIGHTOBJECT") == 0)
    {
        validateLight();
        validateMat();

        m_inDiffuse = false;
        m_inGeom = false;
        m_inLight = true;
        m_inBump = false;
        m_inSpecular = false;
        m_inReflection = false;
        m_inRefraction = false;

        m_light.m_castShadows = true;
        m_lightID = 0;
        memset(&m_light, 0, sizeof(s_light));
    }

	if (sscanf(dummy, " *TM_POS %f %f %f", &x, &y, &z) == 3 && m_inLight)
    {
        m_light.m_pos[0] = y;
        m_light.m_pos[1] = z;
        m_light.m_pos[2] = x;
    }

	if (sscanf(dummy, " *LIGHT_COLOR %f %f %f", &x, &y, &z) == 3 && m_inLight)
    {
        m_light.m_color[0] = x;
        m_light.m_color[1] = y;
        m_light.m_color[2] = z;
        m_light.m_color[3] = 1.0f;
    }
    
    if (stricmp(cmd, "*LIGHT_SHADOWS") == 0 && m_inLight)
    {
        m_light.m_castShadows = (!(stricmp(cmd, "*OFF") == 0));
    }

    if (sscanf(dummy, " *LIGHT_INTENS %f", &x) == 1 && m_inLight)
    {
        m_light.m_radius = x;
    }
}

///
///     Validates the object: calculate the vertex data and indices
///
void c_aseFile::validate(const int matIndex)
{
    c_object *obj = new c_object(m_lpDevice);

    /// Delete degenerated triangles from faces list
    for (int f = 0; f < m_nbFaces; f++)
    {
        if (m_faces[f].m_pos[0] == -1)
        {
            /// Exchange #f and #(m_nbFaces-1) faces

            memcpy(m_faces + f, m_faces + (m_nbFaces - 1), sizeof(s_face));
            memcpy(m_VNorm + f * 3, m_VNorm + (m_nbFaces - 1) * 3, sizeof(s_normal) * 3);
            m_VNorm[f * 3 + 0].id = f * 3 + 0;
            m_VNorm[f * 3 + 1].id = f * 3 + 1;
            m_VNorm[f * 3 + 2].id = f * 3 + 2;
            m_nbFaces--;
            m_faces = (s_face *)realloc(m_faces, m_nbFaces * sizeof(s_face));
            m_nbVNorm -= 3;
            m_VNorm = (s_normal *)realloc(m_VNorm, m_nbVNorm * sizeof(s_normal));
            m_faces[f].m_norm[0] = f * 3 + 0;
            m_faces[f].m_norm[1] = f * 3 + 1;
            m_faces[f].m_norm[2] = f * 3 + 2;
            f--;
        }
    }

    /// Sort faces by sub primitive ID..
    qsort(m_faces, m_nbFaces, sizeof(s_face), facesComp);

    /// Step 1: remove normals duplicates
    qsort(m_VNorm, m_nbVNorm, sizeof(s_normal), normComp);

    int *assoc = new int[m_nbVNorm];
    for (int i = 0; i < m_nbVNorm; i++)
    {
        if (m_VNorm[i].id < 0 || m_VNorm[i].id >= m_nbVNorm)
        {
            MessageBox(NULL, "Internal error: invalid normal ID", 0, MB_OK);
            exit(1034);
        }

        assoc[m_VNorm[i].id] = i;
    }

    int start = 0;

    for (i = 1; i < m_nbVNorm; i++)
    {
        /// fusion normals that are the same

        if (normComp(&(m_VNorm[i - 1]), &(m_VNorm[i])) == 0)
            assoc[m_VNorm[i].id] = start;
        else
            start = i;
    }

    for (i = 0; i < m_nbFaces; i++)
    {
        /// update faces indices

        m_faces[i].m_norm[0] = assoc[m_faces[i].m_norm[0]];
        m_faces[i].m_norm[1] = assoc[m_faces[i].m_norm[1]];
        m_faces[i].m_norm[2] = assoc[m_faces[i].m_norm[2]];
    }

    delete [] assoc;

    /// Step 2: remove texture coords duplicates

    qsort(m_VTC, m_nbVTC, sizeof(s_texcoord), texCoordComp);

    assoc = new int[m_nbVTC];
    for (i = 0; i < m_nbVTC; i++)
        assoc[m_VTC[i].id] = i;

    start = 0;

    for (i = 1; i < m_nbVTC; i++)
    {
        /// fusion tex coords that are the same

        if (texCoordComp(&(m_VTC[i - 1]), &(m_VTC[i])) == 0)
            assoc[m_VTC[i].id] = start;
        else
            start = i;
    }

    for (i = 0; i < m_nbFaces; i++)
    {
        /// update faces indices

        if (m_faces[i].m_tc[0] < 0 || m_faces[i].m_tc[0] >= m_nbVTC ||
            m_faces[i].m_tc[1] < 0 || m_faces[i].m_tc[1] >= m_nbVTC ||
            m_faces[i].m_tc[2] < 0 || m_faces[i].m_tc[2] >= m_nbVTC)
        {
            MessageBox(NULL, "An object seems to be missing valid texture coordinates..",
                "Error", MB_OK);
            exit(0);
        }

        m_faces[i].m_tc[0] = assoc[m_faces[i].m_tc[0]];
        m_faces[i].m_tc[1] = assoc[m_faces[i].m_tc[1]];
        m_faces[i].m_tc[2] = assoc[m_faces[i].m_tc[2]];
    }

    delete [] assoc;

    /// Step 3: determine all the unique triplets

    s_triplet *triplet = new s_triplet[m_nbFaces * 3];

    for (i = 0; i < m_nbFaces; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            triplet[i * 3 + j].id = i * 3 + j;
            triplet[i * 3 + j].pos = m_faces[i].m_pos[j];
            triplet[i * 3 + j].tc = m_faces[i].m_tc[j];
            triplet[i * 3 + j].norm = m_faces[i].m_norm[j];
            triplet[i * 3 + j].index = -1;
        }
    }

    qsort(triplet, m_nbFaces * 3, sizeof(s_triplet), tripletComp);

    assoc = new int[m_nbFaces * 3];
    for (i = 0; i < m_nbFaces * 3; i++)
        assoc[triplet[i].id] = i;

    start = 0;

    for (i = 1; i < m_nbFaces * 3; i++)
    {
        /// fusion triplets that are the same

        if (tripletComp(&(triplet[i - 1]), &(triplet[i])) == 0)
            assoc[triplet[i].id] = start;
        else
            start = i;
    }

    /// Now, we just have to parse the triplets and build the vertex
    /// and index arrays on-the-fly

    int new_nbVerts = 0;

    for (i = 0; i < m_nbFaces * 3; i++)
    {
        int id = assoc[i];

        if (triplet[id].index == -1)
        {
            triplet[id].index = 0;
            new_nbVerts++;
        }
    }

    obj->m_nbVerts = 0;
    obj->m_nbFrames = 1;
    obj->m_curFrame = 0;
    obj->m_verts[0] = (s_stdVertex *)malloc(sizeof(s_stdVertex) * new_nbVerts);
	memset(obj->m_verts[0], 0, new_nbVerts * sizeof(s_stdVertex));

    obj->m_indices = (int *)malloc(sizeof(int) * m_nbFaces * 3);

    for (i = 0; i < m_nbFaces * 3; i++)
        triplet[i].index = -1;

    new_nbVerts = 0;

    for (i = 0; i < m_nbFaces * 3; i++)
    {
        int id = assoc[i];

        if (triplet[id].index == -1)
        {
            triplet[id].index = new_nbVerts;
            new_nbVerts++;

            memcpy(
                obj->m_verts[0][triplet[id].index].m_pos,
                m_VPos + triplet[id].pos * 3,
                sizeof(float) * 3
                );

            memcpy(
                obj->m_verts[0][triplet[id].index].m_tc,
				&(m_VTC[triplet[id].tc].u),
                sizeof(float) * 2
                );

            memcpy(
                obj->m_verts[0][triplet[id].index].m_normal,
                &(m_VNorm[triplet[id].norm].x),
                sizeof(float) * 3
                );
        }

        obj->m_indices[i] = triplet[id].index;
    }

    obj->m_nbVerts += new_nbVerts;
    obj->m_nbIndices += (m_nbFaces * 3);

    strcpy(obj->m_name, m_name);

    /// Last step: generate the primitives, that's the most simple part left :)

    if (m_mats[256 * matIndex + 1] == NULL)
    {
        /// no sub-mats
        for (i = 0; i < m_nbFaces; i++)
            m_faces[i].m_subID = 0;
    }

    int startOffset = 0;
    int subMatID = m_faces[0].m_subID;
   
    for (i = 0; i < m_nbFaces; i++)
    {
        if (m_faces[i].m_subID != subMatID)
        {
            obj->m_nbPrims++;
            obj->m_primsData = (s_primitive *)realloc(obj->m_primsData, sizeof(s_primitive) *
                    obj->m_nbPrims);
            obj->m_primsData[obj->m_nbPrims - 1].m_start = startOffset * 3;
            obj->m_primsData[obj->m_nbPrims - 1].m_size = (i - startOffset) * 3;
            
            if (m_mats[256 * matIndex + subMatID] == NULL)
                exit(1035);

            obj->m_primsData[obj->m_nbPrims - 1].m_mat = m_mats[256 * matIndex + subMatID];

            startOffset = i;
            subMatID = m_faces[i].m_subID;
        }
    }

    obj->m_nbPrims++;
    obj->m_primsData = (s_primitive *)realloc(obj->m_primsData, sizeof(s_primitive) *
        obj->m_nbPrims);
    obj->m_primsData[obj->m_nbPrims - 1].m_start = startOffset * 3;
    obj->m_primsData[obj->m_nbPrims - 1].m_size = (m_nbFaces - startOffset) * 3;

    if (m_mats[256 * matIndex + subMatID] == NULL)
        exit(1035);

    obj->m_primsData[obj->m_nbPrims - 1].m_mat = m_mats[256 * matIndex + subMatID];

    if (obj->m_primsData[obj->m_nbPrims - 1].m_mat->m_tex == NULL)
    {
        c_texture *texObj = new c_texture(m_lpDevice, "clear.tga");
        obj->m_primsData[obj->m_nbPrims - 1].m_mat->m_tex = texObj;
    }
    free(m_VPos);
    free(m_VTC);
    free(m_VNorm);
    free(m_faces);

    delete [] assoc;
    delete [] triplet;

    m_nbVPos = 0;
    m_VPos = NULL;
    m_nbVTC = 0;
    m_VTC = NULL;
    m_nbVNorm = 0;
    m_VNorm = NULL;
    m_normFace = 0;
    m_normOffset = 0;
    m_faces = NULL;
    m_nbFaces = 0;

    obj->create();
    m_objs[m_nbObjs++] = obj;
    c_scene::ms_scene->addObject(obj);
}

///
///     Creates an ASE file
///
c_aseFile::c_aseFile(IDirect3DDevice9 *a_device, const char *fname, const char *gname)
{
    m_lpDevice = a_device;

    m_file = NULL;
    strcpy(m_gname, gname);

    strcpy(m_name, "Unknown");
    m_matID = -1;
    m_subMatID = -1;
    m_lightID = -1;
    m_inDiffuse = false;
    m_inGeom = false;
    m_inLight = false;
    m_inBump = false;
    m_inSpecular = false;
    m_inReflection = false;
    m_inRefraction = false;

    memset(&m_light, 0, sizeof(s_light));
    memset(&m_mat, 0, sizeof(s_material));

    m_nbVPos = 0;
    m_VPos = NULL;
    m_nbVTC = 0;
    m_VTC = NULL;
    m_nbVNorm = 0;
    m_VNorm = NULL;
    m_normFace = 0;
    m_normOffset = 0;
    m_mats = NULL;

    m_nbFaces = 0;
    m_faces = NULL;
    m_mats = NULL;

    m_nbObjs = 0;

    char binName[256];
	sprintf(binName, "%s.ase", fname);
    m_file = fopen(binName, "r");
    if (m_file == NULL)
    {
        sprintf(binName, "Model file '%s.ase' missing..", fname);
        MessageBox(NULL, binName, "Error", MB_OK);
        exit(1038);
    }
}

///
///     Reads the ASE file
///
void c_aseFile::load()
{
    if (m_file == NULL)
        return;

    while (!feof(m_file))
        analyze();

    validateLight();
    validateMat();
}

///
///		Destroys the ASE file
///
c_aseFile::~c_aseFile()
{
    fclose(m_file);

    if (m_VPos)
        free(m_VPos);
    if (m_VTC)
        free(m_VTC);
    if (m_VNorm)
        free(m_VNorm);
    if (m_faces)
        free(m_faces);
    if (m_mats)
        delete [] m_mats;
}

void c_aseFile::validateLight()
{
    if (m_lightID >= 0)
    {
        s_light *light = new s_light();
        light->m_castShadows = m_light.m_castShadows;
        copy4f(light->m_ocolor, m_light.m_color);
        copy4f(light->m_color, m_light.m_color);
        copy3f(light->m_pos, m_light.m_pos);
        copy3f(light->m_opos, m_light.m_pos);
        strcpy(light->m_name, m_light.m_name);
        light->m_radius = m_light.m_radius;
        light->m_oradius = m_light.m_radius;
        light->m_disabled = m_light.m_disabled;
        c_scene::ms_scene->addLight(light);
        m_lightID = -1;
    }
}

///
///     Validates a material
///
void c_aseFile::validateMat()
{
    if (m_matID >= 0 && m_subMatID >= 0)
    {
        /// validates previous material..
        s_material *mat = c_scene::ms_scene->addMaterial(&m_mat);
        m_mats[256 * m_matID + m_subMatID] = mat;
        m_matID = -1; m_subMatID = -1;
    }
}
