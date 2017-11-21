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

std::ostream & operator<<(std::ostream & os, const IPAdapterInfo & info);

class AdapterManager
{
public:
  AdapterManager();
  ~AdapterManager();

public:
  void Init();

  const std::vector<IPAdapterInfo> & GetAdapterInfos() const;
  void GetAdapterInfo(size_t index, IPAdapterInfo & info) const;

  void LoadConfig(const std::string & name, IPAdapterInfo & info) const;
  void UpdateConfig(const std::string saveName, const IPAdapterInfo & info);
  void ApplyConfig(const IPAdapterInfo & info) const;

private:
  void LoadAllConfig();
  void SaveAllConfig();

private:
  std::map<std::string, IPAdapterInfo> configInfos_;
  std::vector<IPAdapterInfo> adptInfos_;
};

} // namespace CxcIPConfig
