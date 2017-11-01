#include "stdafx.h"
#include "CxcIPConfig.h"

#define PATTERN_STRING  "%d [%5p]{%5t} %m%n"
#define LOG_FILE        "CxcIPConfig.log"
#define LOG_PRIORITY    (log4cpp::Priority::DEBUG)

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

  INFO_LOG() << "log file: " << LOG_FILE << ", priority: " << LOG_PRIORITY;
}

int main()
{
  init_log();
  INFO_LOG() << "CxcIPConfig program begin";
  INFO_LOG() << "CxcIPConfig program end";
  return 0;
}
