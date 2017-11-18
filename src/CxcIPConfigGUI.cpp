#include "stdafx.h"
#include "CxcIPConfig.h"
#include "CxcIPConfigGUI.h"
#include "WindowsAPIError.h"
#include "IphlpApiWrapper.h"
#include "HKEYWrapper.h"

using namespace rapidjson;

namespace CxcIPConfig
{

MainWindow::MainWindow(int w, int h, const char * t)
  : Fl_Window(w, h, t)
{
}

MainWindow::~MainWindow()
{
}

void MainWindow::init()
{
  TRACE_FUNC();

  choiceInterface_ = new Fl_Choice(80, 20, 200, 20, "Interface: ");
  input_ipAddr_ = new Fl_Input(80, 60, 200, 20, "IP Address: ");
  input_ipMask_ = new Fl_Input(80, 80, 200, 20, "IP Mask: ");
  input_ipGate_ = new Fl_Input(80, 100, 200, 20, "IP Gate: ");
  input_dns_ = new Fl_Input(80, 120, 200, 20, "DNS: ");
  check_dhcp_ = new Fl_Check_Button(80, 140, 200, 20, "Enable DHCP: ");

  end();

  try {
    GetAllAdaptors(adptInfos_);
    GetAdaptorsInfo(adptInfos_);
  }
  catch (WindowsAPIError & e) {
    ERROR_LOG() << e.what();
    throw;
  }

  for (auto adptInfo : adptInfos_)
    choiceInterface_->add(adptInfo.desc.c_str(), NULL, static_choiceInterface_Selected, this);
}

//static
void MainWindow::static_choiceInterface_Selected(Fl_Widget * w, void * f)
{
  ((MainWindow *)f)->choiceInterface_Selected(w);
}

void MainWindow::choiceInterface_Selected(Fl_Widget * /*w*/)
{
  int index = choiceInterface_->value();
  INFO_LOG() << "choiceInterface index " << index << " selected";
  IPAdapterInfo & info = adptInfos_[index];
  input_ipAddr_->value(info.ipAddr.c_str());
  input_ipMask_->value(info.ipMask.c_str());
  input_ipGate_->value(info.ipGate.c_str());
  input_dns_->value(info.dns.c_str());
  check_dhcp_->value(info.enableDHCP);


  StringBuffer buffer;
  //std::ofstream ofs("out.json");
  Writer<StringBuffer> writer(buffer);
  writer.StartObject();
  writer.Key("ipAddr");
  writer.String(input_ipAddr_->value());
  writer.EndObject();

  INFO_LOG() << buffer.GetString();
}

} // namespace CxcIPConfig
