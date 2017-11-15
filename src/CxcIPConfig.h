#pragma once

namespace CxcIPConfig
{

struct IPAdapterInfo
{
  std::string name; // GUID
  std::string desc; // description
  int index;
  std::string type; // type string
  bool enableDHCP; // enable DHCP
  std::string ipAddr; // ip address
  std::string ipMask; // ip mask
  std::string ipGate; // ip gateway
  std::string dns; // dns

  IPAdapterInfo() : index(0), enableDHCP(false) {}
};

void GetAdaptorsInfo(std::vector<IPAdapterInfo> & adptInfos);

} // namespace CxcIPConfig
