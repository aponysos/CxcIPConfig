#include "stdafx.h"
#include "CxcIPConfig.h"

#define PATTERN_STRING  "%d [%5p]{%5t} %m%n"
#define LOG_FILE        "CxcIPConfig.log"
#define LOG_PRIORITY    (log4cpp::Priority::DEBUG)

void init_log();
void init_net();

int main(int /*argc*/, char ** /*argv*/)
{
  init_log();
  INFO_LOG() << "CxcIPConfig program begin";

  init_net();

  //QApplication app(argc, argv);

  //QLabel *label = new QLabel("Hello, world");
  //label->show();

  //return app.exec();
  INFO_LOG() << "CxcIPConfig program end";
  INFO_LOG() << "------------------------------------------------------------\n";
  return 0;
}

void init_log()
{
  log4cpp::Appender * appender = new log4cpp::FileAppender(
    "file", LOG_FILE);
  log4cpp::PatternLayout * layout = new log4cpp::PatternLayout();
  layout->setConversionPattern(PATTERN_STRING);
  appender->setLayout(layout);

  log4cpp::Appender * appender1 = new log4cpp::OstreamAppender(
    "console", &std::cout);
  appender1->setLayout(new log4cpp::BasicLayout());

  log4cpp::Category & root = log4cpp::Category::getRoot();
  root.setPriority(LOG_PRIORITY);
  root.addAppender(appender);
  root.addAppender(appender1);

  INFO_LOG() << "------------------------------------------------------------";
  INFO_LOG() << "log file: " << LOG_FILE << ", priority: " << LOG_PRIORITY;
}

void init_net()
{
  errno_t err = NO_ERROR;

  PIP_ADAPTER_INFO pAdapterInfo = NULL;
  IP_ADAPTER_INFO peek;
  ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
  err = ::GetAdaptersInfo(&peek, &ulOutBufLen);
  if (ERROR_BUFFER_OVERFLOW == err) {
    INFO_LOG() << "ulOutBufLen: " << ulOutBufLen;
    pAdapterInfo = new IP_ADAPTER_INFO[ulOutBufLen / sizeof(IP_ADAPTER_INFO)];
  }
  err = ::GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
  if (NO_ERROR == err) {
    PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
    for (; pAdapter; pAdapter = pAdapter->Next) {
      INFO_LOG() << "ComboIndex: " << pAdapter->ComboIndex;
      INFO_LOG() << "AdapterName: " << pAdapter->AdapterName;
      INFO_LOG() << "Description: " << pAdapter->Description;
      INFO_LOG() << "Index: " << pAdapter->Index;
      INFO_LOG() << "Type: " << pAdapter->Type;
      for (UINT i = 0; i < pAdapter->AddressLength; i++) {
        INFO_LOG() << (int)pAdapter->Address[i];
      }
      INFO_LOG() << "IP Address: " << pAdapter->IpAddressList.IpAddress.String;
      INFO_LOG() << "IP Mask: " << pAdapter->IpAddressList.IpMask.String;
      INFO_LOG() << "IP tGateway: " << pAdapter->GatewayList.IpAddress.String;
    } // end of for pAdapter
    delete[] pAdapter;
  }

  delete pAdapterInfo;
}
