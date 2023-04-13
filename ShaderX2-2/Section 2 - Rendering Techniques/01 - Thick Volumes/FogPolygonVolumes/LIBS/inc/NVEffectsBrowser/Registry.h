#ifndef __NVEB_REGISTRY_H__
#define __NVEB_REGISTRY_H__

#include <atlbase.h>

// Update to regular CRegKey class to add querying/setting of binary values
class CRegKeyEx : public CRegKey
{
    public:
       LONG QueryBinaryValue(LPCTSTR pszValueName, void* pValue, ULONG* pnBytes);
       LONG SetBinaryValue(LPCTSTR pszValueName, const void* pData, ULONG nBytes);
};

inline LONG CRegKeyEx::QueryBinaryValue(LPCTSTR pszValueName, void* pValue, ULONG* pnBytes)
{
	LONG lRes;
	DWORD dwType;

	ATLASSERT(pnBytes != NULL);
	ATLASSERT(m_hKey != NULL);

	lRes = ::RegQueryValueEx(m_hKey, pszValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pValue),
		pnBytes);
	if (lRes != ERROR_SUCCESS)
		return lRes;
	if (dwType != REG_BINARY)
		return ERROR_INVALID_DATA;

	return ERROR_SUCCESS;
}

inline LONG CRegKeyEx::SetBinaryValue(LPCTSTR pszValueName, const void* pData, ULONG nBytes)
{
	ATLASSERT(m_hKey != NULL);
	return ::RegSetValueEx(m_hKey, pszValueName, NULL, REG_BINARY, reinterpret_cast<const BYTE*>(pData), nBytes);
}


#endif