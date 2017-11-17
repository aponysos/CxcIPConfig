#include "stdafx.h"
#include "CxcIPConfig.h"
#include "Util.h"
#include "WindowsAPIError.h"
#include "IphlpApiWrapper.h"
#include "HKEYWrapper.h"

using namespace CxcIPConfig;

Fl_Choice * choice;
void selected(Fl_Widget *, void *)
{
  INFO_LOG() << choice->value() << " selected";
}

int main(int argc, char **argv)
{
  InitLog();

  std::vector<IPAdapterInfo> adptInfos;
  try {
    GetAllAdaptors(adptInfos);
    GetAdaptorsInfo(adptInfos);
  }
  catch (WindowsAPIError & e) {
    ERROR_LOG() << e.what();
    return e.GetErrorCode();
  }

  Fl_Window * window = new Fl_Window(300, 180);
  
  choice = new Fl_Choice(80, 20, 200, 20, "Interface: ");
  choice->add(adptInfos[0].desc.c_str(), NULL, selected);

  Fl_Input * inputIpAddr = new Fl_Input(80, 60, 200, 20, "IP Address: ");
  inputIpAddr->value(adptInfos[0].ipAddr.c_str());

  //Fl_Box * box = new Fl_Box(20, 80, 260, 100, "Hello, World!");
  //box->box(FL_UP_BOX);
  //box->labelsize(36);
  //box->labelfont(FL_BOLD + FL_ITALIC);
  //box->labeltype(FL_SHADOW_LABEL);

  window->end();
  window->show(argc, argv);
  return Fl::run();

  //INFO_LOG() << "CxcIPConfig program end\n";
  //return 0;
}

namespace CxcIPConfig
{

void GetAdaptorsInfo(std::vector<IPAdapterInfo> & adptInfos)
{
  TRACE_FUNC();

  std::vector<IPAdapterInfo> filteredAdptInfos;
  for (auto info : adptInfos) {
    std::ostringstream oss;
    oss << "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\" 
      << std::setw(4) << std::setfill('0') << info.index << "\\Ndi\\Interfaces";

    HKEYWrapper hkeyClass;
    try {
      hkeyClass.Open(HKEY_LOCAL_MACHINE, oss.str(), false);
    }
    catch (FileNotFoundError & e) {
      WARN_LOG() << e.what();
      continue;
    }

    hkeyClass.Query("LowerRange", info.type);
    if (info.type.find("ethernet", 0) == std::string::npos) {
      INFO_LOG() << info.desc << "(" << info.index << ") ignored with type: " << info.type;
      continue;
    }

    filteredAdptInfos.push_back(info);
  }

  adptInfos.swap(filteredAdptInfos);
  filteredAdptInfos.clear();

  for (auto info : adptInfos) {
    std::ostringstream oss;
    oss << "SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters\\Interfaces\\"
      << info.name;

    HKEYWrapper hkeyTcpip;
    try {
      hkeyTcpip.Open(HKEY_LOCAL_MACHINE, oss.str(), false);

      int enableDHCP = 0;
      hkeyTcpip.Query("EnableDHCP", enableDHCP);
      info.enableDHCP = (enableDHCP == 1);

      if (info.enableDHCP) {
        hkeyTcpip.Query("DhcpIPAddress", info.ipAddr);
        hkeyTcpip.Query("DhcpSubnetMask", info.ipMask);
      }
      else {
        hkeyTcpip.Query("IPAddress", info.ipAddr);
        hkeyTcpip.Query("SubnetMask", info.ipMask);
        hkeyTcpip.Query("DefaultGateway", info.ipGate);
        hkeyTcpip.Query("NameServer", info.dns);
      }

      filteredAdptInfos.push_back(info);
    }
    catch (FileNotFoundError & e) {
      WARN_LOG() << e.what();
      continue;
    }
  } // for (auto info : adptInfos)

  adptInfos.swap(filteredAdptInfos);
}

} // namespace CxcIPConfig
