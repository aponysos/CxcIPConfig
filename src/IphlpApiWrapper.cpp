#include "stdafx.h"
#include "CxcIPConfig.h"
#include "WindowsAPIError.h"
#include "IphlpApiWrapper.h"

namespace CxcIPConfig
{

void GetAllAdaptors(std::vector<IPAdapterInfo> & adptInfos)
{
  TRACE_FUNC();

  ULONG err = ERROR_SUCCESS;
  ULONG ulOutBufLen = 0;

  err = ::GetAdaptersInfo(NULL, &ulOutBufLen);
  if (ERROR_BUFFER_OVERFLOW != err)
    throw WindowsAPIError(err, "GetAdaptersInfo", "probe");

  size_t nNeeded = ulOutBufLen / sizeof(IP_ADAPTER_INFO);
  std::unique_ptr<IP_ADAPTER_INFO[]> infos(new IP_ADAPTER_INFO[nNeeded]);
  INFO_LOG() << "GetAdaptersInfo(probe): " << nNeeded << " IP_ADAPTER_INFO needed";

  err = ::GetAdaptersInfo(infos.get(), &ulOutBufLen);
  if (ERROR_SUCCESS != err)
    throw WindowsAPIError(err, "GetAdaptersInfo");

  for (PIP_ADAPTER_INFO pAdapter = infos.get();
    pAdapter; pAdapter = pAdapter->Next) {
    IPAdapterInfo info;
    info.name = pAdapter->AdapterName;
    info.desc = pAdapter->Description;
    info.index = pAdapter->Index;

    adptInfos.push_back(info);
    INFO_LOG() << info.name << "|" << info.desc << "|" << info.index;
  } // for pAdapter
}

} // namespace CxcIPConfig
