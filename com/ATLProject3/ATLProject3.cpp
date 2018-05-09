// ATLProject3.cpp: реализация WinMain


#include "stdafx.h"
#include "resource.h"
#include "ATLProject3_i.h"


using namespace ATL;


class CATLProject3Module : public ATL::CAtlExeModuleT< CATLProject3Module >
{
public :
	DECLARE_LIBID(LIBID_ATLProject3Lib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLPROJECT3, "{e4d108aa-4d14-4d39-a035-bc9cd7412ade}")
};

CATLProject3Module _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

