/*****************************************************************************
 *
 *  INDI device watchdog  - Monitors the specified INDI devices and optionally
 *  the corresponding Linux devices and tries to keep them connected. Under
 *  certain conditions the device watchdog restarts the respective INDI driver
 *  without restarting the complete INDI server.
 *
 *  Copyright(C) 2024 Carsten Schmitt <c [at] lost-infinity.com>
 *
 *  More info on https://www.lost-infinity.com
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

#include <iostream>
#include <filesystem>
#include <string>
#include <boost/program_options.hpp>

#include "indi_device_watchdog/indi_device_watchdog-version.h"
#include "indi_device_watchdog.h"
#include "device_data_persistance.h"
#include "logging.h"

std::string composeStartupMessage() {
  std::stringstream ss;
  
  ss << INDI_DEVICE_WATCHDOG_NAME_VERSION << std::endl << std::endl;
  ss << "Further infos at " << INDI_DEVICE_WATCHDOG_AUTHOR_DOMAIN << std::endl;
  ss << "Contact: " << INDI_DEVICE_WATCHDOG_AUTHOR_MAINTAINER << std::endl;
  
  return ss.str();
}


int main(int argc, char *argv[]) {

  using namespace boost::program_options;
  namespace fs = std::filesystem;  
  
  // Declare the supported options.
  options_description options("Astrobox control options");
  options.add_options()
    ("help", "Display this parameter overview")
    ("hostname", value<std::string>()->default_value("localhost"), "Set hostname of INDi server")
    ("port", value<int>()->default_value(7624), "Port of INDI server.")
    ("config", value<std::string>()->default_value("indi_devices.json"), "Config file with devices to monitor.")    
    ;

  variables_map vm;
  
  store(command_line_parser(argc, argv).options(options).run(), vm);
  notify(vm);
  
  
  if (vm.count("help")) {
    std::cout << INDI_DEVICE_WATCHDOG_PROJECT_NAME << std::endl
	      << INDI_DEVICE_WATCHDOG_PROJECT_DESCRIPTION << std::endl << std::endl
	      << "Options: " << std::endl
	      << options << std::endl;
    return 1;
  }
  

  // TODO: Pass in log-level via cmdline
  logging::trivial::severity_level sev = logging::trivial::debug;
  LoggingT::init(sev, true /*console*/, true /*log file*/);


  LOG(info) << composeStartupMessage() << std::endl;
  
  
  fs::path currentPath = fs::current_path();
  fs::path configFilename = vm["config"].as<std::string>();
  fs::path fullPath = currentPath / configFilename;
  
  std::vector<DeviceDataT> devicesToMonitor = device_data_persistance::load(configFilename);

  std::string indiHostname = vm["hostname"].as<std::string>();
  int indiPort = vm["port"].as<int>();
  
  IndiDeviceWatchdogT indiDeviceWatchdog(indiHostname, indiPort, devicesToMonitor);

  indiDeviceWatchdog.run();
    
  return 0;
}
