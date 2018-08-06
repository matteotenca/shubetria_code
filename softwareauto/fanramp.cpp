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

#include "fanramp.h"

#include <QDebug>

#include "fancontrollerdata.h"

FanSpeedRampParameters::FanSpeedRampParameters()
    : fixedRpm(false)
{}

int FanSpeedRampParameters::temperatureToRpm(int temperatureF, int maxRpm) const
{
    if (temperatureF < temperatureF_fanOn)
    {
        if (allowFanToTurnOff)
            return 0;
        else if (temperatureF <= temperatureF_rampStart)
            return minUsableRpm;
    }

    if (temperatureF < temperatureF_rampStart)
    {
        return minUsableRpm;
    }

    if (temperatureF <= temperatureF_rampMid)
    {
        if (temperatureF == temperatureF_rampStart)
            return speed_rampStart;
        if (temperatureF == temperatureF_rampMid)
            return speed_rampMid;

        QPoint a(temperatureF_rampStart, speed_rampStart);
        QPoint b(temperatureF_rampMid, speed_rampMid);

        return speedFromTemperatureLinear(temperatureF, a, b);
    }

    if (temperatureF < temperatureF_fanToMax)
    {
        QPoint a;
        QPoint b;

        if (temperatureF > temperatureF_rampEnd)
        {
            a.setX(temperatureF_rampEnd);
            a.setY(speed_rampEnd);
            b.setX(temperatureF_fanToMax);
            b.setY(maxRpm);
        }
        else
        {
            a.setX(temperatureF_rampMid);
            a.setY(speed_rampMid);
            b.setX(temperatureF_rampEnd);
            b.setY(speed_rampEnd);
        }

        return speedFromTemperatureLinear(temperatureF, a, b);
    }

    return maxRpm;
}

int FanSpeedRampParameters::speedFromTemperatureLinear(int temperatureF,
                                         const QPoint& a,
                                         const QPoint& b) const
{
    double speed;

    int deltax = b.x() - a.x();

    if (deltax == 0)
    {
        return FanSpeedRamp::snapToStepSize(b.y(), speedStepSize);
    }

    /* Adding speedStepSize/2 to deltay to ensure the ramp ends at ramp end
       ; i.e. we are using floor() to round the calcs and also rounding to
         closest "step size" (e.g. 100) and this can mean that ramp end speed
         may not be reached... adding speedStepSize/2 to make sure it does.
     */
    int deltay = b.y() - a.y() + speedStepSize/2;
    double m = (double)deltay/deltax;
    double yi = a.y() - m*a.x();

    speed = m * temperatureF + yi;

    speed  =  FanSpeedRamp::snapToStepSize(speed, speedStepSize);

    return speed;
}

FanSpeedRamp::FanSpeedRamp()
    : m_rampIsInitialised(false),
      m_isCustom(false),
      m_isModified(false),
      m_profileId(-1),
      m_channel(-1)
{

}

bool FanSpeedRamp::init(const FanControllerData& fcd,
                        int channel,
                        int profileId)
{
    if (profileId != -1)
    {
        // TODO: load from database if exists
    }
    return initWithDefaultData(fcd, channel);
}


bool FanSpeedRamp::initWithDefaultData(const FanControllerData& fcd, int channel)
{

    // Setup to be roughly equivalent to the Recon's built-in Auto

    m_channel                       = channel;

    m_rampParameters.speedStepSize           = 100;
    m_rampParameters.allowFanToTurnOff       = false;
    m_rampParameters.temperatureF_fanOn      = 68; // 68F = 20C;
    m_rampParameters.minUsableRpm            = snapToStepSize(fcd.maxRPM(channel) * 0.50, m_rampParameters.speedStepSize);
    m_rampParameters.maxUsableRpm            = fcd.maxRPM(channel);
    m_rampParameters.temperatureF_rampStart  = 68; // 68F = 20C;
    m_rampParameters.temperatureF_rampMid    = 122; // 104F = 50C
    m_rampParameters.temperatureF_rampEnd    = 140; // 140F = 60C
    m_rampParameters.temperatureF_fanToMax   = fcd.alarmTemp(channel);
    m_rampParameters.speed_fanOn             = m_rampParameters.minUsableRpm;
    m_rampParameters.speed_rampStart         = m_rampParameters.minUsableRpm;
    m_rampParameters.speed_rampEnd           = fcd.maxRPM(channel);
    m_rampParameters.speed_rampMid           = (m_rampParameters.minUsableRpm + m_rampParameters.speed_rampEnd) / 2;
    m_rampParameters.speed_rampMid           = snapToStepSize(m_rampParameters.speed_rampMid, m_rampParameters.speedStepSize);

    m_rampParameters.fixedRpm                = false;
    m_rampParameters.probeAffinity           = channel;

    m_rampParameters.tHysteresisUp           = 1;   // degrees F
    m_rampParameters.tHysteresisDown         = 2;   // degrees F
    m_rampParameters.tHysteresisFanOff       = 3;   // degrees F

    generateCurve(m_rampParameters, 0, 255, &m_ramp);

    m_isModified = false;
    m_isCustom = false;

    return true;
}

int FanSpeedRamp::temperatureToRpm(int tF) const
{
    if (!m_rampIsInitialised) return -1;

    int c = m_ramp.count();
    int rpm = 0;
    int i;
    for (i = 0; i < c; ++i)
    {
        if (m_ramp.at(i).x() >= tF)
        {
            rpm = m_ramp.at(i).y();
            break;
        }
    }
    if (i == c)
        rpm = m_rampParameters.maxUsableRpm;

    return rpm;
}

bool FanSpeedRamp::generateCurve(const FanSpeedRampParameters& fanCurveData,
                             int tempRangeMin,
                             int tempRangeMax,
                             QList<QPoint>* dest)
{
    dest->clear();

    int lastRpm = -1;
    int maxRpm = fanCurveData.maxUsableRpm;

    for (int i = tempRangeMin; i < tempRangeMax; ++i)
    {
        int rpm = fanCurveData.temperatureToRpm(i, maxRpm);
        if (rpm < fanCurveData.minUsableRpm)
        {
            if (fanCurveData.allowFanToTurnOff)
                rpm = 0;
            else
                rpm = fanCurveData.minUsableRpm;
        }

        if (fanCurveData.allowFanToTurnOff && i == fanCurveData.temperatureF_fanOn - 1)
        {
            dest->append(QPoint(i, 0));
        }
        else if (fanCurveData.allowFanToTurnOff && i == fanCurveData.temperatureF_fanOn)
        {
            dest->append(QPoint(i, minUsableRpm()));
        }
        else if (i == fanCurveData.temperatureF_rampStart)
        {
            if (i != 0)
                dest->append(QPoint(i-1, lastRpm));
            dest->append(QPoint(i, rpm));
        }
        else if (rpm != lastRpm
                || i == tempRangeMax-1)
        {
            dest->append(QPoint(i, rpm));
        }

        lastRpm = rpm;
    }

    if (lastRpm != maxRpm)
        dest->append(QPoint(tempRangeMax, maxRpm));

    m_rampIsInitialised = true;

    return true;
}
