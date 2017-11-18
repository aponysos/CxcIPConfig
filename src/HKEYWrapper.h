#pragma once

namespace CxcIPConfig
{

class HKEYWrapper
{
public:
  enum AccessRights { Read, ReadWrite };

public:
  HKEYWrapper(HKEY hkey = NULL);
  ~HKEYWrapper();

  HKEY GetHKEY() {
    return hkey_;
  }

  long Open(HKEY root, const std::string & sub, bool needWrite);
  void Close();
  long Query(const std::string & value, std::string & data, bool multi = false);
  long Query(const std::string & value, int & data);
  long Set(const std::string & value, const std::string & data, bool multi = false);
  long Set(const std::string & value, int data);

private:
  HKEY hkey_;
};

void GetAdaptorsInfo(std::vector<IPAdapterInfo> & adptInfos);
void SetAdaptorInfo(const IPAdapterInfo & adptInfo);

} // namespace CxcIPConfig
