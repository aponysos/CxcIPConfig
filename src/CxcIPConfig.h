#pragma once

namespace CxcIPConfig
{

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

class WindowsAPIError : public std::runtime_error
{
public:
  WindowsAPIError(long errCode, const std::string & apiName, const std::string & params = "");
  virtual const char * what();
  long GetErrorCode() { return errCode_; }

private:
  std::string apiName_;
  std::string params_;
  long errCode_;
  std::string msg_;
};

template<class T>
inline std::string ToString(const T & val)
{
  std::ostringstream oss;
  oss << val;
  return oss.str();
}

void GetAllAdaptorInfo(std::vector<IPAdapterInfo> & adptInfos);
void GetAllAdaptorInfo2(std::vector<IPAdapterInfo> & adptInfos);
void GetAllAdaptorInfo3(std::vector<IPAdapterInfo> & adptInfos);

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

private:
  HKEY hkey_;
};

} // namespace CxcIPConfig
