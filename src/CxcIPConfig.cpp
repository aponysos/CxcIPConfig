#include "stdafx.h"
#include "CxcIPConfig.h"
#include "WindowsAPIError.h"
#include "IphlpApiWrapper.h"
#include "HKEYWrapper.h"

using namespace rapidjson;

namespace CxcIPConfig
{

std::ostream & operator<<(std::ostream & os, const IPAdapterInfo & info)
{
  os << "(" << info.name << '|' << info.desc << '|' << info.index
    << '|' << info.type << info.enableDHCP << '|' << info.ipAddr << '|'
    << info.ipMask << '|' << info.ipGate << '|' << info.dns << '|' << ")";
  return os;
}

AdapterManager::AdapterManager()
{
}

AdapterManager::~AdapterManager()
{
}

void AdapterManager::Init()
{
  TRACE_FUNC();

  LoadAllConfig();

  try {
    GetAllAdaptors(adptInfos_);
    GetAdaptorsInfo(adptInfos_);
    for (auto adptInfo : adptInfos_)
      INFO_LOG() << "IPAdapterInfo: " << adptInfo;
  }
  catch (WindowsAPIError & e) {
    ERROR_LOG() << e.what();
    throw;
  }
}

const std::vector<IPAdapterInfo> & AdapterManager::GetAdapterInfos() const
{
  return adptInfos_;
}

void AdapterManager::GetAdapterInfo(size_t index, IPAdapterInfo & info) const
{
  if (index < adptInfos_.size())
    info = adptInfos_[index];
}

void AdapterManager::LoadConfig(const std::string & name, IPAdapterInfo & info) const
{
  auto i = configInfos_.find(name);
  if (i != configInfos_.end())
    info = i->second;
  INFO_LOG() << "LoadConfig " << name << " : " << info;
}

void AdapterManager::UpdateConfig(const std::string saveName, const IPAdapterInfo & info)
{
  configInfos_[saveName] = info;
  INFO_LOG() << "UpdateConfig " << saveName << " : " << info;

  SaveAllConfig();
}

void AdapterManager::ApplyConfig(const IPAdapterInfo & info) const
{
  TRACE_FUNC();

  try {
    SetAdaptorInfo(info);
  }
  catch (WindowsAPIError & e) {
    ERROR_LOG() << e.what();
    throw;
  }

  INFO_LOG() << "config applied: " << info;
}

void AdapterManager::LoadAllConfig()
{
  TRACE_FUNC();

  std::ifstream ifs("out.json");
  std::string json;
  ifs >> json;

  INFO_LOG() << "config json: " << json;

  Document d;
  d.Parse(json.c_str());
  if (!d.IsArray())
    return;

  for (Value * pv = d.Begin(); pv != d.End(); ++pv) {
    Value & v = *pv;
    std::string name = v["name"].GetString();
    IPAdapterInfo info;
    info.ipAddr = v["ipAddr"].GetString();
    info.ipMask = v["ipMask"].GetString();
    info.ipGate = v["ipGate"].GetString();
    info.dns = v["dns"].GetString();
    info.enableDHCP = v["dhcp"].GetBool();

    configInfos_[name] = info;
    INFO_LOG() << name << " config loaded: " << info;
  }
}

void AdapterManager::SaveAllConfig()
{
  TRACE_FUNC();

  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  writer.StartArray();
  for (auto ci : configInfos_) {
    writer.StartObject();
    writer.Key("name");
    writer.String(ci.first);
    IPAdapterInfo & info = ci.second;
    writer.Key("ipAddr");
    writer.String(info.ipAddr);
    writer.Key("ipMask");
    writer.String(info.ipMask);
    writer.Key("ipGate");
    writer.String(info.ipGate);
    writer.Key("dns");
    writer.String(info.dns);
    writer.Key("dhcp");
    writer.Bool(info.enableDHCP);
    writer.EndObject();
  }
  writer.EndArray();

  std::ofstream ofs("out.json");
  ofs << buffer.GetString();
  INFO_LOG() << "config saved: " << buffer.GetString();
}

} // namespace CxcIPConfig
