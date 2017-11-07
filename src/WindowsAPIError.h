#pragma once

namespace CxcIPConfig
{

class WindowsAPIError : public std::runtime_error
{
public:
  WindowsAPIError(long errCode, const std::string & apiName, const std::string & params = "");
  virtual const char * what();
  long GetErrorCode() const { return errCode_; }
  virtual std::string GetErrorMsg() const;

private:
  std::string apiName_;
  std::string params_;
  long errCode_;
  std::string msg_;
};

class FileNotFoundError : public WindowsAPIError
{
public:
  FileNotFoundError(const std::string & apiName, const std::string & params = "");
  virtual std::string GetErrorMsg() const;
};

} // namespace CxcIPConfig
