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

class WindowsAPIError : public std::runtime_error
{
public:
  WindowsAPIError(const std::string & apiName, long errCode);
  virtual const char * what();

private:
  std::string apiName_;
  long errCode_;
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
  HKEYWrapper(HKEY hkey = NULL);
  ~HKEYWrapper();

  HKEY GetHKEY() {
    return hkey_;
  }

private:
  HKEY hkey_;
};
