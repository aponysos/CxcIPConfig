#pragma once

// log4cpp headers
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <iomanip>
#include <sstream>
#include <WinSock2.h>
#include <iphlpapi.h>

#include "log4cpp\log4cpp.h"

#pragma warning(push)
#pragma warning(disable:4244) // '=': conversion from 'x' to 'x', possible loss of data
#pragma warning(disable:4458) // declaration of 'x' hides class member
#include "FL/Fl.H"
#include "FL/Fl_Window.H"
#include "FL/Fl_Box.H"
#pragma warning(pop)
