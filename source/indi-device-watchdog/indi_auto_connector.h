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

#ifndef SOURCE_INDI_AUTO_CONNECTOR_H_
#define SOURCE_INDI_AUTO_CONNECTOR_H_ SOURCE_INDI_AUTO_CONNECTOR_H_

#include <boost/signals2.hpp>
#include <map>
#include <mutex>

#include "indi_client.h"
#include "device_data.h"
#include "indi_driver_restart_manager.h"

/**
 * TODO: Idea: Rename to IndiDeviceWatchdogT
 */
class IndiAutoConnectorT {
 private:
  IndiClientT client_;
  bool connected_;
  boost::signals2::connection serverConnectionFailedListenerConnection_;
  boost::signals2::connection newDeviceListenerConnection_;
  boost::signals2::connection removeDeviceListenerConnection_;
  boost::signals2::connection newPropertyListenerConnection_;
  boost::signals2::connection removePropertyListenerConnection_;
  boost::signals2::connection updatePropertyListenerConnection_;

  typedef std::map<std::string /*device name*/, DeviceDataT> DeviceConnStateMapT;
  DeviceConnStateMapT deviceConnections_; 

  std::mutex deviceConnectionsMutex_;

  IndiDriverRestartManagerT indiDriverRestartManager_;
  
  void addIndiDevice(INDI::BaseDevice device);
  void removeIndiDevice(INDI::BaseDevice device);
  void propertyUpdated(INDI::Property property);
  void propertyRemoved(INDI::Property property);


  void switchAllDeviceLeds(bool onOff);
  void updateOverallStatusLed(bool allAreConnected);
  bool areAllIndiDevicesConnected() const;
  void requestIndiDriverRestart(DeviceDataT & deviceData);
  bool sendIndiDeviceConnectRequest(INDI::BaseDevice indiBaseDevice);
  bool sendIndiDeviceDisconnectRequest(INDI::BaseDevice indiBaseDevice);
  bool fileExists(const std::string & pathToFile) const;
  bool isIndiDeviceConnected(INDI::PropertySwitch indiConnectionProp) const;
  void handleDeviceConnection(DeviceDataT & deviceData);

  
 public:
  IndiAutoConnectorT(const std::string & hostname, int port, const std::vector<DeviceDataT> & devicesToMonitor);
  ~IndiAutoConnectorT();
  
  void run();
};

#endif /*SOURCE_INDI_AUTO_CONNECTOR_H_*/
