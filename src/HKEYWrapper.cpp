#include "stdafx.h"
#include "WindowsAPIError.h"
#include "HKEYWrapper.h"

namespace CxcIPConfig
{

HKEYWrapper::HKEYWrapper(HKEY hkey)
  : hkey_(hkey)
{
}

HKEYWrapper::~HKEYWrapper()
{
  Close();
}

long HKEYWrapper::Open(HKEY root, const std::string & sub, bool needWrite)
{
  TRACE_FUNC1(sub);

  Close();

  LRESULT status = ::RegOpenKeyEx(root, sub.c_str(), 0,
    needWrite ? KEY_WRITE : KEY_READ, &hkey_);
  if (ERROR_SUCCESS != status)
    throw WindowsAPIError(status, "RegOpenKeyEx", sub);

  INFO_LOG() << "RegOpenKeyEx: " << sub << " opened";
  return status;
}

void HKEYWrapper::Close()
{
  if (NULL != hkey_)
    ::RegCloseKey(hkey_);
  hkey_ = NULL;
}

long HKEYWrapper::Query(const std::string & value, std::string & data)
{
  TRACE_FUNC1(value);

  DWORD len = 255;
  BYTE lpData[255];
  LRESULT status = ::RegQueryValueEx(hkey_, value.c_str(), NULL, NULL, lpData, &len);
  if (ERROR_SUCCESS != status)
    throw WindowsAPIError(status, "RegQueryValueEx", value);

  data.assign((LPTSTR)lpData);
  INFO_LOG() << "RegQueryValueEx: " << value << " = " << data;

  return status;
}

}
