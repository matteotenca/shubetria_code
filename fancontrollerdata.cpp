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

#include "fancontrollerdata.h"
#include "fanchanneldata.h"
#include "shubetriaapp.h"

// TODO: hack
#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))

#include <QDebug>


#define SHOW_SW_WAUTO_DETAILS

FanControllerData::FanControllerState::FanControllerState() :
    m_isSet(false)
{
}

FanControllerData::FanControllerData(QObject *parent)
    : QObject(parent),
      m_isCelcius(-1),
      m_isAuto(-1),
      m_isAudibleAlarm(-1),
      m_rampsReady(false),
      m_isSoftwareAuto(false),
      m_lastProfileId(-1)
{
    clearRampTemps();
}

void FanControllerData::connectSignals(void)
{
    connect(&ph_dispatcher(), SIGNAL(refresh_critical()),
            this, SLOT(onReset()));
    connect(&ph_dispatcher(), SIGNAL(refresh_low()),
            this, SLOT(softReset()));
}

void FanControllerData::syncWithProfile(const FanControllerProfile& fcp)
{
    m_isCelcius = fcp.isCelcius();
    m_isAuto = fcp.isAuto();
    m_isAudibleAlarm = fcp.isAudibleAlarm();
    m_isSoftwareAuto = fcp.isSoftwareAuto();

    for (int i = 0; i < FC_MAX_CHANNELS; i++)
    {
        const BasicChannelData& fcs = fcp.getChannelSettings(i);
        setAlarmTemp(i, fcs.alarmTemp);
        setManualRPM(i, fcs.speed);
        setRamp(i, fcp.ramp(i));
    }
}


void FanControllerData::storeCurrentState(void)
{
    m_storedState.m_state.setFromCurrentData(*this);
    m_storedState.m_isSet = true;
}

// ------------------------------------------------------------------------
//  Access functions to channel settings
// ------------------------------------------------------------------------
int FanControllerData::maxRPM(int channel) const
{
    return m_channelSettings[channel].maxRPM();
}

int FanControllerData::alarmTemp(int channel) const
{
    return m_channelSettings[channel].alarmTemp();
}

int FanControllerData::manualRPM(int channel) const
{
    return m_channelSettings[channel].manualRPM();
}

int FanControllerData::lastTemp(int channel) const
{
    return m_channelSettings[channel].lastTemp();
}

int FanControllerData::maxTemp(int channel) const
{
    return m_channelSettings[channel].maxTemp();
}

int FanControllerData::minTemp(int channel) const
{
    return m_channelSettings[channel].minTemp();
}

int FanControllerData::lastRPM(int channel) const
{
    return m_channelSettings[channel].lastRPM();
}

int FanControllerData::minLoggedRPM(int channel) const
{
    return m_channelSettings[channel].minLoggedRPM();
}

int FanControllerData::maxLoggedRPM(int channel) const
{
    return m_channelSettings[channel].maxLoggedRPM();
}

/* Returns the percentage p as an rpm. The rpm will be a multiple of stepSize.

   pre: 0 <= channel <= 4
   pre: 0 <= p <= 100
   pre: stepsize > 0

   Note: The RPM returned may not necessarily be a settable RPM; i.e. the
         RPM may be less than the fan is able to spin at. It is up to the
         calling function to check for this condition.
 */
int FanControllerData::percentageToRpm(int channel, int p, unsigned stepSize) const
{
    // If channel max RPM has not been read from the Recon yet just return 0
    if (!m_channelSettings[channel].isSet_maxRpm())
        return 0;

    int maxRpm = m_channelSettings[channel].maxRPM();

    double rpm = p / 100.0 * maxRpm;

    // return as a multiple of the rpm step size
    return int(rpm / (double)stepSize) * stepSize;
}


/* Returns the rpm as an integer percentage (0-100) of the channels maximum
   settable fan speed as reported by the Recon.

   pre: 0 <= channel <= 4
 */
int FanControllerData::rpmToPercentage(int channel, int rpm) const
{
    // If channel max RPM has not been read from the Recon yet just return 0
    if (!m_channelSettings[channel].isSet_maxRpm())
        return 0;

    int maxRpm = m_channelSettings[channel].maxRPM();

    if (maxRpm == 0)
        return 0;

    return maxRpm == RECON_MAXRPM || rpm > maxRpm
            ? 100                           // max
            : ceil(rpm * 100.0 / maxRpm);   // Calculated percentage
}


// ------------------------------------------------------------------------
// Update channel settings
// ------------------------------------------------------------------------

void FanControllerData::updateMaxRPM(int channel, int to, bool emitSignal)
{
    FanChannelData& cd = m_channelSettings[channel];
    if (cd.maxRPM() != to || !cd.isSet_maxRpm())
    {
        cd.setMaxRPM(to);
        if (emitSignal) emit maxRPM_changed(channel, to);
    }
}

void FanControllerData::updateAlarmTemp(int channel, int to, bool emitSignal)
{
    FanChannelData& cd = m_channelSettings[channel];
    if (cd.alarmTemp() != to || !cd.isSet_alarmTemp())
    {
        cd.setAlarmTemp(to);
        if (emitSignal) emit currentAlarmTemp_changed(channel, to);
    }
}

void FanControllerData::updateManualRPM(int channel, int to, bool emitSignal)
{
    FanChannelData& cd = m_channelSettings[channel];

    if (to == 0xffff) return; // Ignore (0xffff is "not set")

    if ( cd.manualRPM() != to || !cd.isSet_manualRPM())
    {
        cd.setManualRPM(to);
        if (emitSignal) emit manualRPM_changed(channel, to);
    }
}

void FanControllerData::updateTempF(int channel, int to, bool emitSignal)
{

    FanChannelData& cd = m_channelSettings[channel];

    cd.addTempAvgSample(to);

    if (cd.lastTemp() != to || !cd.isSet_lastTemp())
    {
        cd.setLastTemp(to);
        if (emitSignal) emit temperature_changed(channel, to);

        updateMinMax_temp(channel, to);
    }

    if (m_isSoftwareAuto)
    {
        int tempToUse;
        tempToUse = cd.tempAveraged_isOk() ? cd.tempAveraged() : to;

        // qDebug() << "Channel" << channel << "temp:" << tempToUse;
        doSoftwareAuto(channel, tempToUse);
    }
}

void FanControllerData::doSoftwareAuto(int channel, int tempF)
{
    for (int i = 0; i < FC_MAX_CHANNELS; ++i)
    {
        if (isRampInitialised(i) && m_ramp[i].probeAffinity() == channel)
        {
            doSoftwareAutoChannel(i, tempF);
        }
    }
}

void FanControllerData::doSoftwareAutoChannel(int channel, int tempF)
{
    FanChannelData& cd = m_channelSettings[channel];

    int currRpm;
    int newRpm = m_ramp[channel].temperatureToRpm(tempF);
    if (newRpm == -1) return;   // Do nothing if ramp not initialised

    int rDelta;
    int direction;
    int threshold;

    if (!m_rTemps[channel].isSet())
    {
        rDelta = direction = 0;
        threshold = 0;
        currRpm = 0;
    }
    else
    {
        rDelta = m_rTemps[channel].temperature() - tempF;
        direction = rDelta > 0 ? -1 : 1;
        rDelta = abs(rDelta);
        currRpm = m_ramp[channel].temperatureToRpm(m_rTemps[channel].temperature());

        threshold = getSwAutoThreshold(channel, direction, newRpm);
    }

    if (rDelta >= threshold || !m_rTemps[channel].isSet())
    {
        if (newRpm != currRpm
                || !m_rTemps[channel].isSet()
                || !cd.isSet_manualRPM() )
        {
            updateMinMax_rpm(channel, newRpm);
            m_rTemps[channel].setTemperature(tempF); // Save tF for next time

            ph_fanControllerIO().setChannelSettings(channel,
                                                    alarmTemp(channel),
                                                    newRpm);

#if defined QT_DEBUG && defined SHOW_SW_WAUTO_DETAILS
            if (newRpm == 0)
                qDebug() << "Turning fan off. Threshold:" << threshold;

                qDebug() << "S/W Auto -- Channel/Temp/RPM ="
                         << channel << tempF << newRpm;
#endif

        }
    }
}

int FanControllerData::getSwAutoThreshold(int channel, int tDirection, int newRpm)
{
    int threshold;

    if (tDirection < 0)
    {
        threshold = newRpm == 0 ? m_ramp[channel].hysteresisFanOff()
                                : m_ramp[channel].hysteresisDown();
    }
    else
    {
        threshold = m_ramp[channel].hysteresisUp();
    }

    return threshold;
}


void FanControllerData::updateRPM(int channel, int to, bool emitSignal)
{
    FanChannelData& cd = m_channelSettings[channel];
    if (cd.lastRPM() != to || !cd.isSet_lastRPM())
    {
        updateMinMax_rpm(channel, to);
        cd.setLastRPM(to);
        if (emitSignal) emit RPM_changed(channel, to);
    }
}


void FanControllerData::updateIsCelcius(bool isCelcius, bool emitSignal)
{
    if (m_isCelcius != (short)isCelcius || m_isCelcius == -1)
    {
        m_isCelcius = isCelcius;
        if (emitSignal) emit temperatureScale_changed(isCelcius);
    }
}

void FanControllerData::updateIsAuto(bool isAuto, bool emitSignal)
{
    if (m_isAuto != (short)isAuto || m_isAuto == -1)
    {
        m_isAuto = (short)isAuto;

        m_isSoftwareAuto = false;

        if (emitSignal) emit controlMode_changed(isAuto);
    }
}


void FanControllerData::updateIsSwAuto(bool isSwAuto, bool forceToHWAutoWhenOff)
{
    if (isSwAuto)
    {
        // Turn SWA on

        if (!m_isSoftwareAuto)
        {
            /* NOTE: It may not at first make sense to turn s/ware auto on
                     if it's already on, however this function may be
                     called after a sleep/wake cycle of the OS to make
                     sure everything is set correctly. If that's the case
                     it's correct that these actions are undertaken,
                     *except* for storing the pre-state; the pre-state
                     would have been stored before the sleep/wake so don't
                     re-store it here because the current state of the Recon
                     most-likely does not represent the state before the OS
                     slept.
            */

            // Store current state
            storeCurrentState();
        }

        // Set to manual
        ph_fanControllerIO().setDeviceFlags(
                    isCelcius(),
                    false,          // Hardware Manual
                    isAudibleAlarm());

        clearRampTemps();
        m_isSoftwareAuto = true;
        m_isAuto = false;
    }
    else
    {
        // Turn SWA off
        if (m_storedState.m_isSet && !forceToHWAutoWhenOff)
        {
            if (ph_fanControllerIO().setFromProfile(m_storedState.m_state))
            {
                syncWithProfile(m_storedState.m_state);
            }
            m_storedState.m_isSet = false;
        }
        else
        {
            // No pre-SWA state stored, just set to Auto
            ph_fanControllerIO().setDeviceFlags(
                        isCelcius(),
                        true,           // Hardware Auto
                        isAudibleAlarm());
            m_isAuto = true;
        }
        m_isSoftwareAuto = false;
    }
}

void FanControllerData::updateIsAudibleAlarm(bool isAudible, bool emitSignal)
{
    if (m_isAudibleAlarm != (short)isAudible || m_isAudibleAlarm == -1)
    {
        m_isAudibleAlarm = isAudible;
        if (emitSignal) emit alarmIsAudible_changed(isAudible);
    }
}

void FanControllerData::clearMinMax(void)
{
    for (int i = 0; i < FC_MAX_CHANNELS; ++i)
    {
        FanChannelData& cd = m_channelSettings[i];
        cd.setMinTemp(FanChannelData::minLoggedTempNotSetValue);
        cd.setMaxTemp(FanChannelData::maxLoggedTempNotSetValue);
        cd.setMinLoggedRPM(FanChannelData::rpmNotSetValue);
        cd.setMaxLoggedRPM(FanChannelData::rpmNotSetValue);
    }
}

void FanControllerData::clearRampTemps(void)
{
    if (!m_isSoftwareAuto)
        return;

    for (int i = 0; i < FC_MAX_CHANNELS; ++i)
        m_rTemps[i].clear();
}

void FanControllerData::clearRampTemp(int channel)
{
    m_rTemps[channel].clear();
}

void FanControllerData::updateMinMax_temp(int channel, int t)
{

    if (isSoftwareAuto())
    {
        for (int i = 0; i < FC_MAX_CHANNELS; ++i)
        {
            if (m_ramp[i].probeAffinity() == channel)
                updateMinMax_temp_setvals(i, t);
        }
    }
    else
    {
        updateMinMax_temp_setvals(channel, t);
    }
}

void FanControllerData::updateMinMax_temp_setvals(int channel, int t)
{
    FanChannelData& cd = m_channelSettings[channel];

    if (cd.minTemp() > t || !cd.isSet_MinTemp())
    {
        cd.setMinTemp(t);
        emit minLoggedTemp_changed(channel, t);
    }
    if (cd.maxTemp() < t || !cd.isSet_MaxTemp())
    {
        cd.setMaxTemp(t);
        emit maxLoggedTemp_changed(channel, t);
    }
}

void FanControllerData::clearAllChannelRpmAndTemp(void)
{
    for (int i = 0; i < FC_MAX_CHANNELS; ++i)
        m_channelSettings[i].clearRpmAndTemp();
}

void FanControllerData::updateMinMax_rpm(int channel, int rpm)
{
    FanChannelData& cd = m_channelSettings[channel];
    if (cd.minLoggedRPM() > rpm || !cd.isSet_minLoggedRPM())
    {
        cd.setMinLoggedRPM(rpm);
        emit minLoggedRPM_changed(channel, rpm);
    }
    if ((cd.maxLoggedRPM() < rpm || !cd.isSet_maxLoggedRPM()) && rpm != RECON_MAXRPM)
    {
        cd.setMaxLoggedRPM(rpm);
        emit maxLoggedRPM_changed(channel, rpm);
    }
}

const QString* FanControllerData::targetRpmString(int channel, QString* dest)
{
    if (isAuto())
    {
        *dest = QString(tr("AUTO"));
    }
    else
    {
        int targetRPM = manualRPM(channel);

        if (targetRPM == RECON_MAXRPM)
        {
            *dest = QString(tr("MAX"));
        }
        else
        {
            if (targetRPM == FanChannelData::rpmNotSetValue)
                *dest = "?";
            else if (targetRPM == 0)
                *dest = QString(tr("OFF"));
            else
                *dest = QString::number(targetRPM);
        }
    }
    return dest;
}

// ------------------------------------------------------------------------
// Set channel settings
// ------------------------------------------------------------------------

void FanControllerData::setMaxRPM(int channel, int to)
{
    m_channelSettings[channel].setMaxRPM(to);
}

void FanControllerData::setAlarmTemp(int channel, int to)
{
    m_channelSettings[channel].setAlarmTemp(to);
}

void FanControllerData::setManualRPM(int channel, int to)
{
    m_channelSettings[channel].setManualRPM(to);
}

void FanControllerData::setLastTemp(int channel, int to)
{
    m_channelSettings[channel].setLastTemp(to);
}

void FanControllerData::setMinTemp(int channel, int to)
{
    m_channelSettings[channel].setMinTemp(to);
}

void FanControllerData::setMaxTemp(int channel, int to)
{
    m_channelSettings[channel].setMaxTemp(to);
}

void FanControllerData::setLastRPM(int channel, int to)
{
    m_channelSettings[channel].setLastRPM(to);
}

void FanControllerData::setMinLoggedRPM(int channel, int to)
{
    m_channelSettings[channel].setMinLoggedRPM(to);
}

void FanControllerData::setMaxLoggedRPM(int channel, int to)
{
    m_channelSettings[channel].setMaxLoggedRPM(to);
}

QString FanControllerData::temperatureString(int temperature,
                                             bool addScaleSymbol,
                                             bool isCelcius)
{
    QString r;
    int t = isCelcius ? toCelcius(temperature) : temperature;
    r = QString::number(t);
    if (addScaleSymbol) r += (isCelcius ? " C" : " F");
    return r;
}

/* Note:    Due to the conversion being somewhat specific for the fan controller
 *          device this should not be made a general (global) function; i.e.
 *          the rounding (or potential rounding) etc. make the function
 *          unsuitable for general purpose use.
 */
int FanControllerData::toCelcius(int tempInF)
{
    return round((tempInF-32)*5.0/9);
}

/* it seems thre is a -10 °C delta between coretemp and displayed data. Little hack.
   TODO: understand this topic
       */

int FanControllerData::toCelcius_ct(int tempInF)
{
    return (10+round((tempInF-32)*5.0/9));
}

double FanControllerData::toCelciusReal(int tempInF)
{
    return (tempInF-32)*5.0/9;
}

/* Note:    Due to the conversion being somewhat specific for the fan controller
 *          device this should not be made a general (global) function; i.e.
 *          the rounding (or potential rounding) etc. make the function
 *          unsuitable for general purpose use.
 */
int FanControllerData::toFahrenheit(int tempInC, double errCorr)
{
    return round( (tempInC + errCorr) * 9/5.0 + 32);
}

bool FanControllerData::ramp_reqParamsForInitAreSet(void) const
{
    int cc = channelCount();
    bool r = true;

    for (int i = 0; i < cc; ++i)
    {
        if (!m_channelSettings[i].reqRampParamsAreSet())
        {
            r = false;
            break;
        }
    }
    return r;
}

QString FanControllerData::currStatusAsText(void) const
{
    QString s;

    if (ph_fanControllerIO().isConnected())
        s = "Connected";
    else
    {
        s = "Not connected!";
        return s;
    }

    for (int i = 0; i < FC_MAX_CHANNELS; ++i)
    {
        if (i != FC_MAX_CHANNELS)
            s += "\n";
        s += "Ch " + QString::number(i + 1) + ": ";
        s += temperatureString(lastTemp(i), true) + "; ";
        s += QString::number(lastRPM(i)) + " RPM";

    }

    return s;
}


void FanControllerData::initAllRamps(void)
{
    int cc = channelCount();

    for (int i = 0; i < cc; ++i)
    {
        if (!isRampInitialised(i))
            initRamp(i);
    }

    m_rampsReady = true;
}

void FanControllerData::onReset(void)
{

    bool bs = ph_dispatcher().blockSignals(true);
    bool bs2 = ph_fanControllerIO().blockSignals(true);
    bool bs3 = blockSignals(true);

    ph_fanControllerIO().reset();

    if (m_isSoftwareAuto)
    {
        // Set to Auto to force Recon to recalibrate itself
        //
//        ph_fanControllerIO().setDeviceFlags(
//                    m_isCelcius,
//                    true,
//                    isAudibleAlarm());
    }
    updateIsSwAuto(m_isSoftwareAuto);
    clearMinMax();
    clearAllChannelRpmAndTemp();
    clearRampTemps();

    blockSignals(bs3);
    ph_fanControllerIO().blockSignals(bs2);
    ph_dispatcher().blockSignals(bs);

    emit gui_sync();

#if defined QT_DEBUG
    qDebug() << "FanControllerData::onReset() called";
#endif
}

void FanControllerData::softReset(void)
{
    clearAllChannelRpmAndTemp();
    clearRampTemps();

    emit gui_sync();
}


