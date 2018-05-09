// Object.h: объявление CObject

#pragma once
#include "resource.h"       // основные символы
#include <atlcomcli.h>
#include <string>
#include <chrono>


#include "ATLProject3_i.h"



using namespace ATL;


// CObject

class ATL_NO_VTABLE CObject :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CObject, &CLSID_Object>,
	public IDispatchImpl<IObject, &IID_IObject, &LIBID_ATLProject3Lib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
  std::wstring shared_name;

public:
	CObject()
	{
    using std::chrono::system_clock;
    system_clock::time_point tp = system_clock::now();
    auto d = std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch());
    shared_name = std::to_wstring(d.count());
	}

DECLARE_REGISTRY_RESOURCEID(106)


BEGIN_COM_MAP(CObject)
	COM_INTERFACE_ENTRY(IObject)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
  virtual HRESULT STDMETHODCALLTYPE GetSharedMemoryName(BSTR* pVal)
  {
    CComBSTR str(shared_name.c_str());
    str.CopyTo(pVal);
    return S_OK;
  }


};

OBJECT_ENTRY_AUTO(__uuidof(Object), CObject)
