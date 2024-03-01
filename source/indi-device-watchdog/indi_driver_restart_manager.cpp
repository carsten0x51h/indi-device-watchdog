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
#include <iostream>
#include <fstream>

#include "logging.h"
#include "indi_driver_restart_manager.h"


IndiDriverRestartManagerT::IndiDriverRestartManagerT() : restartTriggerLimit_(3), indiBinPath_("/usr/bin"), indiServerPipe_("/tmp/indiserverFIFO") {
}


IndiDriverRestartManagerT::IndiDriverRestartManagerT(int restartTriggerLimit, const std::string & indiBinPath, const std::string & indiServerPipe) : restartTriggerLimit_(restartTriggerLimit), indiBinPath_(indiBinPath), indiServerPipe_(indiServerPipe) {

}


void IndiDriverRestartManagerT::reset() {
  driverRestartMap_.clear();
}


void IndiDriverRestartManagerT::restart(const std::string & indiDriverName) {
  std::ofstream indiServerPipe(indiServerPipe_);
  
  if (indiServerPipe.is_open()) {
    std::filesystem::path indiDriverPath = indiBinPath_ / std::filesystem::path(indiDriverName);
    LOG(info) << "Restarting INDi driver '" << indiDriverPath.string() << "'..." << std::endl;
    
    indiServerPipe << "stop " << indiDriverPath.string() << std::endl;
    indiServerPipe << "start " << indiDriverPath.string() << std::endl;

    indiServerPipe.close();
  }
  else {
    LOG(error) << "ERROR: Cannot open INDI server pipe '" <<  indiServerPipe_ << "'." << std::endl;
  }
}


bool IndiDriverRestartManagerT::requestRestart(const std::string & indiDriverName) {
  auto it = driverRestartMap_.find(indiDriverName);
  bool restarted = false;

  if (it != driverRestartMap_.end()) {
    // Already known..
    bool isRestartConditionReached = (it->second >= restartTriggerLimit_ - 1);
    
    if (isRestartConditionReached) {
      restart(indiDriverName);
      restarted = true;

      // Reset the counter
      it->second = 0;
    }
    else {
      it->second++;
    }
  }
  else {
    // New entry
    // First request -> directly restart
    restart(indiDriverName);
    restarted = true;
    
    driverRestartMap_.insert(std::pair<std::string, int> (indiDriverName, 0));
  }

  return restarted;
}


void IndiDriverRestartManagerT::requestImmediateRestart(const std::string & indiDriverName) {
  restart(indiDriverName);
}
