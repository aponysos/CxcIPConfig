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
