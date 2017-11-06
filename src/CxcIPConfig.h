#pragma once

namespace CxcIPConfig
{

struct IPAdapterInfo
{
  std::string name; // GUID
  std::string desc; // description
  int index;
  std::string type; // type string
  std::string ipAddr; // ip address
  std::string ipMask; // ip mask
  std::string ipGate; // ip gateway
  bool enableDHCP; // enable DHCP
};

void GetAllAdaptorInfo(std::vector<IPAdapterInfo> & adptInfos);
void GetAllAdaptorInfo2(std::vector<IPAdapterInfo> & adptInfos);
void GetAllAdaptorInfo3(std::vector<IPAdapterInfo> & adptInfos);

} // namespace CxcIPConfig
