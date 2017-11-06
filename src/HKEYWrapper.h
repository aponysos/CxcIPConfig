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
  long Query(const std::string & value, std::string & data);
  long Query(const std::string & value, int & data);

private:
  HKEY hkey_;
};

}
