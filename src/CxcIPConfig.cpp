#include "stdafx.h"
#include "CxcIPConfig.h"
#include "IphlpApiWrapper.h"

#define PATTERN_STRING  "%d [%5p]{%5t} %m%n"
#define LOG_FILE        "CxcIPConfig.log"
#define LOG_PRIORITY    (log4cpp::Priority::DEBUG)

int main(int /*argc*/, char ** /*argv*/)
{
  init_log();

  std::vector<IPAdapterInfo> adptInfos;
  GetAllAdaptorInfo(adptInfos);
  GetAllAdaptorInfo2(adptInfos);

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

void GetAllAdaptorInfo2(std::vector<IPAdapterInfo>& adptInfos)
{
  LSTATUS status = ERROR_SUCCESS;
  for (auto info : adptInfos) {
    HKEY hkeyInterface;
    std::ostringstream oss;
    oss << "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\" 
      << std::setw(4) << std::setfill('0') << info.index << "\\Ndi\\Interfaces";
    INFO_LOG() << oss.str();
    status = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
      oss.str().c_str(), 0, KEY_ALL_ACCESS, &hkeyInterface);
    if (ERROR_SUCCESS != status) {
      ::RegCloseKey(hkeyInterface);
      if (ERROR_FILE_NOT_FOUND == status) {
        INFO_LOG() << "RegOpenKeyEx: ERROR_FILE_NOT_FOUND";
        continue;
      }
      ERROR_LOG() << "RegOpenKeyEx: " << status;
      throw RegError("RegOpenKeyEx", status);
    }
    INFO_LOG() << "RegOpenKeyEx: ERROR_SUCCESS";
    ::RegCloseKey(hkeyInterface);
  }
}
