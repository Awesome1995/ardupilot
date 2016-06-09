/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * AP_IRLock_SITL.cpp
 *
 *  Created on: June 09, 2016
 *      Author: Ian Chen
 */
#include <AP_HAL/AP_HAL.h>

#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
#include "AP_IRLock_SITL.h"

#include <fcntl.h>
#include <unistd.h>

#include <iostream>

extern const AP_HAL::HAL& hal;

AP_IRLock_SITL::AP_IRLock_SITL() :
    _last_timestamp(0),
    sock(true)
{}

void AP_IRLock_SITL::init()
{
    // try to bind to a specific port so that if we restart ArduPilot
    // Gazebo keeps sending us packets. Not strictly necessary but
    // useful for debugging
    sock.bind("127.0.0.1", 9005);

    sock.reuseaddress();
    sock.set_blocking(false);

    hal.console->printf("AP_IRLock_SITL::init()\n");

    _flags.healthy = true;
}

// retrieve latest sensor data - returns true if new data is available
bool AP_IRLock_SITL::update()
{
    // return immediately if not healthy
    if (!_flags.healthy) {
        return false;
    }
    // my additions:S
    irlock_packet pkt;
    uint16_t count = 0;
    /*
      we re-send the servo packet every 0.1 seconds until we get a
      reply. This allows us to cope with some packet loss to the FDM
     */
    size_t s = sock.recv(&pkt, sizeof(pkt), 5);
    if (s == sizeof(pkt) && pkt.timestamp >_last_timestamp) {
        _target_info[count].timestamp = pkt.timestamp;
        _target_info[count].target_num = pkt.target_num;
        _target_info[count].angle_x = pkt.angle_x;
        _target_info[count].angle_y = pkt.angle_y;
        _target_info[count].size_x = pkt.size_x;
        _target_info[count].size_y = pkt.size_y;
        count++;
        _last_timestamp = pkt.timestamp;
        _last_update = AP_HAL::millis();
    }

    // update num_blocks and implement timeout
    if (count > 0) {
        _num_targets = count;
    } else if ((AP_HAL::millis() - _last_update) > IRLOCK_TIMEOUT_MS) {
        _num_targets = 0;
    }

    // return true if new data found
    return (_num_targets > 0);
}

#endif // CONFIG_HAL_BOARD == HAL_BOARD_SITL
