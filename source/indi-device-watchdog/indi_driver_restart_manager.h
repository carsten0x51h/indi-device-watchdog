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

#ifndef SOURCE_INDI_DRIVER_RESTART_MANAGER_H_
#define SOURCE_INDI_DRIVER_RESTART_MANAGER_H_ SOURCE_INDI_DRIVER_RESTART_MANAGER_H_

#include <map>

class IndiDriverRestartManagerT {
 private:
  int restartTriggerLimit_;
  std::map<std::string, int> driverRestartMap_;
  std::string indiBinPath_;
  std::string indiServerPipe_;
  
  void restart(const std::string & indiDriverName);
  
 public:
  IndiDriverRestartManagerT();
  IndiDriverRestartManagerT(int restartTriggerLimit, const std::string & indiBinPath, const std::string & indiServerPipe);
  
  bool requestRestart(const std::string & indiDriverName);
  void requestImmediateRestart(const std::string & indiDriverName);
  void reset();
};

#endif /* SOURCE_INDI_DRIVER_RESTART_MANAGER_H_ */
