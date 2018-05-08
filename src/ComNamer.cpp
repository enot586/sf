#include <windows.h>
#include <exception>
#include <atlbase.h>
#include "../inc/ATLProject3_i.h"
#include "../inc/ATLProject3_i.c"
#include "ComNamer.h"

using namespace std;

ComNamer::ComNamer()
{
  if ( FAILED( CoInitialize(NULL) ) )
  {
    throw std::runtime_error("ERROR: Can\'t initialize COM object.");
  }

  IObject * pIObj;

  HRESULT hr = CoCreateInstance(CLSID_Object,
                                NULL, CLSCTX_SERVER,
                                IID_IObject, (void**)&pIObj);
  if ( SUCCEEDED(hr) ) {
    CComBSTR str;
    hr = pIObj->GetSharedMemoryName(&str);
 
    if ( SUCCEEDED(hr) ) {
      CW2A s( str );
      shared_memory_name = s;
    }

    pIObj->Release();
  }
}

ComNamer::~ComNamer()
{
  CoUninitialize();
}

string ComNamer::GetSharedMemoryName()
{
  return shared_memory_name;
}