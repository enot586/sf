// dllmain.h: объявление класса модуля.

class CATLProject3Module : public ATL::CAtlDllModuleT< CATLProject3Module >
{
public :
	DECLARE_LIBID(LIBID_ATLProject3Lib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLPROJECT3, "{ff7bec05-18c4-4cc8-a6d5-69f449e61140}")
};

extern class CATLProject3Module _AtlModule;
