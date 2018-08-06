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

#ifndef FANCHANNELDATA_H
#define FANCHANNELDATA_H

#include <QString>

#include "fanramp.h"
#include "averager.h"

class FanChannelData
{
public:
    FanChannelData();

    static const int rpmNotSetValue;
    static const int temperatureNotSetValue;
    static const int maxLoggedTempNotSetValue;
    static const int minLoggedTempNotSetValue;
    static const int manualRPMNotSetValue;
    static const int manualRPMNotSetValue_Preferred;

    //
    inline int maxRPM(void) const;
    inline int alarmTemp(void) const;
    inline int manualRPM(void) const;
    inline int lastTemp(void) const;
    inline int tempAveraged(void) const;
    inline bool tempAveraged_isOk(void) const;
    inline int maxTemp(void) const;
    inline int minTemp(void) const;
    inline int lastRPM(void) const;
    inline int minLoggedRPM(void) const;
    inline int maxLoggedRPM(void) const;

    //
    inline void setMaxRPM(int to);
    inline void setAlarmTemp(int to);
    inline void setManualRPM(int to);
    inline void setLastTemp(int to);
    inline void addTempAvgSample(int currentTempF);
    inline void setMinTemp(int to);
    inline void setMaxTemp(int to);
    inline void setLastRPM(int to);
    inline void setMinLoggedRPM(int to);
    inline void setMaxLoggedRPM(int to);

    //

    inline bool isSet_maxRpm(void) const;
    inline bool isSet_alarmTemp(void) const;
    inline bool isSet_manualRPM(void) const;
    inline bool isSet_lastTemp(void) const;
    inline bool isSet_MinTemp(void) const;
    inline bool isSet_MaxTemp(void) const;
    inline bool isSet_lastRPM(void) const;
    inline bool isSet_minLoggedRPM(void) const;
    inline bool isSet_maxLoggedRPM(void) const;
    inline static bool isUndefinedValue_manualRPM(int value);

    inline bool reqRampParamsAreSet(void) const;

    inline void clearRpmAndTemp(void);

private:
    int m_maxRPM;
    int m_alarmTemp;

    int m_lastTemp;
    int m_maxTemp;
    int m_minTemp;

    int m_lastRPM;
    int m_minLoggedRPM;
    int m_maxLoggedRPM;

    int m_manualRPM;

    Averager m_tempAverager;
};


int FanChannelData::maxRPM(void) const
{
    return m_maxRPM;
}

int FanChannelData::alarmTemp(void) const
{
    return m_alarmTemp;
}

int FanChannelData::manualRPM(void) const
{
    return m_manualRPM;
}

int FanChannelData::lastTemp(void) const
{
    return m_lastTemp;
}

int FanChannelData::tempAveraged(void) const
{
    return m_tempAverager.average();
}

bool FanChannelData::tempAveraged_isOk(void) const
{
    return m_tempAverager.sampleSetComplete();
}

int FanChannelData::maxTemp(void) const
{
    return m_maxTemp;
}

int FanChannelData::minTemp(void) const
{
    return m_minTemp;
}

int FanChannelData::lastRPM(void) const
{
    return m_lastRPM;
}

int FanChannelData::minLoggedRPM(void) const
{
    return m_minLoggedRPM;
}

int FanChannelData::maxLoggedRPM(void) const
{
    return m_maxLoggedRPM;
}

//
void FanChannelData::setMaxRPM(int to)
{
    m_maxRPM = to;
}

void FanChannelData::setAlarmTemp(int to)
{
    m_alarmTemp = to;
}

void FanChannelData::setManualRPM(int to)
{
    m_manualRPM = to;
}

void FanChannelData::setLastTemp(int to)
{
    m_lastTemp = to;
}

void FanChannelData::addTempAvgSample(int currentTempF)
{
    m_tempAverager.addSampleValue(currentTempF);
}

void FanChannelData::setMinTemp(int to)
{
    m_minTemp = to;
}

void FanChannelData::setMaxTemp(int to)
{
    m_maxTemp = to;
}

void FanChannelData::setLastRPM(int to)
{
    m_lastRPM = to;
}

void FanChannelData::setMinLoggedRPM(int to)
{
    m_minLoggedRPM = to;
}

void FanChannelData::setMaxLoggedRPM(int to)
{
    m_maxLoggedRPM = to;
}

//

bool FanChannelData::isSet_maxRpm(void) const
{
    return m_maxRPM != rpmNotSetValue;
}

bool FanChannelData::isSet_alarmTemp(void) const
{
    return m_alarmTemp != temperatureNotSetValue;
}

bool FanChannelData::isSet_manualRPM(void) const
{
    return m_manualRPM != manualRPMNotSetValue
           && m_manualRPM != manualRPMNotSetValue_Preferred;
}

bool FanChannelData::isSet_lastTemp(void) const
{
    return m_lastTemp != temperatureNotSetValue;
}

bool FanChannelData::isSet_MinTemp(void) const
{
    return m_minTemp != minLoggedTempNotSetValue;
}

bool FanChannelData::isSet_MaxTemp(void) const
{
    return m_maxTemp != maxLoggedTempNotSetValue;
}

bool FanChannelData::isSet_lastRPM(void) const
{
    return m_lastRPM != rpmNotSetValue;
}

bool FanChannelData::isSet_minLoggedRPM(void) const
{
    return m_minLoggedRPM != rpmNotSetValue;
}

bool FanChannelData::isSet_maxLoggedRPM(void) const
{
    return m_maxLoggedRPM != rpmNotSetValue;
}

bool FanChannelData::isUndefinedValue_manualRPM(int value)
{
    return !( value == manualRPMNotSetValue
              || value == manualRPMNotSetValue_Preferred );
}

bool FanChannelData::reqRampParamsAreSet(void) const
{
    return isSet_alarmTemp() && isSet_maxRpm();
}

void FanChannelData::clearRpmAndTemp(void)
{
    if (isSet_lastTemp())
        m_tempAverager.setAllSamplesToValue(m_lastTemp);
    else
        m_tempAverager.setAllSamplesToValue(0);

    m_lastTemp = temperatureNotSetValue;
    m_lastRPM = rpmNotSetValue;
    m_manualRPM = rpmNotSetValue;
    m_alarmTemp = temperatureNotSetValue;

}

#endif // FANCHANNELDATA_H
