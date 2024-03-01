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

#ifndef SOURCE_DEVICE_DATA_PERSIST_H_
#define SOURCE_DEVICE_DATA_PERSIST_H_ SOURCE_DEVICE_DATA_PERSIST_H_

#include <boost/property_tree/ptree.hpp>

#include <vector>
#include <filesystem>

#include "device_data.h"

namespace device_data_persistance {

  std::vector<DeviceDataT> load(const std::filesystem::path & configFilePath);
  void save(const std::vector<DeviceDataT> & deviceDataVec, const std::filesystem::path & configFilePath);

}

#endif /* SOURCE_DEVICE_DATA_PERSIST_H_ */
