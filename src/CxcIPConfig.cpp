#include "stdafx.h"
#include "CxcIPConfig.h"

using namespace CxcIPConfig;

int main(int /*argc*/, char ** /*argv*/)
{
  init_log();

  std::vector<IPAdapterInfo> adptInfos;
  try {
    GetAllAdaptorInfo(adptInfos);
    GetAllAdaptorInfo2(adptInfos);
    GetAllAdaptorInfo3(adptInfos);
  }
  catch (WindowsAPIError & e) {
    ERROR_LOG() << e.what();
  }

  INFO_LOG() << "CxcIPConfig program end\n";
  return 0;
}

namespace CxcIPConfig
{

  void init_log()
{
  const char * PATTERN_STRING = "%d [%5p](%5t) %m%n";
  const char * LOG_FILE = "CxcIPConfig.log";
  const int LOG_PRIORITY = log4cpp::Priority::INFO;

  log4cpp::PatternLayout * layout = new log4cpp::PatternLayout();
  layout->setConversionPattern(PATTERN_STRING);

  log4cpp::Appender * appender = new log4cpp::FileAppender(
    "file", LOG_FILE);
  appender->setLayout(layout);

  log4cpp::Category & root = log4cpp::Category::getRoot();
  root.setPriority(LOG_PRIORITY);
  root.addAppender(appender);

  INFO_LOG() << "------------------------------------------------------------";
  INFO_LOG() << "CxcIPConfig program begin";
  INFO_LOG() << "log file: " << LOG_FILE << ", priority: " << LOG_PRIORITY;
  INFO_LOG() << "------------------------------------------------------------";
}

WindowsAPIError::WindowsAPIError(
  long errCode, const std::string & apiName, const std::string & params)
  : std::runtime_error(""), errCode_(errCode), apiName_(apiName), params_(params)
{
  msg_.append(apiName_).append("(").append(params_).append(")")
    .append(" : ").append(ToString(errCode_));
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
  return msg_.c_str();
}

void GetAllAdaptorInfo(std::vector<IPAdapterInfo> & adptInfos)
{
  TRACE_FUNC();

  ULONG err = ERROR_SUCCESS;
  ULONG ulOutBufLen = 0;
  
  err = ::GetAdaptersInfo(NULL, &ulOutBufLen);
  if (ERROR_BUFFER_OVERFLOW != err)
    throw WindowsAPIError(err, "GetAdaptersInfo(probe)");

  size_t nNeeded = ulOutBufLen / sizeof(IP_ADAPTER_INFO);
  INFO_LOG() << nNeeded << " IP_ADAPTER_INFO needed";
  std::unique_ptr<IP_ADAPTER_INFO[]> infos(new IP_ADAPTER_INFO[nNeeded]);
  
  err = ::GetAdaptersInfo(infos.get(), &ulOutBufLen);
  if (ERROR_SUCCESS != err)
    throw WindowsAPIError(err, "GetAdaptersInfo");

  for (PIP_ADAPTER_INFO pAdapter = infos.get();
    pAdapter; pAdapter = pAdapter->Next) {
    IPAdapterInfo info;
    info.name = pAdapter->AdapterName;
    info.desc = pAdapter->Description;
    info.index = pAdapter->Index;
    INFO_LOG() << "AdapterName: " << info.name;
    INFO_LOG() << "Description: " << info.desc;
    INFO_LOG() << "Index: " << info.index;
    
    adptInfos.push_back(info);
  } // end of for pAdapter
}

void GetAllAdaptorInfo2(std::vector<IPAdapterInfo> & adptInfos)
{
  TRACE_FUNC();

  long status = ERROR_SUCCESS;
  for (int i = 0; i < adptInfos.size(); ++i) {
    IPAdapterInfo & info = adptInfos[i];

    std::ostringstream oss;
    oss << "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\" 
      << std::setw(4) << std::setfill('0') << info.index << "\\Ndi\\Interfaces";

    HKEYWrapper hkeyInterface;
    try {
      status = hkeyInterface.Open(HKEY_LOCAL_MACHINE, oss.str(), false);
    }
    catch (WindowsAPIError & e) {
      if (ERROR_FILE_NOT_FOUND == e.GetErrorCode()) {
        WARN_LOG() << "RegOpenKeyEx: ERROR_FILE_NOT_FOUND";
        continue;
      }
      else throw;
    }

    hkeyInterface.Query("LowerRange", info.type);
  }
}

void GetAllAdaptorInfo3(std::vector<IPAdapterInfo> & adptInfos)
{
  TRACE_FUNC();

  for (auto info : adptInfos) {
    if (info.type.find("ethernet", 0) == std::string::npos) {
      INFO_LOG() << info.desc << " ignored with type: " << info.type ;
      continue;
    }

    std::ostringstream oss;
    oss << "SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters\\Interfaces\\"
      << info.name;

    HKEYWrapper hkeyInterface;
    try {
      hkeyInterface.Open(HKEY_LOCAL_MACHINE, oss.str(), false);
    }
    catch (WindowsAPIError & e) {
      if (ERROR_FILE_NOT_FOUND == e.GetErrorCode()) {
        WARN_LOG() << e.what();
        continue;
      }
      else throw;
    }

    try {
      hkeyInterface.Query("IPAddress", info.ipAddr);
      hkeyInterface.Query("SubnetMask", info.ipMask);
      hkeyInterface.Query("DefaultGateway", info.ipGate);
    }
    catch (WindowsAPIError & e) {
      if (ERROR_FILE_NOT_FOUND == e.GetErrorCode()) {
        WARN_LOG() << e.what();
        continue;
      }
      else throw;
    }
  }
}

HKEYWrapper::HKEYWrapper(HKEY hkey)
  : hkey_(hkey)
{
}

HKEYWrapper::~HKEYWrapper()
{
  Close();
}

long HKEYWrapper::Open(HKEY root, const std::string & sub, bool needWrite)
{
  TRACE_FUNC1(sub);

  Close();

  LRESULT status = ::RegOpenKeyEx(root, sub.c_str(), 0, 
    needWrite ? KEY_WRITE : KEY_READ, &hkey_);
  if (ERROR_SUCCESS != status)
    throw WindowsAPIError(status, "RegOpenKeyEx", sub);

  INFO_LOG() << "RegOpenKeyEx: " << sub << " opened";
  return status;
}

void HKEYWrapper::Close()
{
  if (NULL != hkey_)
    ::RegCloseKey(hkey_);
  hkey_ = NULL;
}

long HKEYWrapper::Query(const std::string & value, std::string & data)
{
  TRACE_FUNC1(value);

  DWORD len = 255;
  BYTE lpData[255];
  LRESULT status = ::RegQueryValueEx(hkey_, value.c_str(), NULL, NULL, lpData, &len);
  if (ERROR_SUCCESS != status)
    throw WindowsAPIError(status, "RegQueryValueEx", value);

  data.assign((LPTSTR)lpData);
  INFO_LOG() << "RegQueryValueEx: " << value << " = " << data;

  return status;
}

} // namespace CxcIPConfig
