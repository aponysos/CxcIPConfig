#pragma once

void init_net();

struct IPAdapterInfo
{
  std::string name; // GUID
  std::string desc; // description
  int index;
  std::string ipAddr; // ip address
  std::string ipMask; // ip mask
  std::string ipGate; // ip gateway
};

void GetAllAdaptorInfo(std::vector<IPAdapterInfo> & adptInfos);
