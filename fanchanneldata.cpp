/*  Copyright 2012 Craig Robbins and Christopher Ferris

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cmath>
#include "fanchanneldata.h"


const int FanChannelData::rpmNotSetValue = -1;
const int FanChannelData::temperatureNotSetValue = 65535;
const int FanChannelData::maxLoggedTempNotSetValue = 65535;
const int FanChannelData::minLoggedTempNotSetValue = 63335;
const int FanChannelData::manualRPMNotSetValue = -1;
// Keeping -1 so that V1.0.0-Beta profiles do not break.
const int FanChannelData::manualRPMNotSetValue_Preferred = 65535;


FanChannelData::FanChannelData()
{
    m_lastTemp      = temperatureNotSetValue;
    m_maxLoggedRPM  = rpmNotSetValue;
    m_lastRPM       = rpmNotSetValue;
    m_minLoggedRPM  = rpmNotSetValue;
    m_maxTemp       = maxLoggedTempNotSetValue;
    m_minTemp       = minLoggedTempNotSetValue;
    m_alarmTemp     = temperatureNotSetValue;
    m_maxRPM        = rpmNotSetValue;
    m_manualRPM     = manualRPMNotSetValue_Preferred;
}

