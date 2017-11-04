#pragma once

#include "log4cpp\Category.hh"
#include "log4cpp\OstreamAppender.hh"
#include "log4cpp\FileAppender.hh"
#include "log4cpp\PatternLayout.hh"

#define ROOT_LOG()  (log4cpp::Category::getRoot())
#define DEBUG_LOG() (ROOT_LOG().debugStream())
#define INFO_LOG()  (ROOT_LOG().infoStream())
#define WARN_LOG()  (ROOT_LOG().warnStream())
#define ERROR_LOG() (ROOT_LOG().errorStream())

namespace log4cpp
{

class TraceFunction
{
public:
  TraceFunction(const char * funcName)
    : funcName_(funcName) {
    DEBUG_LOG() << "ENTER " << funcName_;
  }
  ~TraceFunction() {
    DEBUG_LOG() << "LEAVE " << funcName_;
  }

private:
  const char * funcName_;
};

}

#define TRACE_FUNC() log4cpp::TraceFunction traceFunction_(__FUNCTION__)
#define TRACE_FUNC1(param1) \
  TRACE_FUNC(); \
  DEBUG_LOG() << #param1 << " = " << param1
#define TRACE_FUNC2(param1, param2) \
  TRACE_FUNC1(param1); \
  DEBUG_LOG() << #param2 << " = " << param2
