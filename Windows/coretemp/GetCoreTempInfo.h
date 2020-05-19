#pragma once

#ifdef GETCORETEMPINFO_EXPORTS
#define GETCORETEMPINFO_API __declspec(dllexport)
#else
#define GETCORETEMPINFO_API __declspec(dllimport)
#endif

#define UNKNOWN_EXCEPTION 0x20000000
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//#include <stdio.h>
//#include <tchar.h>



// TODO: reference additional headers your program requires here

#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct core_temp_shared_data
{
	unsigned int	uiLoad[256];
	unsigned int	uiTjMax[128];
	unsigned int	uiCoreCnt;
	unsigned int	uiCPUCnt;
	float			fTemp[256];
	float			fVID;
	float			fCPUSpeed;
	float			fFSBSpeed;
	float			fMultipier;	
	char			sCPUName[100];
	unsigned char	ucFahrenheit;
	unsigned char	ucDeltaToTjMax;
}CORE_TEMP_SHARED_DATA,*PCORE_TEMP_SHARED_DATA,**PPCORE_TEMP_SHARED_DATA;

bool GETCORETEMPINFO_API fnGetCoreTempInfo(CORE_TEMP_SHARED_DATA *&pData);
bool WINAPI fnGetCoreTempInfoAlt(CORE_TEMP_SHARED_DATA *pData);

class GETCORETEMPINFO_API CoreTempProxy
{
public:
	CoreTempProxy(void);
	virtual ~CoreTempProxy(void);
	
	UINT GetCoreLoad(int Index) const;
    UINT GetTjMax(int Index) const;
    UINT GetCoreCount() const;
    UINT GetCPUCount() const;
    float GetTemp(int Index) const;
    float GetVID() const;
    float GetCPUSpeed() const;
    float GetFSBSpeed() const;
    float GetMultiplier() const;
    LPCSTR GetCPUName() const;
    bool IsFahrenheit() const;
    bool IsDistanceToTjMax() const;
    const CORE_TEMP_SHARED_DATA &GetDataStruct() const;

	bool GetData();
	DWORD GetDllError() const { return GetLastError(); }
	LPCWSTR GetErrorMessage();
private:

	CORE_TEMP_SHARED_DATA m_pCoreTempData;
	WCHAR m_ErrorMessage[100];
};
