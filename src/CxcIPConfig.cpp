#include "stdafx.h"
#include "CxcIPConfig.h"

int main()
{
  log4cpp::Category & cat = log4cpp::Category::getInstance("CxcIPConfig");
  cat.debugStream() << "start";
  return 0;
}
