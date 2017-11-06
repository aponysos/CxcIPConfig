#include "stdafx.h"
#include "Util.h"
#include "WindowsAPIError.h"

namespace CxcIPConfig
{

WindowsAPIError::WindowsAPIError(
  long errCode, const std::string & apiName, const std::string & params)
  : std::runtime_error(""), errCode_(errCode), apiName_(apiName), params_(params)
{
}

std::string WindowsAPIError::GetErrorMsg() const
{
  return ToString(errCode_);
}

const char * WindowsAPIError::what()
{
  //LPTSTR lpMsgBuf;
  //DWORD ret = ::FormatMessage(
  //  FORMAT_MESSAGE_ALLOCATE_BUFFER | 
  //  FORMAT_MESSAGE_FROM_SYSTEM | 
  //  FORMAT_MESSAGE_IGNORE_INSERTS, 
  //  NULL, errCode_, 0, //Default language
  //  lpMsgBuf, 0, NULL);
  if (msg_.empty())
    msg_.append(apiName_).append("(").append(params_).append(")")
      .append(": ").append(GetErrorMsg());
  return msg_.c_str();
}

FileNotFoundError::FileNotFoundError(const std::string & apiName, const std::string & params)
  : WindowsAPIError(ERROR_FILE_NOT_FOUND, apiName, params)
{
}

std::string FileNotFoundError::GetErrorMsg() const
{
  return std::string("(").append(WindowsAPIError::GetErrorMsg())
    .append(") The system cannot find the file specified.");
}

}
