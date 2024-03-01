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
#include <iomanip>
#include <filesystem>
#include <string>
#include <boost/program_options.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "indi_device_watchdog/indi_device_watchdog-version.h"
#include "indi_device_watchdog.h"
#include "device_data_persistance.h"
#include "option_level.h"
#include "logging.h"


std::string composeStartupMessage() {
  std::stringstream ss;
  
  ss << INDI_DEVICE_WATCHDOG_NAME_VERSION << std::endl << std::endl;
  ss << "Further infos at " << INDI_DEVICE_WATCHDOG_AUTHOR_DOMAIN << std::endl;
  ss << "Contact: " << INDI_DEVICE_WATCHDOG_AUTHOR_MAINTAINER << std::endl;
  
  return ss.str();
}

void printErrorHelp(const std::string & errorMsg, const boost::program_options::options_description & options) {
  std::cerr << std::endl << "Error: " << errorMsg << std::endl << std::endl;
  std::cerr << options << std::endl;
}


int main(int argc, char *argv[]) {

  using namespace boost::program_options;
  namespace fs = std::filesystem;  

  typedef OptionLevelT<'v'> VOptionLevelT;

  unsigned verbosity = 0U;
  VOptionLevelT optionLevel(verbosity);
  
  // Declare the supported options.
  // See also http://stackoverflow.com/questions/17093579/specifying-levels-e-g-verbose-using-boost-program-options
  options_description options("INDI device watchdog options");
  options.add_options()
    ("help,h", "Display this parameter overview")
    ("hostname,H", value<std::string>()->default_value("localhost"), "Set hostname of INDI server")
    ("port,p", value<int>()->default_value(7624), "Port of INDI server.")
    ("indi-bin,B", value<std::string>()->default_value("/usr/bin"), "Search path for INDI binaries.")
    ("indi-server-pipe,P", value<std::string>()->default_value("/tmp/indiserverFIFO"), "Pipe which should be used to write commands to the INDI server.")
    ("device-config,D", value<std::string>()->required(), "Config file with devices to monitor.")
    ("verbose,v", level_value(& optionLevel), "Print more verbose messages at each additional verbosity level.")
    ;

  variables_map vm;
  std::string errorMsg;
  
  try {
    store(command_line_parser(argc, argv).options(options).run(), vm);
    notify(vm);
  
    std::cout << composeStartupMessage() << std::endl
	    << INDI_DEVICE_WATCHDOG_PROJECT_DESCRIPTION << std::endl << std::endl;

  } catch (boost::program_options::required_option & exc) {
    errorMsg = exc.what();  
  } catch (boost::program_options::invalid_command_line_syntax & exc) {
    errorMsg = exc.what();
  }

  if (vm.count("help")) {
    std::cout << options << std::endl;
      
    return 1;
  }

  
  if (! errorMsg.empty()) {
    printErrorHelp(errorMsg, options);

    return 1;
  }
  
  
  logging::trivial::severity_level sev = logging::trivial::warning;

  if (vm.count("verbose") > 0) {
    const VOptionLevelT & ol = vm["verbose"].as<VOptionLevelT>();
    unsigned verboseLevel = (ol.n <= 3 ? ol.n : 3);
    sev = static_cast<logging::trivial::severity_level> (sev - verboseLevel);
  }
  
  std::cout << "Set log-level to: " << sev << std::endl;
  
  LoggingT::init(sev, true /*console*/, true /*log file*/);

  
  try {  
    fs::path currentPath = fs::current_path();
    fs::path deviceConfigFilename = vm["device-config"].as<std::string>();
    fs::path fullPath = currentPath / deviceConfigFilename;
  
    std::vector<DeviceDataT> devicesToMonitor = device_data_persistance::load(deviceConfigFilename);

    std::string indiHostname = vm["hostname"].as<std::string>();
    int indiPort = vm["port"].as<int>();
    std::string indiBinPath = vm["indi-bin"].as<std::string>();
    std::string indiServerPipePath = vm["indi-server-pipe"].as<std::string>();
  
    IndiDeviceWatchdogT indiDeviceWatchdog(indiHostname, indiPort, devicesToMonitor, indiBinPath, indiServerPipePath);

    indiDeviceWatchdog.run();
  } catch (boost::property_tree::json_parser::json_parser_error & exc) {
    errorMsg = exc.what();
  } catch (boost::bad_any_cast & exc) {
    errorMsg = exc.what();
  }

  if (! errorMsg.empty()) {
    printErrorHelp(errorMsg, options);    

    return 1;
  }

  
  return 0;
}
