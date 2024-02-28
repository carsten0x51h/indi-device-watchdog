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

#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <filesystem>

#include "wait_for.h"

#include "indi_auto_connector.h"

IndiAutoConnectorT::IndiAutoConnectorT(const std::string & hostname, int port, const std::vector<DeviceDataT> & devicesToMonitor) : hostname_(hostname), port_(port) {
  using namespace std::chrono_literals;

  resetIndiClient();
  
  // Process config entries to deviceConnections_
  for (auto it = devicesToMonitor.begin(); it != devicesToMonitor.end(); ++it) {
    deviceConnections_.insert( std::pair<std::string, DeviceDataT>(it->getIndiDeviceName(), *it) );
  }

}

IndiAutoConnectorT::~IndiAutoConnectorT() {

  serverConnectionFailedListenerConnection_.disconnect();
  //serverConnectionStateChangedConnection_.disconnect();
  newDeviceListenerConnection_.disconnect();
  removeDeviceListenerConnection_.disconnect();
  newPropertyListenerConnection_.disconnect();
  removePropertyListenerConnection_.disconnect();
  updatePropertyListenerConnection_.disconnect();
  
  client_->disconnect();
}


void IndiAutoConnectorT::resetIndiClient() {

  serverConnectionFailedListenerConnection_.disconnect();
  //serverConnectionStateChangedConnection_.disconnect();
  newDeviceListenerConnection_.disconnect();
  removeDeviceListenerConnection_.disconnect();
  newPropertyListenerConnection_.disconnect();
  removePropertyListenerConnection_.disconnect();
  updatePropertyListenerConnection_.disconnect();

  if (client_ != nullptr) {
    client_->disconnect();
  }

  std::cerr << "Resetting INDI client..." << std::endl;

  connected_ = false;
  
  client_ = std::make_shared<IndiClientT>(); // Create a new client
  
  client_->setServer(hostname_.c_str(), port_);
  
  serverConnectionFailedListenerConnection_ = client_->registerServerConnectionFailedListener([&]() {
    std::cerr << "Connection to INDI server failed." << std::endl;
    connected_ = false;
  });

  // serverConnectionStateChangedConnection_ = client_->registerServerConnectionStateChangedListener([&](IndiServerConnectionStateT::TypeE indiServerConnectionState) {
  //   std::cout << "Connection to INDI changed to " << IndiServerConnectionStateT::asStr(indiServerConnectionState) << std::endl;
    
  //   connected_ = (indiServerConnectionState == IndiServerConnectionStateT::CONNECTED);
  // });
    
  newDeviceListenerConnection_ = client_->registerNewDeviceListener([&](INDI::BaseDevice device) {
    addIndiDevice(device);
  });

  removeDeviceListenerConnection_ = client_->registerRemoveDeviceListener([&](INDI::BaseDevice device) {
    removeIndiDevice(device);
  });

  newPropertyListenerConnection_ = client_->registerNewPropertyListener([&](INDI::Property property) {
    propertyUpdated(property);
  });

  removePropertyListenerConnection_ = client_->registerRemovePropertyListener([&](INDI::Property property) {
    propertyRemoved(property);
  });
  
  updatePropertyListenerConnection_ = client_->registerUpdatePropertyListener([&](INDI::Property property) {
    propertyUpdated(property);
  });
}

INDI::BaseDevice IndiAutoConnectorT::getBaseDeviceFromProperty(INDI::Property property) {
#if INDI_MAJOR_VERSION < 2
  return *property.getBaseDevice();
#else
  return property.getBaseDevice();
#endif
}


void IndiAutoConnectorT::addIndiDevice(INDI::BaseDevice indiBaseDevice) {
  std::string indiDeviceName = indiBaseDevice.getDeviceName();

  std::cerr << "addIndiDevice: " << indiDeviceName << std::endl;

  std::lock_guard<std::mutex> guard(deviceConnectionsMutex_);

  auto indiDeviceDataIt = deviceConnections_.find(indiDeviceName);

  if (indiDeviceDataIt != deviceConnections_.end()) {
    indiDeviceDataIt->second.setIndiDeviceName(indiDeviceName);
    indiDeviceDataIt->second.setIndiBaseDevice(indiBaseDevice);
  }
  else {
    std::cerr << "NOTE: Not handling INDI device '" << indiDeviceName << "' since it is not on the device list." << std::endl;
  }
}



void IndiAutoConnectorT::removeIndiDevice(INDI::BaseDevice indiBaseDevice) {
  std::string indiDeviceName = indiBaseDevice.getDeviceName();

  std::cerr << "removeIndiDevice: " << indiDeviceName << std::endl;
  
  std::lock_guard<std::mutex> guard(deviceConnectionsMutex_);

  auto indiDeviceDataIt = deviceConnections_.find(indiDeviceName);

  if (indiDeviceDataIt != deviceConnections_.end()) {
    indiDeviceDataIt->second.setIndiBaseDevice(INDI::BaseDevice());
  }
  else {
    std::cerr << "NOTE: Not handling INDI device '" << indiDeviceName << "' since it is not on the device list." << std::endl;
  }
}

void IndiAutoConnectorT::propertyRemoved(INDI::Property property) {

  if (! std::strcmp(property.getName(), "CONNECTION")) {
    std::cerr << "propertyRemoved..." << std::endl;

    
    // INDI::BaseDevice indiBaseDevice = getBaseDeviceFromProperty(property);
    // std::string indiDeviceName = indiBaseDevice.getDeviceName();

    // std::lock_guard<std::mutex> guard(deviceConnectionsMutex_);

    // DeviceConnStateMapT::iterator deviceDataIt = deviceConnections_.find(indiDeviceName);
    
    // if (deviceDataIt != deviceConnections_.end()) {
    //   // Put an empty property as connection property
    //   deviceDataIt->second.setIndiConnectionProp(INDI::Property());
    // }
  }
}


void IndiAutoConnectorT::propertyUpdated(INDI::Property /*property*/) {
  
  // if (! std::strcmp(property.getName(), "CONNECTION")) {

  //   INDI::BaseDevice indiBaseDevice = getBaseDeviceFromProperty(property);
  //   std::string indiDeviceName = indiBaseDevice.getDeviceName();

  //   std::lock_guard<std::mutex> guard(deviceConnectionsMutex_);

  //   DeviceConnStateMapT::iterator deviceDataIt = deviceConnections_.find(indiDeviceName);
    
  //   INDI::PropertySwitch connectionSwitch = indiBaseDevice.getSwitch("CONNECTION");

  //   if (deviceDataIt != deviceConnections_.end()) {
  //     // Update the connection property only
  //     deviceDataIt->second.setIndiConnectionProp(connectionSwitch);
  //   }
  // }  
}


bool IndiAutoConnectorT::requestIndiDriverRestart(DeviceDataT & deviceData) {
  std::string driverName = deviceData.getIndiDeviceDriverName();
  
  bool restarted = indiDriverRestartManager_.requestRestart(driverName);
  
  deviceData.setIndiBaseDevice(INDI::BaseDevice());

  return restarted;
}


bool IndiAutoConnectorT::isDeviceValid(INDI::BaseDevice indiBaseDevice) {
#if INDI_MAJOR_VERSION < 2
  return (indiBaseDevice.getDeviceName() != nullptr);
#else
  return indiBaseDevice.isValid();
#endif
}


/**
 * NOTE: Just sening this successfully, does not yet mean that it was successfully connected!
 *      --> Actually an updated from the INDI server will be sent once the property has
 *          changed successfully. Only then the connect was successful.
 */
bool IndiAutoConnectorT::requestConnectionStateChange(INDI::BaseDevice indiBaseDevice, bool connect) {

  std::cerr << "Sending INDI device " << (connect ? "connect" : " disconnect") << " request for device ' '" << indiBaseDevice.getDeviceName() << "'..." << std::endl;

  if (! isDeviceValid(indiBaseDevice)) {
    return false;
  }


#if INDI_MAJOR_VERSION < 2
    ISwitchVectorProperty* connectionSwitchVec = indiBaseDevice.getSwitch("CONNECTION");

    if (connectionSwitchVec == nullptr) {
      return false;
    }
    
    // TODO: Better get via propery name "CONNECT" / "DISCONNECT"? -> ISwitch * p = IUFindSwitch(vec, "CONNECT");
    // TODO: Check switch state - IPS_ALERT / IPS_OK...?
    // TODO: Check connectionSwitchVec->sp...
    connectionSwitchVec->sp[0].s = (connect ? ISS_ON : ISS_OFF);
    connectionSwitchVec->sp[1].s = (connect ? ISS_OFF : ISS_ON);
    
    client_->sendNewSwitch(connectionSwitchVec);

#else    
  INDI::PropertySwitch connectionSwitch = indiBaseDevice.getSwitch("CONNECTION");

  if (! connectionSwitch.isValid()) {
    return false;
  }

  // TODO: Better get via property name "CONNECT" / "DISCONNECT"? 
  connectionSwitch[0].setState(connect ? ISS_ON : ISS_OFF);
  connectionSwitch[1].setState(connect ? ISS_OFF : ISS_ON);

  client_->sendNewSwitch(connectionSwitch);
#endif
  
  return true;
}


bool IndiAutoConnectorT::fileExists(const std::string & pathToFile) const {
  return std::filesystem::exists(pathToFile);
}


bool IndiAutoConnectorT::isIndiDeviceConnected(INDI::BaseDevice indiBaseDevice) {

#if INDI_MAJOR_VERSION < 2
  ISwitchVectorProperty* connectionSwitchVec = indiBaseDevice.getSwitch("CONNECTION");
  return (connectionSwitchVec != nullptr ? (connectionSwitchVec->sp != nullptr ? connectionSwitchVec->sp[0].s == ISS_ON : false) : false);  
#else    
  INDI::PropertySwitch connectionSwitch = indiBaseDevice.getSwitch("CONNECTION");
  return (connectionSwitch.isValid() ? (connectionSwitch[0].getState() == ISS_ON ? true : false) : false);
#endif
}


bool IndiAutoConnectorT::handleDeviceConnection(DeviceDataT & deviceData) {
  std::string indiDeviceName = deviceData.getIndiDeviceName();

  bool indiDeviceConnected = isIndiDeviceConnected(deviceData.getIndiBaseDevice());
  bool linuxDeviceExists = fileExists(deviceData.getLinuxDeviceName());
  bool indiDeviceExists = isDeviceValid(deviceData.getIndiBaseDevice());
  
  std::cerr << "Processing '" << indiDeviceName << "' -> Linux device exists? " << linuxDeviceExists << ", INDI device exists? " << indiDeviceExists << ", INDI device connected? " << indiDeviceConnected << " (details: " << deviceData << ")" << std::endl;

  if (linuxDeviceExists) {
    // Linux device already existed before
    if (! indiDeviceExists) {
      // Linux device is there but corresponding INDI base
      // device does not exist -> Restart INDI driver
      return requestIndiDriverRestart(deviceData);
    }
    else {
      // Linux device is there and corresponding INDI base
      // device exists -> connect if auto connect is enabled
      // and INDI device is not connected
      if (! indiDeviceConnected && deviceData.getEnableAutoConnect()) {
	// Try to connect INDI device
	bool successful = requestConnectionStateChange(deviceData.getIndiBaseDevice(), true);

	if (! successful) {
	  // If connection fails, restart INDI driver
	  return requestIndiDriverRestart(deviceData);
	}
      }
    }
  }
  else {
    // Linux device does not exist
    if (indiDeviceConnected) {
      // Disconnect INDI device
      bool successful = requestConnectionStateChange(deviceData.getIndiBaseDevice(), false);

      deviceData.setIndiBaseDevice(INDI::BaseDevice());
      
      if (! successful) {
	// If disconnect fails, restart INDI driver
	return requestIndiDriverRestart(deviceData);
      }
    }
  }

  return false;
}


void IndiAutoConnectorT::run() {
  using namespace std::chrono_literals;


  // Try to connect to the INDI server forever
  while(true) {
    std::cerr << "Trying to connect to INDI server...";

    // Try to (re-) connect to the INDI server
    client_->connect();

    auto isClientConnected = [&]() -> bool {
      return client_->isConnected();
    };
  
    try {
      wait_for(isClientConnected, 5000ms);
      connected_ = true;
    } catch (std::runtime_error & exc) {
      std::cerr << "Timeout!" << std::endl;
      connected_ = false;
      continue;
    }

    std::cerr << "Connected!" << std::endl;

    while(connected_) {
      std::this_thread::sleep_for(std::chrono::milliseconds(5000ms));

      std::lock_guard<std::mutex> guard(deviceConnectionsMutex_);
      
      for (auto it = deviceConnections_.begin(); it != deviceConnections_.end(); ++it) {
	bool restarted = handleDeviceConnection(it->second);

	if (restarted) {

	  resetIndiClient();
	  // std::this_thread::sleep_for(std::chrono::milliseconds(3000ms));

	  // client_->disconnect();

	  // auto isClientDisconnected = [&]() -> bool {
	  //   return !client_->isConnected();
	  // };
  
	  // try {
	  //   wait_for(isClientDisconnected, 5000ms);
	  //   connected_ = false;
	  // } catch (std::runtime_error & exc) {
	  //   std::cerr << "Timeout! Assuming disconnected." << std::endl;
	  //   connected_ = false;
	  // }
	  
	  break;
	}
      }
      
      std::cerr << std::endl << std::endl;
    }

    std::cerr << "Lost connection to INDI server." << std::endl;
  }

}
