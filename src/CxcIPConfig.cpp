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
  const int LOG_PRIORITY = log4cpp::Priority::DEBUG;

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

WindowsAPIError::WindowsAPIError(const std::string & apiName, long errCode)
  : std::runtime_error(""), apiName_(apiName), errCode_(errCode)
{
}

const char * WindowsAPIError::what()
{
  std::string msg;
  msg.append(apiName_).append(" : ");
  //LPTSTR lpMsgBuf;
  //DWORD ret = ::FormatMessage(
  //  FORMAT_MESSAGE_ALLOCATE_BUFFER | 
  //  FORMAT_MESSAGE_FROM_SYSTEM | 
  //  FORMAT_MESSAGE_IGNORE_INSERTS, 
  //  NULL, errCode_, 0, //Default language
  //  lpMsgBuf, 0, NULL);
  msg.append("(");
  msg.append(ToString(errCode_));
  msg.append(")");
  return msg.c_str();
}

void GetAllAdaptorInfo(std::vector<IPAdapterInfo> & adptInfos)
{
  ULONG err = ERROR_SUCCESS;

  IP_ADAPTER_INFO probe;
  ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
  err = ::GetAdaptersInfo(&probe, &ulOutBufLen);
  if (ERROR_BUFFER_OVERFLOW != err)
    throw WindowsAPIError("GetAdaptersInfo(probe)", err);

  size_t nNeeded = ulOutBufLen / sizeof(IP_ADAPTER_INFO);
  INFO_LOG() << nNeeded << " IP_ADAPTER_INFO needed";

  std::unique_ptr<IP_ADAPTER_INFO[]> infos(new IP_ADAPTER_INFO[nNeeded]);
  err = ::GetAdaptersInfo(infos.get(), &ulOutBufLen);
  if (ERROR_SUCCESS != err)
    throw WindowsAPIError("GetAdaptersInfo", err);

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
  LSTATUS status = ERROR_SUCCESS;
  for (int i = 0; i < adptInfos.size(); ++i) {
    IPAdapterInfo & info = adptInfos[i];
    std::ostringstream oss;
    oss << "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\" 
      << std::setw(4) << std::setfill('0') << info.index << "\\Ndi\\Interfaces";
    DEBUG_LOG() << oss.str();
    HKEY hkeyInterface;
    status = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
      oss.str().c_str(), 0, KEY_READ, &hkeyInterface);
    if (ERROR_SUCCESS != status) {
      ::RegCloseKey(hkeyInterface);
      if (ERROR_FILE_NOT_FOUND == status) {
        INFO_LOG() << "RegOpenKeyEx: ERROR_FILE_NOT_FOUND";
        continue;
      }
      ERROR_LOG() << "RegOpenKeyEx: " << status;
      throw WindowsAPIError("RegOpenKeyEx", status);
    }
    DEBUG_LOG() << "RegOpenKeyEx: ERROR_SUCCESS";

    DWORD len = 255;
    BYTE lpData[255];
    ::RegQueryValueEx(hkeyInterface, "LowerRange", NULL, NULL, lpData, &len);
    info.type.assign((LPTSTR)lpData);
    INFO_LOG() << "type: " << info.type;
    ::RegCloseKey(hkeyInterface);
  }
}

void GetAllAdaptorInfo3(std::vector<IPAdapterInfo> & adptInfos)
{
  LSTATUS status = ERROR_SUCCESS;
  for (auto info : adptInfos) {
    if (info.type.find("ethernet", 0) == std::string::npos) {
      INFO_LOG() << info.type << " ignored";
      continue;
    }
    std::ostringstream oss;
    oss << "SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters\\Interfaces\\"
      << info.name;
    DEBUG_LOG() << oss.str();
    HKEY hkeyInterface;
    status = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
      oss.str().c_str(), 0, KEY_READ, &hkeyInterface);
    if (ERROR_SUCCESS != status) {
      ::RegCloseKey(hkeyInterface);
      ERROR_LOG() << "RegOpenKeyEx: " << status;
      continue;
    }

    DWORD len = 255;
    BYTE lpData[255];
    status = ::RegQueryValueEx(hkeyInterface, "IPAddress", NULL, NULL, lpData, &len);
    if (ERROR_SUCCESS != status)
      ERROR_LOG() << "RegQueryValueEx: " << status;
    else
      info.ipAddr.assign((LPTSTR)lpData);
    INFO_LOG() << "ipAddr: " << info.ipAddr;
    status = ::RegQueryValueEx(hkeyInterface, "SubnetMask", NULL, NULL, lpData, &len);
    if (ERROR_SUCCESS != status)
      ERROR_LOG() << "RegQueryValueEx: " << status;
    else
      info.ipMask.assign((LPTSTR)lpData);
    INFO_LOG() << "ipMask: " << info.ipMask;
    status = ::RegQueryValueEx(hkeyInterface, "DefaultGateway", NULL, NULL, lpData, &len);
    if (ERROR_SUCCESS != status)
      ERROR_LOG() << "RegQueryValueEx: " << status;
    else
      info.ipGate.assign((LPTSTR)lpData);
    INFO_LOG() << "ipGate: " << info.ipGate;

    ::RegCloseKey(hkeyInterface);
  }
}

HKEYWrapper::HKEYWrapper(HKEY hkey)
  : hkey_(hkey)
{
}

HKEYWrapper::~HKEYWrapper()
{
  ::RegCloseKey(hkey_);
}

} // namespace CxcIPConfig
