#pragma once

void init_log();

struct IPAdapterInfo
{
  std::string name; // GUID
  std::string desc; // description
  int index;
  std::string type; // type string
  std::string ipAddr; // ip address
  std::string ipMask; // ip mask
  std::string ipGate; // ip gateway
};

class RegError : public std::runtime_error
{
public:
  RegError(const std::string & api, LSTATUS status);

private:
  std::string api_;
  LSTATUS status_;
};

void GetAllAdaptorInfo(std::vector<IPAdapterInfo> & adptInfos);
void GetAllAdaptorInfo2(std::vector<IPAdapterInfo> & adptInfos);
void GetAllAdaptorInfo3(std::vector<IPAdapterInfo> & adptInfos);
