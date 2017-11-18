#pragma once

// log4cpp headers
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <WinSock2.h>
#include <iphlpapi.h>

#include "log4cpp\log4cpp.h"

#pragma warning(push)
#pragma warning(disable:4244) // '=': conversion from 'x' to 'x', possible loss of data
#pragma warning(disable:4458) // declaration of 'x' hides class member
#include "FL/Fl.H"
#include "FL/Fl_Window.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_Choice.H"
#include "FL/Fl_Input.H"
#include "FL/Fl_Check_Button.H"
#pragma warning(pop)

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
