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
  public IPersistPropertyBag2,
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CObject, &CLSID_Object>,
	public IDispatchImpl<IObject, &IID_IObject, &LIBID_ATLProject3Lib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
  HRESULT m_Dirty;
  long long m_value;

public:
	CObject()
	{

	}

DECLARE_REGISTRY_RESOURCEID(106)


BEGIN_COM_MAP(CObject)
	COM_INTERFACE_ENTRY(IObject)
	COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY(IPersistPropertyBag2)
  COM_INTERFACE_ENTRY2(IPersist, IPersistPropertyBag2)
END_COM_MAP()

BEGIN_PROP_MAP(CObject)
  PROP_DATA_ENTRY("value", m_value, VT_UI8)
END_PROP_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
  STDMETHOD(GetClassID)(CLSID *pClassID) 
  {
    ATLTRACE(atlTraceCOM, 2, _T("IObject::GetClassID\n") );
    if (pClassID == NULL)
      return E_POINTER;

    *pClassID = this->GetObjectCLSID();
    return S_OK;
  }

  virtual STDMETHODIMP InitNew()
  {
    using std::chrono::system_clock;
    system_clock::time_point tp = system_clock::now();
    m_value = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
    return S_OK;
  }

  virtual STDMETHODIMP IsDirty()
  {
    return m_Dirty;
  }

  virtual STDMETHODIMP Load(IPropertyBag2 *pBag, IErrorLog *pLog) {
    // Иницализируем VARIANT типом VT_BSTR
//    CComVariant v( (BSTR)NULL );

    // Инициализируем VARIANT типом VT_I4
//    v = 0L;

//    HRESULT hr = pBag->Read(OLESTR("value"), &v, pLog);
//    if ( FAILED(hr) ) return hr;
//    m_value = v.lVal;

    return S_OK;
  }

  virtual STDMETHODIMP Save(IPropertyBag2 *pBag,
    BOOL fClearDirty, BOOL /* fSaveAllProperties */) {

//    CComVariant v = m_value;
//    HRESULT hr = pBag->Write(OLESTR("value"), &v);
//    if ( FAILED(hr) ) return hr;

//    if (fClearDirty) m_Dirty = FALSE;
//    return hr;
    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE GetSharedMemoryName(BSTR* pVal)
  {
    std::wstring shared_name = std::to_wstring(m_value);
    CComBSTR str( shared_name.c_str() );
    str.CopyTo(pVal);
    return S_OK;
  }

};

OBJECT_ENTRY_AUTO(__uuidof(Object), CObject)
