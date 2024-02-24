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

#include <filesystem>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "device_data.h"
#include "device_data_persistance.h"

namespace device_data_persistance {

  /**
   * Load devices to monitor from a JSON file to a vector of DeviceDataT objects.
   */
  std::vector<DeviceDataT> load(const std::filesystem::path & configFilePath) {

    boost::property_tree::ptree rootPt;
    boost::property_tree::json_parser::read_json(configFilePath.string(), rootPt);
    std::vector<DeviceDataT> deviceDataVec;

    for (boost::property_tree::ptree::value_type & deviceDataNode : rootPt.get_child("indiDevices")) {
      // Animal is a std::pair of a string and a child
      
      const boost::property_tree::ptree & deviceDataPt = deviceDataNode.second;
      
      DeviceDataT deviceData(
			     deviceDataPt.get<std::string>("indiDeviceName"),
			     deviceDataPt.get<std::string>("linuxDeviceName"),
			     deviceDataPt.get<std::string>("indiDeviceDriverName"),
			     deviceDataPt.get<bool>("enableAutoConnect")
			     );
	
	deviceDataVec.push_back(deviceData);
    }
    
    return deviceDataVec;
  }

  
  /**
   * NOTE: So far only used to write the initial data structure to JSON.
   */
  // void save(const std::vector<DeviceDataT> & deviceDataVec, const std::filesystem::path & configFilePath) {
  //   boost::property_tree::ptree rootPt;

  //   boost::property_tree::ptree devicesPt;

  //   for (auto & deviceData : deviceDataVec) {
  //     boost::property_tree::ptree deviceDataPt;
  //     deviceDataPt.put<std::string>("indiDeviceName", deviceData.getIndiDeviceName());
  //     deviceDataPt.put<std::string>("linuxDeviceName", deviceData.getLinuxDeviceName());
  //     deviceDataPt.put<std::string>("indiDeviceDriverName", deviceData.getIndiDeviceDriverName());
  //     deviceDataPt.put<bool>("enableAutoConnect", deviceData.getEnableAutoConnect());

  //     devicesPt.push_back(std::make_pair("", deviceDataPt));      
  //   }
    
  //   rootPt.add_child("indiDevices", devicesPt);
    

  //   // TODO: Open ooutput stream for configFilePath...
  //   boost::property_tree::json_parser::write_json(std::cout, rootPt);
  // }
  
}
