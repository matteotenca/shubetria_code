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

#ifndef FANCONTROLLERDATA_H
#define FANCONTROLLERDATA_H

#include "fanchanneldata.h"

#include <QObject>
#include <QString>
#include <QDateTime>

#include "fanprofiles.h"
#include "fanramp.h"
#include "timestampedtemperature.h"

class FanControllerData : public QObject
{
    Q_OBJECT

    class FanControllerState
    {
    public:
        FanControllerState();
        FanControllerProfile m_state;
        bool m_isSet;
    };

public:
    explicit FanControllerData(QObject *parent = 0);

    void connectSignals(void);

    const QString& name(void) const;

    void syncWithProfile(const FanControllerProfile& fcp);

    void storeCurrentState(void);

    const FanChannelData& fanChannelSettings(int channel) const
        { return m_channelSettings[channel]; }

    int channelCount(void) const
        { return sizeof(m_channelSettings) / sizeof(m_channelSettings[0]); }

    // Access functions for common settings
    bool isCelcius(void) const
        { return m_isCelcius != -1 ? m_isCelcius : false; }

    bool isAuto(void) const
        { return m_isAuto != -1 ? m_isAuto : false; }

    bool isAudibleAlarm(void) const
        { return m_isAudibleAlarm != -1 ? m_isAudibleAlarm : false; }

    bool isSoftwareAuto(void) const
        { return m_isSoftwareAuto; }

    bool isAutoSet(void) const
        { return m_isAuto != -1; }

    // Access functions to channel settings
    int maxRPM(int channel) const;
    int alarmTemp(int channel) const;

    int manualRPM(int channel) const;
    bool isManualRpmSet(int channel) const
        { return m_channelSettings[channel].isSet_manualRPM(); }

    inline int probeTemp(int probe) const;

    int lastTemp(int channel) const;
    int maxTemp(int channel) const;
    int minTemp(int channel) const;

    int lastRPM(int channel) const;
    int minLoggedRPM(int channel) const;
    int maxLoggedRPM(int channel) const;

    int percentageToRpm(int channel, int p, unsigned stepSize) const;
    int rpmToPercentage(int channel, int rpm) const;

    //
    void updateMaxRPM(int channel, int to, bool emitSignal = true);
    void updateAlarmTemp(int channel, int to, bool emitSignal = true);
    void updateManualRPM(int channel, int to, bool emitSignal = true);
    void updateTempF(int channel, int to, bool emitSignal = true);
    void doSoftwareAuto(int channel, int tempF);
    void doSoftwareAutoChannel(int channel, int tempF);
    int getSwAutoThreshold(int channel, int tDirection, int newRpm);
    void updateRPM(int channel, int to, bool emitSignal = true);
    void updateIsCelcius(bool isCelcius, bool emitSignal = true);
    void updateIsAuto(bool isAuto, bool emitSignal = true);
    void updateIsSwAuto(bool isSwAuto, bool forceToHWAutoWhenOff = false);
    void updateIsAudibleAlarm(bool isAudible, bool emitSignal = true);

    bool ramp_reqParamsForInitAreSet(void) const;

    QString currStatusAsText(void) const;

    FanSpeedRamp ramp(int channel) const
        { return m_ramp[channel]; }

    void setRamp(int channel, const FanSpeedRamp& ramp)
        {
            m_ramp[channel] = ramp;
            clearRampTemp(channel);
        }

    bool initRamp(int channel)
        { return m_ramp[channel].init(*this, channel, m_lastProfileId); }

    bool initRamp(int channel, int profileId)
        { return m_ramp[channel].init(*this, channel, profileId); }

    bool isRampInitialised(int channel)
        { return m_ramp[channel].isInitialised(); }

    bool isRampCustom(int channel)
        { return m_ramp[channel].isCustom(); }

    void initAllRamps(void);

    bool rampsReady(void) const
        { return m_rampsReady; }

    // Set channel settings
    // TODO: make these protected or even private
    void setMaxRPM(int channel, int to);
    void setAlarmTemp(int channel, int to);

    void setManualRPM(int channel, int to);

    void setLastTemp(int channel, int to);
    void setMinTemp(int channel, int to);
    void setMaxTemp(int channel, int to);

    void setLastRPM(int channel, int to);
    void setMinLoggedRPM(int channel, int to);
    void setMaxLoggedRPM(int channel, int to);
    // END TODO: make these protected or even private


    static QString temperatureString(int temperature, bool addScaleSymbol, bool isCelcius);
    QString temperatureString(int temperature, bool addScaleSymbol) const
    { return temperatureString(temperature, addScaleSymbol, m_isCelcius); }

    static int toCelcius(int tempInF);
    /**/
    static int toCelcius_ct(int tempInF);
    /**/
    static double toCelciusReal(int tempInF);
    static int toFahrenheit(int tempInC, double errCorr = 0);

    inline int toCurrTempScale(int tF) const;
    inline double toCurrTempScaleReal(int tF) const;

    void clearRampTemps(void);

    void clearAllChannelRpmAndTemp(void);

    const QString* targetRpmString(int channel, QString* dest);

protected:

    void clearMinMax(void);
    void clearRampTemp(int channel);

    void updateMinMax_temp(int channel, int t);
    void updateMinMax_temp_setvals(int channel, int t);
    void updateMinMax_rpm(int channel, int rpm);

private:

    // Common data
    short m_isCelcius;
    short m_isAuto;
    short m_isAudibleAlarm;

    FanChannelData m_channelSettings[FC_MAX_CHANNELS];
    FanSpeedRamp m_ramp[FC_MAX_CHANNELS];
    TimestampedTemperature m_rTemps[FC_MAX_CHANNELS];

    bool m_rampsReady;

    bool m_isSoftwareAuto;

    int m_lastProfileId;

    FanControllerState m_storedState;

signals:
    void deviceConnected(void);
    void deviceDisconnected(void);
    void temperature_changed(int channel, int temp);
    void minLoggedTemp_changed(int channel, int temp);
    void maxLoggedTemp_changed(int channel, int temp);
    void RPM_changed(int channel, int RPM);
    void minLoggedRPM_changed(int channel, int RPM);
    void maxLoggedRPM_changed(int channel, int RPM);
    void manualRPM_changed(int channel, int RPM);
    void maxRPM_changed(int channel, int RPM);
    void currentAlarmTemp_changed(int channel, int temp);
    void currentRpmOnAlarm_changed(int channel, int RPM);
    void temperatureScale_changed(bool isCelcius);
    void controlMode_changed (bool isAuto);
    void alarmIsAudible_changed (bool isAudibleAlarm);

    void gui_sync(void);

public slots:


    void onReset(void);
    void softReset(void);

};

int FanControllerData::probeTemp(int probe) const
{
    return m_channelSettings[probe].lastTemp();
}

int FanControllerData::toCurrTempScale(int tF) const
{
    return isCelcius() ? toCelcius(tF) : tF;
}

double FanControllerData::toCurrTempScaleReal(int tF) const
{
    return isCelcius() ? toCelciusReal(tF) : tF;
}


#endif // FANCONTROLLERDATA_H
