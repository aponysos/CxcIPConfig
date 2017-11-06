#pragma once

namespace CxcIPConfig
{

void InitLog();

template<class T>
inline std::string ToString(const T & val)
{
  std::ostringstream oss;
  oss << val;
  return oss.str();
}

}
