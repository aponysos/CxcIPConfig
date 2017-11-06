#include "stdafx.h"
#include "CxcIPConfig.h"
#include "Util.h"
#include "WindowsAPIError.h"
#include "HKEYWrapper.h"

using namespace CxcIPConfig;

int main(int /*argc*/, char ** /*argv*/)
{
  InitLog();

  std::vector<IPAdapterInfo> adptInfos;
  try {
    GetAllAdaptorInfo(adptInfos);
    GetAllAdaptorInfo2(adptInfos);
    GetAllAdaptorInfo3(adptInfos);
  }
  catch (WindowsAPIError & e) {
    ERROR_LOG() << e.what();
  }

  INFO_LOG() << "CxcIPConfig program end\n";
  return 0;
}

namespace CxcIPConfig
{

void GetAllAdaptorInfo(std::vector<IPAdapterInfo> & adptInfos)
{
  TRACE_FUNC();

  ULONG err = ERROR_SUCCESS;
  ULONG ulOutBufLen = 0;
  
  err = ::GetAdaptersInfo(NULL, &ulOutBufLen);
  if (ERROR_BUFFER_OVERFLOW != err)
    throw WindowsAPIError(err, "GetAdaptersInfo(probe)");

  size_t nNeeded = ulOutBufLen / sizeof(IP_ADAPTER_INFO);
  INFO_LOG() << "GetAdaptersInfo(probe): " << nNeeded << " IP_ADAPTER_INFO needed";
  std::unique_ptr<IP_ADAPTER_INFO[]> infos(new IP_ADAPTER_INFO[nNeeded]);
  
  err = ::GetAdaptersInfo(infos.get(), &ulOutBufLen);
  if (ERROR_SUCCESS != err)
    throw WindowsAPIError(err, "GetAdaptersInfo");

  for (PIP_ADAPTER_INFO pAdapter = infos.get();
    pAdapter; pAdapter = pAdapter->Next) {
    IPAdapterInfo info;
    info.name = pAdapter->AdapterName;
    info.desc = pAdapter->Description;
    info.index = pAdapter->Index;
    INFO_LOG() << info.name << "|" << info.desc << "|" << info.index;
    
    adptInfos.push_back(info);
  } // for pAdapter
}

void GetAllAdaptorInfo2(std::vector<IPAdapterInfo> & adptInfos)
{
  TRACE_FUNC();

  long status = ERROR_SUCCESS;
  for (int i = 0; i < adptInfos.size(); ++i) {
    IPAdapterInfo & info = adptInfos[i];

    std::ostringstream oss;
    oss << "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\" 
      << std::setw(4) << std::setfill('0') << info.index << "\\Ndi\\Interfaces";

    HKEYWrapper hkeyInterface;
    try {
      status = hkeyInterface.Open(HKEY_LOCAL_MACHINE, oss.str(), false);
    }
    catch (FileNotFoundError & e) {
      WARN_LOG() << e.what();
      continue;
    }

    hkeyInterface.Query("LowerRange", info.type);
  }
}

void GetAllAdaptorInfo3(std::vector<IPAdapterInfo> & adptInfos)
{
  TRACE_FUNC();

  for (auto info : adptInfos) {
    if (info.type.find("ethernet", 0) == std::string::npos) {
      INFO_LOG() << info.desc << " ignored with type: " << info.type ;
      continue;
    }

    std::ostringstream oss;
    oss << "SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters\\Interfaces\\"
      << info.name;

    HKEYWrapper hkeyInterface;
    try {
      hkeyInterface.Open(HKEY_LOCAL_MACHINE, oss.str(), false);
    }
    catch (FileNotFoundError & e) {
      WARN_LOG() << e.what();
      continue;
    }

    try {
      int enableDHCP = -1;
      hkeyInterface.Query("EnableDHCP", enableDHCP);
      info.enableDHCP = (enableDHCP == 1);
    }
    catch (FileNotFoundError & e) {
      WARN_LOG() << e.what();
      continue;
    }

    try {
      if (info.enableDHCP) {
        hkeyInterface.Query("DhcpIPAddress", info.ipAddr);
        hkeyInterface.Query("DhcpSubnetMask", info.ipMask);
      }
      else {
        hkeyInterface.Query("IPAddress", info.ipAddr);
        hkeyInterface.Query("SubnetMask", info.ipMask);
        hkeyInterface.Query("DefaultGateway", info.ipGate);
      }
    }
    catch (FileNotFoundError & e) {
      WARN_LOG() << e.what();
      continue;
    }
  } // for (auto info : adptInfos)
}

} // namespace CxcIPConfig
