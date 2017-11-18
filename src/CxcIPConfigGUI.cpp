#include "stdafx.h"
#include "CxcIPConfig.h"
#include "CxcIPConfigGUI.h"
#include "WindowsAPIError.h"
#include "IphlpApiWrapper.h"
#include "HKEYWrapper.h"

using namespace rapidjson;

namespace CxcIPConfig
{

MainWindow::MainWindow()
  : Fl_Window(300, 300, "CxcIPConfig")
{
}

MainWindow::~MainWindow()
{
}

void MainWindow::init()
{
  TRACE_FUNC();

  choice_interface_ = new Fl_Choice(80, 20, 200, 20, "Interface: ");
  input_ipAddr_ = new Fl_Input(80, 60, 200, 20, "IP Address: ");
  input_ipMask_ = new Fl_Input(80, 80, 200, 20, "IP Mask: ");
  input_ipGate_ = new Fl_Input(80, 100, 200, 20, "IP Gate: ");
  input_dns_ = new Fl_Input(80, 120, 200, 20, "DNS: ");
  check_dhcp_ = new Fl_Check_Button(80, 140, 200, 20, "Enable DHCP: ");
  button_save_ = new Fl_Button(80, 160, 200, 20, "Save");
  button_load_ = new Fl_Button(80, 180, 200, 20, "Load");
  button_apply_ = new Fl_Button(80, 200, 200, 20, "Apply");

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
    choice_interface_->add(adptInfo.desc.c_str(), NULL, static_choice_interface_Selected, this);
  button_save_->callback(static_button_save_Clicked, this);
  button_load_->callback(static_button_load_Clicked, this);
  button_apply_->callback(static_button_apply_Clicked, this);
}

//static
void MainWindow::static_choice_interface_Selected(Fl_Widget * w, void * f)
{
  ((MainWindow *)f)->choice_interface_Selected(w);
}

void MainWindow::choice_interface_Selected(Fl_Widget * /*w*/)
{
  int index = choice_interface_->value();
  INFO_LOG() << "choiceInterface index " << index << " selected";
  IPAdapterInfo & info = adptInfos_[index];
  input_ipAddr_->value(info.ipAddr.c_str());
  input_ipMask_->value(info.ipMask.c_str());
  input_ipGate_->value(info.ipGate.c_str());
  input_dns_->value(info.dns.c_str());
  check_dhcp_->value(info.enableDHCP);
}

void MainWindow::static_button_save_Clicked(Fl_Widget * w, void * f)
{
  ((MainWindow *)f)->button_save_Clicked(w);
}

void MainWindow::button_save_Clicked(Fl_Widget * /*w*/)
{
  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  writer.StartObject();
  writer.Key("ipAddr");
  writer.String(input_ipAddr_->value());
  writer.Key("ipMask");
  writer.String(input_ipMask_->value());
  writer.Key("ipGate");
  writer.String(input_ipGate_->value());
  writer.Key("dns");
  writer.String(input_dns_->value());
  writer.Key("dhcp");
  writer.Int(check_dhcp_->value());
  writer.EndObject();

  std::ofstream ofs("out.json");
  ofs << buffer.GetString();
  INFO_LOG() << "saved: " << buffer.GetString();
}

void MainWindow::static_button_load_Clicked(Fl_Widget * w, void * f)
{
  ((MainWindow *)f)->button_load_Clicked(w);
}

void MainWindow::button_load_Clicked(Fl_Widget * /*w*/)
{
  std::ifstream ifs("out.json");
  std::string json;
  ifs >> json;

  Document d;
  d.Parse(json.c_str());
  input_ipAddr_->value(d["ipAddr"].GetString());
  input_ipMask_->value(d["ipMask"].GetString());
  input_ipGate_->value(d["ipGate"].GetString());
  input_dns_->value(d["dns"].GetString());
  check_dhcp_->value(d["dhcp"].GetInt());

  INFO_LOG() << "loaded: " << json;
}

void MainWindow::static_button_apply_Clicked(Fl_Widget * w, void * f)
{
  ((MainWindow *)f)->button_apply_Clicked(w);
}

void MainWindow::button_apply_Clicked(Fl_Widget * /*w*/)
{
  int index = choice_interface_->value();
  INFO_LOG() << "choiceInterface index " << index << " applied";
  IPAdapterInfo & info = adptInfos_[index];
  info.ipAddr = input_ipAddr_->value();
  info.ipMask = input_ipMask_->value();
  info.ipGate = input_ipGate_->value();
  info.dns = input_dns_->value();
  info.enableDHCP = check_dhcp_->value();

  SetAdaptorInfo(info);
  try {
    SetAdaptorInfo(info);
  }
  catch (WindowsAPIError & e) {
    ERROR_LOG() << e.what();
    throw;
  }
}

} // namespace CxcIPConfig
