#include "stdafx.h"
#include "Util.h"
#include "CxcIPConfig.h"
#include "CxcIPConfigGUI.h"

using namespace CxcIPConfig;

int main(int argc, char **argv)
{
  InitLog();

  MainWindow * window = new MainWindow();
  window->init();
  window->show(argc, argv);

  return Fl::run();
}
