/*********************************************************************NVMH4****
Path:  NVSDK\Common\include\nv_connections
File:  common.h

Copyright NVIDIA Corporation 2002
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
*AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.



Comments:

A holder class designed to simplify interaction with effect parameterdesc info.



******************************************************************************/

#ifndef __NVCGFXTYPE_H
#define __NVCGFXTYPE_H

namespace nv_sys
{


class NVCgFXType
{
public:
	NVCgFXType::NVCgFXType()
		: m_pType(NULL),
		m_dwSize(0),
		m_Type(nv_fx::NVFXPT_UNKNOWN)
	{
		ZeroMemory(&m_Dimensions[0], sizeof(UINT) * NVFX_MAX_DIMENSIONS);
	}

	NVCgFXType::~NVCgFXType()
	{
		free(m_pType);
		m_pType = NULL;
	}

	NVCgFXType(const nv_fx::NVFXPARAMETER_DESC& pdesc, void* pParam = NULL)
	{
		Setup(pdesc.Type, pdesc.Name, pParam, pdesc.Dimension);
	}

	NVCgFXType(const char* strName, const char* strValue)
	{
		Setup(nv_fx::NVFXPT_STRING, strName, (void*)strValue);		
	}

	NVCgFXType(const NVCgFXType& rhs)
	{
		Setup(rhs.m_Type, rhs.m_strName, rhs.m_pType, &rhs.m_Dimensions[0]);
	}

	const NVCgFXType& operator = (const NVCgFXType& rhs)
	{
		if (m_pType)
			free(m_pType);
	
		m_pType = NULL;

		Setup(rhs.m_Type, rhs.m_strName, rhs.m_pType, &rhs.m_Dimensions[0]);
		
		return *this;
	}


	void CopyData(const void* pValue)
	{
		assert(m_pType);
		if (m_pType && pValue)
			memcpy(m_pType, pValue, m_dwSize);
	}

	void Setup(nv_fx::NVFXPARAMETERTYPE Type, const char* strName, const void* pParameter = NULL, const UINT* pDimensions = NULL)
	{
		m_pType = NULL;
		m_Type = Type;
		m_strName = strName;
		m_dwSize = 0;

		// Copy the dimensions, or set to 1000
		if (pDimensions)
		{
			memcpy(&m_Dimensions[0], pDimensions, NVFX_MAX_DIMENSIONS * sizeof(UINT));
		}
		else
		{
			m_Dimensions[0] = 1;
			m_Dimensions[1] = 0;
			m_Dimensions[2] = 0;
			m_Dimensions[3] = 0;
		}

		m_dwSize = 1;
		unsigned int Count = 0;
		for (Count = 0; Count < NVFX_MAX_DIMENSIONS; Count++)
		{
			if (m_Dimensions[Count] != 0)
			{
				m_dwSize *= m_Dimensions[Count];
			}
		}

		switch (Type)
		{
			case nv_fx::NVFXPT_STRING:
			{
				if (pParameter)
					m_dwSize = strlen((char*)pParameter) + 1;
			
				assert(m_Dimensions[0] == 1 && 
					m_Dimensions[1] == 0 && 
					m_Dimensions[2] == 0 && 
					m_Dimensions[3] == 0);
			}
			break;

			case nv_fx::NVFXPT_BOOL:
			{
				m_dwSize *= sizeof(BOOL);
			}
			break;

			case nv_fx::NVFXPT_INT:
			{
				m_dwSize *= sizeof(int);
			}
			break;

			case nv_fx::NVFXPT_FLOAT:
			{
				m_dwSize *= sizeof(float);
			}
			break;

			case nv_fx::NVFXPT_DWORD:
			case nv_fx::NVFXPT_TEXTURE:
			case nv_fx::NVFXPT_CUBETEXTURE:
			case nv_fx::NVFXPT_VOLUMETEXTURE:
			case nv_fx::NVFXPT_VERTEXSHADER:
			case nv_fx::NVFXPT_PIXELSHADER:
			{
				m_dwSize *= sizeof(DWORD);
			}
			break;

			case nv_fx::NVFXPT_UNKNOWN:
			{
				m_dwSize = 0;
			}
			break;


			default:
			{
				m_dwSize = 0;
				break;
			}
		}

		if (m_dwSize)
		{
			m_pType = malloc(m_dwSize);
			ZeroMemory(m_pType, m_dwSize);

			if (pParameter)
				memcpy(m_pType, pParameter, m_dwSize);
		}
	}

	bool operator == (const NVCgFXType& rhs)
	{
		if ((m_Type == rhs.m_Type) &&
			(m_dwSize == rhs.m_dwSize) &&
			(!stricmp(m_strName, rhs.m_strName)))
		{
			if (memcmp(m_pType, rhs.m_pType, m_dwSize) == 0)
				return true;
		}
		return false;
	}

	UINT m_Dimensions[NVFX_MAX_DIMENSIONS];
	DWORD m_dwSize;
	nv_fx::NVFXPARAMETERTYPE m_Type;
	const char* m_strName;									// Name of the parameter.
	void* m_pType;										// Pointer to the parameter data
};


}; // namespace nv_sys

#endif __NVCGFXTYPE_H