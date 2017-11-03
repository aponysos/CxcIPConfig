#include "stdafx.h"
#include "IphlpApiWrapper.h"

using namespace std;

void GetAllAdaptorInfo(std::vector<IPAdapterInfo>& adptInfos)
{
  ULONG err = NO_ERROR;

  IP_ADAPTER_INFO probe;
  ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
  err = ::GetAdaptersInfo(&probe, &ulOutBufLen);
  if (ERROR_BUFFER_OVERFLOW != err) {
    ERROR_LOG() << "GetAdaptersInfo(initial): " << err;
    throw std::logic_error("");
  }

  size_t nNeeded = ulOutBufLen / sizeof(IP_ADAPTER_INFO);
  INFO_LOG() << "GetAdaptersInfo nNeeded: " << nNeeded;

  std::unique_ptr<IP_ADAPTER_INFO[]> infos(new IP_ADAPTER_INFO[nNeeded]);
  err = ::GetAdaptersInfo(infos.get(), &ulOutBufLen);
  if (NO_ERROR != err) {
    ERROR_LOG() << "GetAdaptersInfo: " << err;
    throw std::logic_error("");
  }

  for (PIP_ADAPTER_INFO pAdapter = infos.get();
    pAdapter; pAdapter = pAdapter->Next) {
    IPAdapterInfo info;
    info.name   = pAdapter->AdapterName;
    info.desc   = pAdapter->Description;
    info.index  = pAdapter->Index;
    info.ipAddr = pAdapter->IpAddressList.IpAddress.String;
    info.ipMask = pAdapter->IpAddressList.IpMask.String;
    info.ipGate = pAdapter->GatewayList.IpAddress.String;
    INFO_LOG() << "AdapterName: " << info.name;
    INFO_LOG() << "Description: " << info.desc;
    INFO_LOG() << "Index: " << info.index;
    adptInfos.push_back(info);
  } // end of for pAdapter
}
