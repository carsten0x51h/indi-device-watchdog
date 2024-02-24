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

#ifndef SOURCE_INDI_AUTO_CONNECTOR_DEVICE_DATA_H_
#define SOURCE_INDI_AUTO_CONNECTOR_DEVICE_DATA_H_ SOURCE_INDI_AUTO_CONNECTOR_DEVICE_DATA_H_

#include <string>
#include <memory>

#include "basedevice.h"

class DeviceDataT {
 private:
  std::string indiDeviceName_;
  std::string linuxDeviceName_; // NOTE: Could be party derived from PORT property, but not always. Therefore, it will be explicitly set via cfg.
  std::string indiDeviceDriverName_;
  INDI::BaseDevice indiBaseDevice_;
  bool enableAutoConnect_;
  
 public:
  DeviceDataT();
  DeviceDataT(const std::string & indiDeviceName, const std::string & linuxDeviceName, const std::string & indiDeviceDriverName, bool enableAutoConnect);

  std::string getIndiDeviceName() const;
  void setIndiDeviceName(std::string indiDeviceName);

  std::string getLinuxDeviceName() const;
  void setLinuxDeviceName(const std::string linuxDeviceName);

  std::string getIndiDeviceDriverName() const;
  void setIndiDeviceDriverName(const std::string indiDeviceDriverName);

  INDI::BaseDevice getIndiBaseDevice() const;
  void setIndiBaseDevice(INDI::BaseDevice indiBaseDevice);

  bool getEnableAutoConnect() const;
  void setEnableAutoConnect(bool enableAutoConnect);

  std::ostream &print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const DeviceDataT &deviceData);
};

#endif /*SOURCE_INDI_AUTO_CONNECTOR_DEVICE_DATA_H_*/
