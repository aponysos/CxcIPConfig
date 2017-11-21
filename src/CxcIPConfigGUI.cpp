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

void MainWindow::Init()
{
  TRACE_FUNC();

  choice_interface_ = new Fl_Choice(80, 20, 200, 20, "Interface: ");
  input_ipAddr_ = new Fl_Input(80, 60, 200, 20, "IP Address: ");
  input_ipMask_ = new Fl_Input(80, 80, 200, 20, "IP Mask: ");
  input_ipGate_ = new Fl_Input(80, 100, 200, 20, "IP Gate: ");
  input_dns_ = new Fl_Input(80, 120, 200, 20, "DNS: ");
  check_dhcp_ = new Fl_Check_Button(80, 140, 200, 20, "Enable DHCP: ");
  input_save_ = new Fl_Input(20, 160, 60, 20, "S");
  button_save_ = new Fl_Button(80, 160, 200, 20, "Save");
  input_load_ = new Fl_Input(20, 180, 60, 20, "L");
  button_load_ = new Fl_Button(80, 180, 200, 20, "Load");
  button_apply_ = new Fl_Button(80, 200, 200, 20, "Apply");

  end();

  LoadConfig();

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

void MainWindow::SaveConfig()
{
  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  writer.StartArray();
  for (auto ci : configInfos_) {
    writer.StartObject();
    writer.Key("name");
    writer.String(ci.first);
    IPAdapterInfo & info = ci.second;
    writer.Key("ipAddr");
    writer.String(info.ipAddr);
    writer.Key("ipMask");
    writer.String(info.ipMask);
    writer.Key("ipGate");
    writer.String(info.ipGate);
    writer.Key("dns");
    writer.String(info.dns);
    writer.Key("dhcp");
    writer.Bool(info.enableDHCP);
    writer.EndObject();
  }
  writer.EndArray();

  std::ofstream ofs("out.json");
  ofs << buffer.GetString();
  INFO_LOG() << "saved: " << buffer.GetString();
}

void MainWindow::LoadConfig()
{
  std::ifstream ifs("out.json");
  std::string json;
  ifs >> json;
  Document d;
  d.Parse(json.c_str());
  INFO_LOG() << "loaded: " << json;
  
  if (!d.IsArray())
    return;

  for (Value * pv = d.Begin(); pv != d.End(); ++pv) {
    Value & v = *pv;
    std::string name = v["name"].GetString();
    IPAdapterInfo info;
    info.ipAddr = v["ipAddr"].GetString();
    info.ipMask = v["ipMask"].GetString();
    info.ipGate = v["ipGate"].GetString();
    info.dns = v["dns"].GetString();
    info.enableDHCP = v["dhcp"].GetBool();
    configInfos_[name] = info;
  }
}

//static
void MainWindow::static_choice_interface_Selected(Fl_Widget * w, void * f)
{
  ((MainWindow *)f)->choice_interface_Selected(w);
}

void MainWindow::choice_interface_Selected(Fl_Widget * /*w*/)
{
  int index = choice_interface_->value();
  INFO_LOG() << "choice_interface_Selected index = " << index;

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
  std::string saveName = input_save_->value();
  INFO_LOG() << "button_save_Clicked saveName = " << saveName;

  IPAdapterInfo & info = configInfos_[saveName];
  info.ipAddr = input_ipAddr_->value();
  info.ipMask = input_ipMask_->value();
  info.ipGate = input_ipGate_->value();
  info.dns = input_dns_->value();
  info.enableDHCP = check_dhcp_->value();

  SaveConfig();
}

void MainWindow::static_button_load_Clicked(Fl_Widget * w, void * f)
{
  ((MainWindow *)f)->button_load_Clicked(w);
}

void MainWindow::button_load_Clicked(Fl_Widget * /*w*/)
{
  std::string loadName = input_load_->value();
  INFO_LOG() << "button_load_Clicked loadName = " << loadName;

  IPAdapterInfo & info = configInfos_[loadName];
  input_ipAddr_->value(info.ipAddr.c_str());
  input_ipMask_->value(info.ipMask.c_str());
  input_ipGate_->value(info.ipGate.c_str());
  input_dns_->value(info.dns.c_str());
  check_dhcp_->value(info.enableDHCP);
}

void MainWindow::static_button_apply_Clicked(Fl_Widget * w, void * f)
{
  ((MainWindow *)f)->button_apply_Clicked(w);
}

void MainWindow::button_apply_Clicked(Fl_Widget * /*w*/)
{
  int index = choice_interface_->value();
  INFO_LOG() << "button_apply_Clicked index = " << index;

  IPAdapterInfo & info = adptInfos_[index];
  info.ipAddr = input_ipAddr_->value();
  info.ipMask = input_ipMask_->value();
  info.ipGate = input_ipGate_->value();
  info.dns = input_dns_->value();
  info.enableDHCP = check_dhcp_->value();

  try {
    SetAdaptorInfo(info);
  }
  catch (WindowsAPIError & e) {
    ERROR_LOG() << e.what();
    throw;
  }
}

} // namespace CxcIPConfig
