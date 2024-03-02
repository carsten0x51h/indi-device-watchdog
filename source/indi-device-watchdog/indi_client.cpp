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

#include <thread>

#include "indi_client.h"
#include "indiproperty.h"
#include "basedevice.h"


IndiClientT::IndiClientT() = default;

IndiClientT::~IndiClientT() {
  disconnect();
}


#if INDI_MAJOR_VERSION < 2

void IndiClientT::newDevice(INDI::BaseDevice* dp) {
  notifyNewDevice(*dp);
}

void IndiClientT::removeDevice(INDI::BaseDevice* dp) {
  notifyRemoveDevice(*dp);
}

void IndiClientT::newProperty(INDI::Property* property) {
  notifyNewProperty(*property);
}

void IndiClientT::removeProperty(INDI::Property* property) {
    notifyRemoveProperty(*property);
}

void IndiClientT::newSwitch(ISwitchVectorProperty* svp) {
  std::string deviceName(svp->device);
  std::string propertyName(svp->name);
  INDI::BaseDevice* baseDevicePtr = getDevice(deviceName.c_str());
  INDI::Property* property = baseDevicePtr->getProperty(propertyName.c_str(), INDI_SWITCH);
  
  notifyUpdateProperty(*property);
}

void IndiClientT::newNumber(INumberVectorProperty* nvp) {
  std::string deviceName(nvp->device);
  std::string propertyName(nvp->name);
  INDI::BaseDevice* baseDevicePtr = getDevice(deviceName.c_str());
  INDI::Property* property = baseDevicePtr->getProperty(propertyName.c_str(), INDI_NUMBER);
  
  notifyUpdateProperty(*property);
}

void IndiClientT::newText(ITextVectorProperty* tvp) {
  std::string deviceName(tvp->device);
  std::string propertyName(tvp->name);
  INDI::BaseDevice* baseDevicePtr = getDevice(deviceName.c_str());
  INDI::Property* property = baseDevicePtr->getProperty(propertyName.c_str(), INDI_TEXT);
  
  notifyUpdateProperty(*property);
}

void IndiClientT::newLight(ILightVectorProperty* lvp) {
  std::string deviceName(lvp->device);
  std::string propertyName(lvp->name);
  INDI::BaseDevice* baseDevicePtr = getDevice(deviceName.c_str());
  INDI::Property* property = baseDevicePtr->getProperty(propertyName.c_str(), INDI_LIGHT);
  
  notifyUpdateProperty(*property);
}

void IndiClientT::newBLOB(IBLOB* bp) {
  IBLOBVectorProperty* blobPropVec = bp->bvp;
  std::string deviceName(blobPropVec->device);
  std::string propertyName(blobPropVec->name);
  INDI::BaseDevice* baseDevicePtr = getDevice(deviceName.c_str());
  INDI::Property* property = baseDevicePtr->getProperty(propertyName.c_str(), INDI_BLOB);
  
  notifyUpdateProperty(*property);
}

void IndiClientT::newMessage(INDI::BaseDevice * dp, int messageID) {
  notifyNewMessage(*dp, messageID);
}


#else


void IndiClientT::newDevice(INDI::BaseDevice dp) {
    notifyNewDevice(dp);
}

void IndiClientT::removeDevice(INDI::BaseDevice dp) {
    notifyRemoveDevice(dp);
}

void IndiClientT::newProperty(INDI::Property property) {
    notifyNewProperty(property);
}

void IndiClientT::updateProperty(INDI::Property property) {
    notifyUpdateProperty(property);
}

void IndiClientT::removeProperty(INDI::Property property) {
    notifyRemoveProperty(property);
}

void IndiClientT::newMessage(INDI::BaseDevice dp, int messageID) {
    notifyNewMessage(dp, messageID);
}

#endif


void IndiClientT::serverConnected() {
    notifyServerConnectionStateChanged(IndiServerConnectionStateT::CONNECTED);
}

void IndiClientT::serverDisconnected(int /*exit_code*/) {
    notifyServerConnectionStateChanged(IndiServerConnectionStateT::DISCONNECTED);
}

void IndiClientT::connectToIndiServerBlocking() {
  
  // This function is blocking and therefore needs to be in a separate thread
  bool connectIndiServerResult = this->connectServer();

  if (!connectIndiServerResult) {
    // Emit a failure signal...
    notifyServerConnectionFailed();
  } else {
    // NOTE: Do not emit a success signal since this will already
    // happen from within the INDI server...
  }
}

void IndiClientT::connect() {

    if (!this->isServerConnected()) {
        notifyServerConnectionStateChanged(IndiServerConnectionStateT::CONNECTING);

        mIndiConnectServerThread = std::thread(&IndiClientT::connectToIndiServerBlocking, this);
        mIndiConnectServerThread.detach();
    }
}

void IndiClientT::disconnect() {

    if (this->isServerConnected()) {
      notifyServerConnectionStateChanged(IndiServerConnectionStateT::DISCONNECTING);
      
      this->disconnectServer();
    }

}

bool IndiClientT::isConnected() const {
    return this->isServerConnected();
}

// INDI::BaseDevice IndiClientT::getDevice(const std::string & deviceName) {
//     return this->getDevice(deviceName.c_str());
// }
