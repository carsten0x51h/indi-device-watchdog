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

#include <iostream>
#include <filesystem>
#include <fstream>

#include "indi_driver_restart_manager.h"


IndiDriverRestartManagerT::IndiDriverRestartManagerT() : restartTriggerLimit_(3) {
}


IndiDriverRestartManagerT::IndiDriverRestartManagerT(int restartTriggerLimit) : restartTriggerLimit_(restartTriggerLimit) {
}


void IndiDriverRestartManagerT::reset() {
  driverRestartMap_.clear();
}


void IndiDriverRestartManagerT::restart(const std::string & indiDriverName) {
  std::string indiBinPath = "/usr/bin"; // TODO: Make configurable...
  std::filesystem::path indiServerPipePath = "/tmp/indiserverFIFO"; // TODO: make configurable...
  std::ofstream indiServerPipe(indiServerPipePath);
  
  if (indiServerPipe.is_open()) {
    std::filesystem::path indiDriverPath = indiBinPath / std::filesystem::path(indiDriverName);
    std::cerr << "Restarting INDi driver '" << indiDriverPath.string() << "'..." << std::endl;
    
    indiServerPipe << "stop " << indiDriverPath.string() << std::endl;
    indiServerPipe << "start " << indiDriverPath.string() << std::endl;

    indiServerPipe.close();
  }
  else {
    std::cerr << "ERROR: Cannot open INDI server pipe '" <<  indiServerPipePath << "'." << std::endl;
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
