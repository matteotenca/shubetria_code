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

#ifndef PHOEBETRIA_FANSPEEDRAMP_H
#define PHOEBETRIA_FANSPEEDRAMP_H

#include <QList>
#include <QPoint>

#include <math.h>

// Fwd decls
class FanControllerData;

class FanSpeedRampParameters
{
public:

    friend class FanSpeedRamp;

    FanSpeedRampParameters();

    bool allowFanToTurnOff;
    int temperatureF_fanOn;
    int temperatureF_rampStart;
    int temperatureF_rampMid;
    int temperatureF_rampEnd;
    int temperatureF_fanToMax;
    int speed_fanOn;
    int speed_rampStart;
    int speed_rampMid;
    int speed_rampEnd;

    int minUsableRpm;
    int maxUsableRpm;
    int speedStepSize;
    bool fixedRpm;
    int probeAffinity;
    int tHysteresisUp;
    int tHysteresisDown;
    int tHysteresisFanOff;

protected:


    int temperatureToRpm(int temperatureF, int maxRpm) const;

    int speedFromTemperatureLinear(int temperatureF,
                                   const QPoint& a,
                                   const QPoint& b) const;
};

class FanSpeedRamp
{
public:

    FanSpeedRamp();

    inline bool isInitialised(void) const;

    inline bool isModified(void) const;

    inline bool isCustom(void) const;

    bool init(const FanControllerData& fcd, int channel, int profileId);

    inline const QList<QPoint>& ramp(void) const;

    inline int snapToStepSize(int rpm) const;

    inline static int snapToStepSize(int rpm, int stepSize);

    inline bool generateCurve(void);

    inline const FanSpeedRampParameters& rampParameters(void) const;

    inline bool allowFanToTurnOff(void) const;
    inline int  temperatureF_fanOn(void) const;
    inline int  temperatureF_rampStart(void) const;
    inline int  temperatureF_rampMid(void) const;
    inline int  temperatureF_rampEnd(void) const;
    inline int  temperatureF_fanToMax(void) const;
    inline int  speed_fanOn(void) const;
    inline int  speed_rampStart(void) const;
    inline int  speed_rampMid(void) const;
    inline int  speed_rampEnd(void) const;
    inline int  minUsableRpm(void) const;
    inline int  maxUsableRpm(void) const;
    inline int  speedStepSize(void) const;
    inline bool isFixedRpm(void) const;
    inline int  probeAffinity(void) const;
    inline int  hysteresisUp(void) const;
    inline int  hysteresisDown(void) const;
    inline int  hysteresisFanOff(void) const;

    int temperatureToRpm(int tF) const;


    // ----------- Set functions for setup data

    inline void    setIsCustom(bool isCustom = true);
    inline void    setIsModified(bool isModified);
    inline void    setIsInitialised(bool isInitialised);
    inline void    setAllowFanToTurnOff(bool allow);
    inline void    setTemperatureFanOn(int t);
    inline void    setTemperatureRampStart(int t);
    inline void    setTemperatureRampMid(int t);
    inline void    setTemperatureRampEnd(int t);
    inline void    setTemperatureFanToMax(int t);
    inline void    setSpeedFanOn(int rpm);
    inline void    setSpeedRampStart(int rpm);
    inline void    setSpeedRampMid(int rpm);
    inline void    setSpeedRampEnd(int rpm);

    inline void    setMinUsableRpm(int rpm);
    inline void    setMaxUsableRpm(int rpm);

    inline void    setSpeedStepSize(int step);

    inline void    setIsFixedRpm (bool isFixed);

    inline void    setProbeAffinity(int probeNumber);

    inline void    setTemperatureHysteresisUp(int tempInF);
    inline void    setTemperatureHysteresisDown(int tempInF);
    inline void    setTemperatureHysteresisFanOff(int tempInF);

    inline void    setHysteresisUp(int delta);
    inline void    setHysteresisDown(int delta);
    inline void    setHysteresisFanOff(int delta);

    // ----------- END Set functions for setup data

protected:

    bool initWithDefaultData(const FanControllerData& fcd, int channel);

    bool generateCurve(const FanSpeedRampParameters &fanCurveData,
                       int tempRangeMin,
                       int tempRangeMax,
                       QList<QPoint> *dest);

private:

    bool m_rampIsInitialised;
    bool m_isCustom;
    bool m_isModified;
    int m_profileId;        // profile that was used to init
    int m_channel;
    FanSpeedRampParameters m_rampParameters;
    QList<QPoint> m_ramp;
};

bool FanSpeedRamp::isInitialised(void) const
{
    return m_rampIsInitialised;
}

bool FanSpeedRamp::isModified(void) const
{
    return m_isModified;
}

bool FanSpeedRamp::isCustom(void) const
{
    return m_isCustom;
}

const QList<QPoint>& FanSpeedRamp::ramp(void) const
{
    return m_ramp;
}

int FanSpeedRamp::snapToStepSize(int rpm) const
{
    return snapToStepSize(rpm, m_rampParameters.speedStepSize);
}

int FanSpeedRamp::snapToStepSize(int rpm, int stepSize)
{
    return floor((double)rpm / stepSize) * stepSize;
}

bool FanSpeedRamp::generateCurve(void)
{
    return generateCurve(m_rampParameters, 0, 255, &m_ramp);
}

const FanSpeedRampParameters& FanSpeedRamp::rampParameters(void) const
{
    return m_rampParameters;
}

bool FanSpeedRamp::allowFanToTurnOff(void) const
{
    return m_rampParameters.allowFanToTurnOff;
}

int  FanSpeedRamp::temperatureF_fanOn(void) const
{
    return m_rampParameters.temperatureF_fanOn;
}

int  FanSpeedRamp::temperatureF_rampStart(void) const
{
    return m_rampParameters.temperatureF_rampStart;
}

int  FanSpeedRamp::temperatureF_rampMid(void) const
{
    return m_rampParameters.temperatureF_rampMid;
}

int  FanSpeedRamp::temperatureF_rampEnd(void) const
{
    return m_rampParameters.temperatureF_rampEnd;
}

int  FanSpeedRamp::temperatureF_fanToMax(void) const
{
    return m_rampParameters.temperatureF_fanToMax;
}

int  FanSpeedRamp::speed_fanOn(void) const
{
    return m_rampParameters.speed_fanOn;
}

int  FanSpeedRamp::speed_rampStart(void) const
{
    return m_rampParameters.speed_rampStart;
}

int  FanSpeedRamp::speed_rampMid(void) const
{
    return m_rampParameters.speed_rampMid;
}

int  FanSpeedRamp::speed_rampEnd(void) const
{
    return m_rampParameters.speed_rampEnd;
}

int  FanSpeedRamp::minUsableRpm(void) const
{
    return m_rampParameters.minUsableRpm;
}

int  FanSpeedRamp::maxUsableRpm(void) const
{
    return m_rampParameters.maxUsableRpm;
}

int  FanSpeedRamp::speedStepSize(void) const
{
    return m_rampParameters.speedStepSize;
}

bool FanSpeedRamp::isFixedRpm(void) const
{
    return m_rampParameters.fixedRpm;
}

int  FanSpeedRamp::probeAffinity(void) const
{
    return m_rampParameters.probeAffinity;
}

int  FanSpeedRamp::hysteresisUp(void) const
{
    return m_rampParameters.tHysteresisUp;
}

int  FanSpeedRamp::hysteresisDown(void) const
{
    return m_rampParameters.tHysteresisDown;
}

int  FanSpeedRamp::hysteresisFanOff(void) const
{
    return m_rampParameters.tHysteresisFanOff;
}


/*------------------------------------------------------------------------
  Set functions for setup data
  -----------------------------------------------------------------------*/

void FanSpeedRamp::setIsCustom(bool isCustom)
{
    m_isCustom = isCustom;
}

void FanSpeedRamp::setIsModified(bool isModified)
{
    m_isModified = isModified;
}

void FanSpeedRamp::setIsInitialised(bool isInitialised)
{
    m_rampIsInitialised = isInitialised;
}


void FanSpeedRamp::setAllowFanToTurnOff(bool allow)
{
    m_isModified = true;
    m_rampParameters.allowFanToTurnOff = allow;
}

void FanSpeedRamp::setTemperatureFanOn(int t)
{
    m_isModified = true;
    m_rampParameters.temperatureF_fanOn = t;
}

void FanSpeedRamp::setTemperatureRampStart(int t)
{
    m_isModified = true;
    m_rampParameters.temperatureF_rampStart = t;
}

void FanSpeedRamp::setTemperatureRampMid(int t)
{
    m_isModified = true;
    m_rampParameters.temperatureF_rampMid = t;
}

void FanSpeedRamp::setTemperatureRampEnd(int t)
{
    m_isModified = true;
    m_rampParameters.temperatureF_rampEnd = t;
}

void FanSpeedRamp::setTemperatureFanToMax(int t)
{
    m_isModified = true;
    m_rampParameters.temperatureF_fanToMax = t;
}

void    FanSpeedRamp::setSpeedFanOn(int rpm)
{
    m_isModified = true;
    m_rampParameters.speed_fanOn = rpm;
}

void FanSpeedRamp::setSpeedRampStart(int rpm)
{
    m_isModified = true;
    m_rampParameters.speed_rampStart = rpm;
}

void FanSpeedRamp::setSpeedRampMid(int rpm)
{
    m_isModified = true;
    m_rampParameters.speed_rampMid = rpm;
}

void FanSpeedRamp::setSpeedRampEnd(int rpm)
{
    m_isModified = true;
    m_rampParameters.speed_rampEnd = rpm;
}

void FanSpeedRamp::setMinUsableRpm(int rpm)
{
    m_isModified = true;
    m_rampParameters.minUsableRpm = rpm;
}

void FanSpeedRamp::setMaxUsableRpm(int rpm)
{
    m_isModified = true;
    m_rampParameters.maxUsableRpm = rpm;
}

void FanSpeedRamp::setSpeedStepSize(int step)
{
    m_isModified = true;
    m_rampParameters.speedStepSize = step;
}

void FanSpeedRamp::setIsFixedRpm (bool isFixed)
{
    m_isModified = true;
    m_rampParameters.fixedRpm = isFixed;
}

void FanSpeedRamp::setProbeAffinity(int probeNumber)
{
    m_isModified = true;
    m_rampParameters.probeAffinity = probeNumber;
}

void FanSpeedRamp::setHysteresisUp(int delta)
{
    m_isModified = true;
    m_rampParameters.tHysteresisUp = delta;
}

void FanSpeedRamp::setHysteresisDown(int delta)
{
    m_isModified = true;
    m_rampParameters.tHysteresisDown = delta;
}

void FanSpeedRamp::setHysteresisFanOff(int delta)
{
    m_isModified = true;
    m_rampParameters.tHysteresisFanOff = delta;
}


/*------------------------------------------------------------------------
  END Set functions for setup data
  -----------------------------------------------------------------------*/

#endif // PHOEBETRIA_FANSPEEDRAMP_H
