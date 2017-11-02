#include "stdafx.h"
#include "IphlpApiWrapper.h"

void init_net()
{
  errno_t err = NO_ERROR;

  PIP_ADAPTER_INFO pAdapterInfo = NULL;
  IP_ADAPTER_INFO peek;
  ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
  err = ::GetAdaptersInfo(&peek, &ulOutBufLen);
  if (ERROR_BUFFER_OVERFLOW == err) {
    INFO_LOG() << "ulOutBufLen: " << ulOutBufLen;
    pAdapterInfo = new IP_ADAPTER_INFO[ulOutBufLen / sizeof(IP_ADAPTER_INFO)];
  }
  err = ::GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
  if (NO_ERROR == err) {
    PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
    for (; pAdapter; pAdapter = pAdapter->Next) {
      INFO_LOG() << "ComboIndex: " << pAdapter->ComboIndex;
      INFO_LOG() << "AdapterName: " << pAdapter->AdapterName;
      INFO_LOG() << "Description: " << pAdapter->Description;
      INFO_LOG() << "Index: " << pAdapter->Index;
      INFO_LOG() << "Type: " << pAdapter->Type;
      for (PIP_ADDR_STRING pIpAddrString = &pAdapter->IpAddressList;
        pIpAddrString; pIpAddrString = pIpAddrString->Next) {
        INFO_LOG() << "IP Address: " << pIpAddrString->IpAddress.String;
        INFO_LOG() << "IP Mask: " << pIpAddrString->IpMask.String;
        INFO_LOG() << "IP Gateway: " << pAdapter->GatewayList.IpAddress.String;
      }
    } // end of for pAdapter
    delete[] pAdapter;
  }

  delete pAdapterInfo;
}

void GetAllAdaptorInfo(std::vector<IPAdapterInfo>& /*adptInfos*/)
{
  errno_t err = NO_ERROR;

  PIP_ADAPTER_INFO pAdapterInfo = NULL;
  IP_ADAPTER_INFO peek;
  ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
  err = ::GetAdaptersInfo(&peek, &ulOutBufLen);
  if (ERROR_BUFFER_OVERFLOW == err) {
    INFO_LOG() << "ulOutBufLen: " << ulOutBufLen;
    pAdapterInfo = new IP_ADAPTER_INFO[ulOutBufLen / sizeof(IP_ADAPTER_INFO)];
  }
  err = ::GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
  if (NO_ERROR == err) {
    PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
    for (; pAdapter; pAdapter = pAdapter->Next) {
      INFO_LOG() << "ComboIndex: " << pAdapter->ComboIndex;
      INFO_LOG() << "AdapterName: " << pAdapter->AdapterName;
      INFO_LOG() << "Description: " << pAdapter->Description;
      INFO_LOG() << "Index: " << pAdapter->Index;
      INFO_LOG() << "Type: " << pAdapter->Type;
      for (PIP_ADDR_STRING pIpAddrString = &pAdapter->IpAddressList;
        pIpAddrString; pIpAddrString = pIpAddrString->Next) {
        INFO_LOG() << "IP Address: " << pIpAddrString->IpAddress.String;
        INFO_LOG() << "IP Mask: " << pIpAddrString->IpMask.String;
        INFO_LOG() << "IP Gateway: " << pAdapter->GatewayList.IpAddress.String;
      }
    } // end of for pAdapter
    delete[] pAdapter;
  }

  delete pAdapterInfo;
}
