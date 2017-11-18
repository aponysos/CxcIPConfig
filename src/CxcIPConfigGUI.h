#pragma once

namespace CxcIPConfig
{

class MainWindow : public Fl_Window
{
public:
  MainWindow(int w, int h, const char* t);
  ~MainWindow();

public:
  void init();

private:
  static void static_choiceInterface_Selected(Fl_Widget * w, void * f);
  void choiceInterface_Selected(Fl_Widget * w);

private:
  std::vector<IPAdapterInfo> adptInfos_;
  Fl_Choice * choiceInterface_;
  Fl_Input * input_ipAddr_;
  Fl_Input * input_ipMask_;
  Fl_Input * input_ipGate_;
  Fl_Input * input_dns_;
  Fl_Check_Button * check_dhcp_;
};

} // namespace CxcIPConfig
