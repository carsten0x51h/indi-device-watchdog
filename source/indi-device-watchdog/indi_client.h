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

#ifndef SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_CLIENT_H_
#define SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_CLIENT_H_ SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_CLIENT_H_

#include "indiapi.h"
#include "baseclient.h"
#include "basedevice.h"

#include "indi_server_connection_state.h"

#include <boost/signals2.hpp>
#include <thread>

// TODO: Maybe better inherit protected...
class IndiClientT : public INDI::BaseClient {
private:

    typedef boost::signals2::signal<void(INDI::BaseDevice dp)> NewDeviceListenersT;
    NewDeviceListenersT mNewDeviceListeners;

    typedef boost::signals2::signal<void(INDI::BaseDevice dp)> RemoveDeviceListenersT;
    RemoveDeviceListenersT mRemoveDeviceListeners;

    typedef boost::signals2::signal<void(INDI::Property property)> NewPropertyListenersT;
    NewPropertyListenersT mNewPropertyListeners;

    typedef boost::signals2::signal<void(INDI::Property property)> UpdatePropertyListenersT;
    UpdatePropertyListenersT mUpdatePropertyListeners;

    typedef boost::signals2::signal<void(INDI::Property property)> RemovePropertyListenersT;
    RemovePropertyListenersT mRemovePropertyListeners;

    typedef boost::signals2::signal<void(INDI::BaseDevice dp, int messageID)> NewMessageListenersT;
    NewMessageListenersT mNewMessageListeners;

    typedef boost::signals2::signal<void(
            IndiServerConnectionStateT::TypeE indiServerConnectionState)> ServerConnectionStateChangedListenersT;
    ServerConnectionStateChangedListenersT mServerConectionStateChangedListeners;

    typedef boost::signals2::signal<void()> ServerConnectionFailedListenersT;
    ServerConnectionFailedListenersT mServerConectionFailedListeners;


    void connectToIndiServerBlocking();

    std::thread mIndiConnectServerThread;

    // We do not want device copies
    IndiClientT(const IndiClientT &);

    IndiClientT &operator=(const IndiClientT &);

public:
    IndiClientT();

    ~IndiClientT() override;

    boost::signals2::connection registerNewPropertyListener(const NewPropertyListenersT::slot_type &inCallBack) {
        return mNewPropertyListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterNewPropertyListener(const T &inCallBack) {
        inCallBack.disconnect();
    }


      boost::signals2::connection registerUpdatePropertyListener(const UpdatePropertyListenersT::slot_type &inCallBack) {
        return mUpdatePropertyListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterUpdatePropertyListener(const T &inCallBack) {
        inCallBack.disconnect();
    }

  
  
    boost::signals2::connection registerRemovePropertyListener(const RemovePropertyListenersT::slot_type &inCallBack) {
        return mRemovePropertyListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterRemovePropertyListener(const T &inCallBack) {
        inCallBack.disconnect();
    }


    boost::signals2::connection registerRemoveDeviceListener(const RemoveDeviceListenersT::slot_type &inCallBack) {
        return mRemoveDeviceListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterRemoveDeviceListener(const T &inCallBack) {
        inCallBack.disconnect();
    }

    boost::signals2::connection registerNewDeviceListener(const NewDeviceListenersT::slot_type &inCallBack) {
        return mNewDeviceListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterNewDeviceListener(const T &inCallBack) {
        inCallBack.disconnect();
    }

    boost::signals2::connection registerNewMessageListener(const NewMessageListenersT::slot_type &inCallBack) {
        return mNewMessageListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterNewMessageListener(const T &inCallBack) {
        inCallBack.disconnect();
    }

    boost::signals2::connection
    registerServerConnectionStateChangedListener(const ServerConnectionStateChangedListenersT::slot_type &inCallBack) {
        return mServerConectionStateChangedListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterServerConnectionStateChangedListener(const T &inCallBack) {
        inCallBack.disconnect();
    }

    boost::signals2::connection
    registerServerConnectionFailedListener(const ServerConnectionFailedListenersT::slot_type &inCallBack) {
        return mServerConectionFailedListeners.connect(inCallBack);
    }

    template<class T>
    void unregisterServerConnectionFailedListener(const T &inCallBack) {
        inCallBack.disconnect();
    }


    void connect();

    void disconnect();

    [[nodiscard]] bool isConnected() const;

  //    [[nodiscard]] INDI::BaseDevice getDevice(const std::string &deviceName);

protected:
    void notifyNewDevice(INDI::BaseDevice dp) { mNewDeviceListeners(dp); }

    void notifyRemoveDevice(INDI::BaseDevice dp) { mRemoveDeviceListeners(dp); }

    void notifyNewProperty(INDI::Property property) { mNewPropertyListeners(property); }

    void notifyUpdateProperty(INDI::Property property) { mUpdatePropertyListeners(property); }

    void notifyRemoveProperty(INDI::Property property) { mRemovePropertyListeners(property); }

    void notifyNewMessage(INDI::BaseDevice dp, int messageID) { mNewMessageListeners(dp, messageID); }

    void notifyServerConnectionStateChanged(IndiServerConnectionStateT::TypeE indiServerConectionState) {
        mServerConectionStateChangedListeners(indiServerConectionState);
    }

    void notifyServerConnectionFailed() { mServerConectionFailedListeners(); }


    /////////////////////////////////////////////////////////
    // Implement the base device methods
    /////////////////////////////////////////////////////////
    void newDevice(INDI::BaseDevice dp) override;

    void removeDevice(INDI::BaseDevice dp) override;

    void newProperty(INDI::Property property) override;

    void updateProperty(INDI::Property property) override;

    void removeProperty(INDI::Property property) override;

    void newMessage(INDI::BaseDevice dp, int messageID) override;
  
    void serverConnected() override;

    void serverDisconnected(int exit_code) override;
};


#endif /* SOURCE_FOCUS_FINDER_COMMON_INCLUDE_INDI_CLIENT_H_ */
