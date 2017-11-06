#pragma once

namespace CxcIPConfig
{

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

}
