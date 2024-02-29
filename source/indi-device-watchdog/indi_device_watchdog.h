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
#include <memory>
#include <mutex>

#include "indi_client.h"
#include "device_data.h"
#include "indi_driver_restart_manager.h"

/**
 * TODO: Idea: Rename to IndiDeviceWatchdogT
 */
class IndiDeviceWatchdogT {
 private:
  std::string hostname_;
  int port_;
  std::shared_ptr<IndiClientT> client_;
  bool connected_;
  boost::signals2::connection serverConnectionFailedListenerConnection_;
  boost::signals2::connection newDeviceListenerConnection_;
  boost::signals2::connection removeDeviceListenerConnection_;
  boost::signals2::connection newPropertyListenerConnection_;
  boost::signals2::connection removePropertyListenerConnection_;
  boost::signals2::connection updatePropertyListenerConnection_;
  //  boost::signals2::connection serverConnectionStateChangedConnection_;

  typedef std::map<std::string /*device name*/, DeviceDataT> DeviceConnStateMapT;
  DeviceConnStateMapT deviceConnections_; 

  std::mutex deviceConnectionsMutex_;

  IndiDriverRestartManagerT indiDriverRestartManager_;

  static bool isDeviceValid(INDI::BaseDevice indiBaseDevice);
  static INDI::BaseDevice getBaseDeviceFromProperty(INDI::Property property);
  void resetIndiClient();
  
  void addIndiDevice(INDI::BaseDevice device);
  void removeIndiDevice(INDI::BaseDevice device);
  void propertyUpdated(INDI::Property property);
  void propertyRemoved(INDI::Property property);


  bool requestIndiDriverRestart(DeviceDataT & deviceData);
  bool requestConnectionStateChange(INDI::BaseDevice indiBaseDevice, bool connect);
  bool sendIndiDeviceDisconnectRequest(INDI::BaseDevice indiBaseDevice);
  bool fileExists(const std::string & pathToFile) const;
  static bool isIndiDeviceConnected(INDI::BaseDevice indiBaseDevice);
  bool handleDeviceConnection(DeviceDataT & deviceData);

  
 public:
  IndiDeviceWatchdogT(const std::string & hostname, int port, const std::vector<DeviceDataT> & devicesToMonitor);
  ~IndiDeviceWatchdogT();
  
  void run();
};

#endif /*SOURCE_INDI_AUTO_CONNECTOR_H_*/
