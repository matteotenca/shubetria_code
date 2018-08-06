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

#ifndef PHOEBETRIA_FAN_PROFILES_H
#define PHOEBETRIA_FAN_PROFILES_H

#include <QString>
#include <QSettings>
#include "bfx-recon/bfxrecon.h"
#include "fanchanneldata.h"

class FanControllerData;
class QDir;

typedef struct
{
    int alarmTemp;
    int speed;
} BasicChannelData;


class FanControllerProfile
{
public:

    friend class MainDb;

    FanControllerProfile();
    FanControllerProfile(const QString& name, const QString& desc);

    QString defualtProfileLocation(void) const;

    static QStringList getProfileNames(void);
    static NameAndControlModeList getProfileNamesAndModes(void);

    void setProfileName(const QString& name);
    void setFilenameAndPath(const QString& filenameAndPath);

    void setFromCurrentData(const FanControllerData& data);

    bool save(const QString& profileName);
    bool load(const QString& profileName);

    inline QString profileDescription(void);
    QString profileDescription(const QString& profileName);

    static int importFromIni(QDir &dir);

    static bool erase(const QString& profileName);

    bool isCelcius(void) const;
    bool isAuto(void) const;
    bool isAudibleAlarm(void) const;
    bool isSoftwareAuto(void) const;
    int alarmTemp(int channel) const;
    int speed(int channel) const;

    QString htmlReport(void);

    const BasicChannelData& getChannelSettings(int channel) const;

    inline static bool isReservedProfileName(const QString& name);

    inline static const QString& reservedProfileNameStartChars(void);

    const QString& profileName(void) const;
    const QString& description(void) const;
    const FanSpeedRamp& ramp(int channel) const;

    void setDescription(const QString& desc);

    inline  void setRamp(int channel, const FanSpeedRamp& other);

protected:

    void initCommon(void);

    static bool loadFromIni(const QString& filenameAndPath, FanControllerProfile &dest);

    QString htmlReportCommon(void) const;
    QString htmlReportManual(void) const;
    QString htmlReportSwAuto(void) const;

    QString boolToText(bool isTrue) const;
    QString boolToTempScale(bool isCelcius) const;

private:

    QString m_defaultProfileLocation;

    QString m_name;
    QString m_description;
    QString m_fileNameAndPath;

    bool m_isCelcius;
    bool m_isAuto;
    bool m_isAudibleAlarm;
    bool m_isSoftwareAuto;

    BasicChannelData m_channelSettings[FC_MAX_CHANNELS];
    FanSpeedRamp m_ramp[FC_MAX_CHANNELS];

};


inline bool FanControllerProfile::isCelcius(void) const
{
    return m_isCelcius;
}

inline bool FanControllerProfile::isAuto(void) const
{
    return m_isAuto;
}

inline bool FanControllerProfile::isAudibleAlarm(void) const
{
    return m_isAudibleAlarm;
}

inline bool FanControllerProfile::isSoftwareAuto(void) const
{
    return m_isSoftwareAuto;
}

inline const BasicChannelData&
    FanControllerProfile::getChannelSettings(int channel) const
{
    return m_channelSettings[channel];
}

inline bool FanControllerProfile::isReservedProfileName(const QString& name)
{
    return name.left(reservedProfileNameStartChars().length())
                == reservedProfileNameStartChars();
}

inline const QString& FanControllerProfile::reservedProfileNameStartChars(void)
{
    static QString chrs = "__";
    return chrs;
}

inline const QString& FanControllerProfile::profileName(void) const
{
    return m_name;
}

inline const QString& FanControllerProfile::description(void) const
{
    return m_description;
}

inline const FanSpeedRamp& FanControllerProfile::ramp(int channel) const
{
    return m_ramp[channel];
}

inline void FanControllerProfile::setDescription(const QString& desc)
{
    m_description = desc;
}

inline int FanControllerProfile::alarmTemp(int channel) const
{
    return m_channelSettings[channel].alarmTemp;
}

inline int FanControllerProfile::speed(int channel) const
{
    return m_channelSettings[channel].speed;
}

void FanControllerProfile::setRamp(int channel, const FanSpeedRamp& other)
{
    m_ramp[channel] = other;
    m_ramp[channel].generateCurve();
}

QString FanControllerProfile::profileDescription(void)
{
    return profileDescription(m_name);
}

inline QString FanControllerProfile::boolToText(bool isTrue) const
{
    return isTrue ? "True" : "False";
}

inline QString FanControllerProfile::boolToTempScale(bool isCelcius) const
{
    return isCelcius ? "Celcius" : "Fahrenheit";
}


#endif // PHOEBETRIA_FAN_PROFILES_H
