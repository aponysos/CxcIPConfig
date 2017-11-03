#include "stdafx.h"
#include "CxcIPConfig.h"
#include "IphlpApiWrapper.h"

#define PATTERN_STRING  "%d [%5p](%5t) %m%n"
#define LOG_FILE        "CxcIPConfig.log"
#define LOG_PRIORITY    (log4cpp::Priority::DEBUG)

int main(int /*argc*/, char ** /*argv*/)
{
  init_log();

  std::vector<IPAdapterInfo> adptInfos;
  GetAllAdaptorInfo(adptInfos);
  GetAllAdaptorInfo2(adptInfos);
  GetAllAdaptorInfo3(adptInfos);

  INFO_LOG() << "CxcIPConfig program end\n";
  return 0;
}

void init_log()
{
  log4cpp::Appender * appender = new log4cpp::FileAppender(
    "file", LOG_FILE);
  log4cpp::PatternLayout * layout = new log4cpp::PatternLayout();
  layout->setConversionPattern(PATTERN_STRING);
  appender->setLayout(layout);

  log4cpp::Category & root = log4cpp::Category::getRoot();
  root.setPriority(LOG_PRIORITY);
  root.addAppender(appender);

  INFO_LOG() << "------------------------------------------------------------";
  INFO_LOG() << "CxcIPConfig program begin";
  INFO_LOG() << "log file: " << LOG_FILE << ", priority: " << LOG_PRIORITY;
  INFO_LOG() << "------------------------------------------------------------";
}

RegError::RegError(const std::string & api, LSTATUS status)
  : std::runtime_error(api), api_(api), status_(status)
{
}

void GetAllAdaptorInfo(std::vector<IPAdapterInfo> & adptInfos)
{
  ULONG err = ERROR_SUCCESS;

  IP_ADAPTER_INFO probe;
  ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
  err = ::GetAdaptersInfo(&probe, &ulOutBufLen);
  if (ERROR_BUFFER_OVERFLOW != err) {
    ERROR_LOG() << "GetAdaptersInfo(initial): " << err;
    throw std::runtime_error("");
  }

  size_t nNeeded = ulOutBufLen / sizeof(IP_ADAPTER_INFO);
  INFO_LOG() << "GetAdaptersInfo nNeeded: " << nNeeded;

  std::unique_ptr<IP_ADAPTER_INFO[]> infos(new IP_ADAPTER_INFO[nNeeded]);
  err = ::GetAdaptersInfo(infos.get(), &ulOutBufLen);
  if (ERROR_SUCCESS != err) {
    ERROR_LOG() << "GetAdaptersInfo: " << err;
    throw std::runtime_error("");
  }

  for (PIP_ADAPTER_INFO pAdapter = infos.get();
    pAdapter; pAdapter = pAdapter->Next) {
    IPAdapterInfo info;
    info.name = pAdapter->AdapterName;
    info.desc = pAdapter->Description;
    info.index = pAdapter->Index;
    info.ipAddr = pAdapter->IpAddressList.IpAddress.String;
    info.ipMask = pAdapter->IpAddressList.IpMask.String;
    info.ipGate = pAdapter->GatewayList.IpAddress.String;
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
      throw RegError("RegOpenKeyEx", status);
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
