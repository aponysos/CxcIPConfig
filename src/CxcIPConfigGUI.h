#pragma once

namespace CxcIPConfig
{

class MainWindow : public Fl_Window
{
public:
  MainWindow();
  ~MainWindow();

public:
  void Init();

private:
  static void static_choice_interface_Selected(Fl_Widget * w, void * f);
  void choice_interface_Selected(Fl_Widget * w);
  static void static_button_save_Clicked(Fl_Widget * w, void * f);
  void button_save_Clicked(Fl_Widget * w);
  static void static_button_load_Clicked(Fl_Widget * w, void * f);
  void button_load_Clicked(Fl_Widget * w);
  static void static_button_apply_Clicked(Fl_Widget * w, void * f);
  void button_apply_Clicked(Fl_Widget * w);

private:
  std::unique_ptr<AdapterManager> mgr_;

  Fl_Choice * choice_interface_;
  Fl_Input * input_ipAddr_;
  Fl_Input * input_ipMask_;
  Fl_Input * input_ipGate_;
  Fl_Input * input_dns_;
  Fl_Check_Button * check_dhcp_;
  Fl_Input * input_save_;
  Fl_Button * button_save_;
  Fl_Input * input_load_;
  Fl_Button * button_load_;
  Fl_Button * button_apply_;
};

} // namespace CxcIPConfig
