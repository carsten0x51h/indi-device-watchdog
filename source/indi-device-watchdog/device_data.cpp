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
#include <ostream>

#include "device_data.h"

DeviceDataT::DeviceDataT() {
  
}

DeviceDataT::DeviceDataT(const std::string & indiDeviceName, const std::string & linuxDeviceName, const std::string & indiDeviceDriverName, bool enableAutoConnect) {
  indiDeviceName_ = indiDeviceName;
  linuxDeviceName_ = linuxDeviceName;
  indiDeviceDriverName_ = indiDeviceDriverName;
  enableAutoConnect_ = enableAutoConnect;
}

std::string DeviceDataT::getLinuxDeviceName() const {
  return linuxDeviceName_; 
}

void DeviceDataT::setLinuxDeviceName(const std::string linuxDeviceName) {
  linuxDeviceName_ = linuxDeviceName;
}

std::string DeviceDataT::getIndiDeviceName() const {
  return indiDeviceName_;
}

void DeviceDataT::setIndiDeviceName(std::string indiDeviceName) {
  indiDeviceName_ = indiDeviceName;
}

std::string DeviceDataT::getIndiDeviceDriverName() const {
  return indiDeviceDriverName_;
}

void DeviceDataT::setIndiDeviceDriverName(std::string indiDeviceDriverName) {
  indiDeviceDriverName_ = indiDeviceDriverName;
}

INDI::BaseDevice DeviceDataT::getIndiBaseDevice() const {
  return indiBaseDevice_;
}

void DeviceDataT::setIndiBaseDevice(INDI::BaseDevice indiBaseDevice) {
  indiBaseDevice_ = indiBaseDevice;
}

bool DeviceDataT::getEnableAutoConnect() const {
  return enableAutoConnect_;
}
void DeviceDataT::setEnableAutoConnect(bool enableAutoConnect) {
  enableAutoConnect_ = enableAutoConnect;
}

std::ostream &
DeviceDataT::print(std::ostream &os) const {

  // NOTE: In older INDI versions getDeviceName() does not have a const qualifier.
  const char * indiDeviceName = const_cast<DeviceDataT*>(this)->indiBaseDevice_.getDeviceName();
  
  os << "Device name: " << indiDeviceName_
     << ", linux device: " << linuxDeviceName_
     << ", INDI driver: " << indiDeviceDriverName_
     << ", INDI device: " << (indiDeviceName != nullptr ? indiDeviceName : "NOT SET");

  return os;
}

std::ostream &operator<<(std::ostream &os, const DeviceDataT &deviceData) {
    return deviceData.print(os);
}
