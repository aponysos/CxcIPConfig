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
  INFO_LOG() << nNeeded << " IP_ADAPTER_INFO needed";
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
    INFO_LOG() << "AdapterName: " << info.name;
    INFO_LOG() << "Description: " << info.desc;
    INFO_LOG() << "Index: " << info.index;
    
    adptInfos.push_back(info);
  } // end of for pAdapter
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
    catch (WindowsAPIError & e) {
      if (ERROR_FILE_NOT_FOUND == e.GetErrorCode()) {
        WARN_LOG() << e.what();
        continue;
      }
      else throw;
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
    catch (WindowsAPIError & e) {
      if (ERROR_FILE_NOT_FOUND == e.GetErrorCode()) {
        WARN_LOG() << e.what();
        continue;
      }
      else throw;
    }

    try {
      hkeyInterface.Query("IPAddress", info.ipAddr);
      hkeyInterface.Query("SubnetMask", info.ipMask);
      hkeyInterface.Query("DefaultGateway", info.ipGate);
    }
    catch (WindowsAPIError & e) {
      if (ERROR_FILE_NOT_FOUND == e.GetErrorCode()) {
        WARN_LOG() << e.what();
        continue;
      }
      else throw;
    }
  }
}

} // namespace CxcIPConfig
