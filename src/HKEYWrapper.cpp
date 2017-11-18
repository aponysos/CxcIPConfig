#include "stdafx.h"
#include "CxcIPConfig.h"
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

  LSTATUS status = ::RegOpenKeyEx(root, sub.c_str(), 0,
    needWrite ? KEY_WRITE : KEY_READ, &hkey_);
  switch (status)
  {
  case ERROR_SUCCESS:
    break;
  case ERROR_FILE_NOT_FOUND:
    throw FileNotFoundError("RegOpenKeyEx", sub);
  default:
    throw WindowsAPIError(status, "RegOpenKeyEx", sub);
  }

  INFO_LOG() << "RegOpenKeyEx(" << sub << "): success";
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

  DWORD dwType = REG_SZ;
  BYTE lpData[255];
  DWORD len = 255;
  LSTATUS status = ::RegQueryValueEx(hkey_, value.c_str(), NULL,
    &dwType, lpData, &len);
  switch (status)
  {
  case ERROR_SUCCESS:
    break;
  case ERROR_FILE_NOT_FOUND:
    throw FileNotFoundError("RegQueryValueEx", value);
  default:
    throw WindowsAPIError(status, "RegQueryValueEx", value);
  }

  data.assign((LPTSTR)lpData);
  INFO_LOG() << "RegQueryValueEx(" << value << "): " << data;

  return status;
}

long HKEYWrapper::Query(const std::string & value, int & data)
{
  TRACE_FUNC1(value);

  DWORD dwType = REG_DWORD;
  DWORD dwData = 0;
  DWORD len = sizeof(DWORD);
  LSTATUS status = ::RegQueryValueEx(hkey_, value.c_str(), NULL,
    &dwType, ((LPBYTE)&dwData), &len);
  switch (status)
  {
  case ERROR_SUCCESS:
    break;
  case ERROR_FILE_NOT_FOUND:
    throw FileNotFoundError("RegQueryValueEx", value);
  default:
    throw WindowsAPIError(status, "RegQueryValueEx", value);
  }

  data = dwData;
  INFO_LOG() << "RegQueryValueEx(" << value << "): " << data;

  return status;
}

void GetAdaptorsInfo(std::vector<IPAdapterInfo>& adptInfos)
{
  TRACE_FUNC();

  std::vector<IPAdapterInfo> filteredAdptInfos;
  for (auto info : adptInfos) {
    std::ostringstream oss;
    oss << "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\"
      << std::setw(4) << std::setfill('0') << info.index << "\\Ndi\\Interfaces";

    HKEYWrapper hkeyClass;
    try {
      hkeyClass.Open(HKEY_LOCAL_MACHINE, oss.str(), false);
    }
    catch (FileNotFoundError & e) {
      WARN_LOG() << e.what();
      continue;
    }

    hkeyClass.Query("LowerRange", info.type);
    if (info.type.find("ethernet", 0) == std::string::npos) {
      INFO_LOG() << info.desc << "(" << info.index << ") ignored with type: " << info.type;
      continue;
    }

    filteredAdptInfos.push_back(info);
  }

  adptInfos.swap(filteredAdptInfos);
  filteredAdptInfos.clear();

  for (auto info : adptInfos) {
    std::ostringstream oss;
    oss << "SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters\\Interfaces\\"
      << info.name;

    HKEYWrapper hkeyTcpip;
    try {
      hkeyTcpip.Open(HKEY_LOCAL_MACHINE, oss.str(), false);

      int enableDHCP = 0;
      hkeyTcpip.Query("EnableDHCP", enableDHCP);
      info.enableDHCP = (enableDHCP == 1);

      if (info.enableDHCP) {
        hkeyTcpip.Query("DhcpIPAddress", info.ipAddr);
        hkeyTcpip.Query("DhcpSubnetMask", info.ipMask);
      }
      else {
        hkeyTcpip.Query("IPAddress", info.ipAddr);
        hkeyTcpip.Query("SubnetMask", info.ipMask);
        hkeyTcpip.Query("DefaultGateway", info.ipGate);
        hkeyTcpip.Query("NameServer", info.dns);
      }

      filteredAdptInfos.push_back(info);
    }
    catch (FileNotFoundError & e) {
      WARN_LOG() << e.what();
      continue;
    }
  } // for (auto info : adptInfos)

  adptInfos.swap(filteredAdptInfos);
}

} // namespace CxcIPConfig
