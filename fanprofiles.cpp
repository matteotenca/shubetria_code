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


#include "fanprofiles.h"

#include <QDebug>
#include <QFileInfo>
#include <QDir>

#include "fancontrollerdata.h"
#include "maindb.h"

#include "shubetriaapp.h"

FanControllerProfile::FanControllerProfile()
{
    initCommon();
}

FanControllerProfile::FanControllerProfile(const QString& name,
                                           const QString& desc)
{
    initCommon();
    m_name = name;
    m_description = desc;
}


void FanControllerProfile::initCommon(void)
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       "Shubetria",
                       "Shubetria");

    m_defaultProfileLocation = QFileInfo(settings.fileName()).path()
                               + "/Presets/";

    m_isCelcius = false;
    m_isAuto = true;
    m_isAudibleAlarm = true;
    m_isSoftwareAuto = false;
}


QString FanControllerProfile::defualtProfileLocation(void) const
{
    return m_defaultProfileLocation;
}

QStringList FanControllerProfile::getProfileNames(void)
{
    MainDb mdb;
    QStringList result = mdb.profileNames();
    if (mdb.lastSqlError().type() != QSqlError::NoError)
    {
        qDebug() << "Error reading profile names."
                 << __FILE__ << ":" << __LINE__;

        // TODO: Implement mechanism for passing error back to caller
    }
    return result;
}

NameAndControlModeList FanControllerProfile::getProfileNamesAndModes(void)
{
    MainDb mdb;
    NameAndControlModeList result = mdb.profileNamesAndModes();

    if (mdb.lastSqlError().type() != QSqlError::NoError)
    {
        qDebug() << "Error reading profile names."
                 << __FILE__ << ":" << __LINE__;

        // TODO: Implement mechanism for passing error back to caller
    }
    return result;
}

/* Set from the current controller settings
 */
void FanControllerProfile::setFromCurrentData(const FanControllerData& data)
{
    m_isCelcius = data.isCelcius();
    m_isAuto = data.isAuto();
    m_isAudibleAlarm = data.isAudibleAlarm();
    m_isSoftwareAuto = data.isSoftwareAuto();

    for (int i = 0; i < FC_MAX_CHANNELS; i++)
    {
        m_channelSettings[i].alarmTemp = data.alarmTemp(i);

        int speedToSave;

        speedToSave = data.isManualRpmSet(i) ? data.manualRPM(i)
                                             : data.lastRPM(i);

        m_channelSettings[i].speed = speedToSave;

        m_ramp[i] = data.ramp(i);
    }
}


bool FanControllerProfile::save(const QString& profileName)
{
    MainDb mdb;

    if (!mdb.isValid())
        return false;

    return mdb.writeProfile(profileName, *this);
}


bool FanControllerProfile::load(const QString& profileName)
{
    MainDb mdb;

    if (!mdb.isValid())
        return false;

    if (ph_fanControllerData().ramp_reqParamsForInitAreSet())
        ph_fanControllerData().storeCurrentState();

    return mdb.readProfile(profileName, *this);
}


bool FanControllerProfile::erase(const QString& profileName)
{
    MainDb mdb;

    if (!mdb.isValid())
        return false;

    return mdb.deleteProfile(profileName);
}

QString FanControllerProfile::profileDescription(const QString& profileName)
{
    MainDb mdb;

    if (!mdb.isValid())
        return QString();

    return mdb.profileDescription(profileName);
}


// returns the number of files imported
// pre dir.exists() == true
int FanControllerProfile::importFromIni(QDir& dir)
{
    QFileInfoList files = dir.entryInfoList(QDir::Files);

    MainDb mdb;
    int count = files.count();
    int filesImported = 0;

    for (int i = 0; i < count; ++i)
    {
        FanControllerProfile tmp;
        if (loadFromIni(files.at(i).filePath(), tmp))
        {
            QString profileName(files.at(i).fileName());
            if (profileName.endsWith(".ini"))
                profileName.chop(4);

            if (profileName.isEmpty())  // Paranoia probabaly
                continue;

            if (mdb.writeProfile(profileName, tmp))
                filesImported++;
        }
    }

    return filesImported;
}


bool FanControllerProfile::loadFromIni(const QString& filenameAndPath,
                                       FanControllerProfile& dest)
{
    bool r = true;

    QFile file;
    file.setFileName(filenameAndPath);
    if (!file.exists())
    {
        return false;
    }

    QSettings settings (filenameAndPath, QSettings::IniFormat);

    dest.m_isAuto = settings.value("common/auto", true).toBool();
    dest.m_isCelcius = settings.value("common/celcius", true).toBool();
    dest.m_isAudibleAlarm = settings.value("common/alarm", true).toBool();

    for (int i = 0; i < FC_MAX_CHANNELS; i++)
    {
        QString group = "fan_" + QString::number(i);
        settings.beginGroup(group);

        int value;
        bool ok;

        value = settings.value("alarm_temperature_F",
                               dest.m_channelSettings[i].alarmTemp).toInt(&ok);
        if (ok)
            dest.m_channelSettings[i].alarmTemp = value;
        else
            r = false;

        value = settings.value("rpm",
                               dest.m_channelSettings[i].speed).toInt(&ok);
        if (ok)
            dest.m_channelSettings[i].speed = value;
        else
            r = false;

        settings.endGroup();
    }

    return r && (settings.status() == QSettings::NoError);
}

QString FanControllerProfile::htmlReport(void)
{
    QString report;

    report = "<body><html>";
    report += htmlReportCommon();

    if (isSoftwareAuto())
        report += htmlReportSwAuto();
    else
        report += htmlReportManual();

    report += "</html></body>";

    return report;
}

QString FanControllerProfile::htmlReportCommon(void) const
{
    QString report;

    report += "<table border=0>";
    report += "<tr><td width=120 align=left><h3>Profile Name:</h3></td><td width=100 align=left><h3>" + profileName() + "</h3></td></tr>";
    report += "</table>";
    report += "<p></p>";
    report += "<table border=0>";
    report += "<tr><td width=120 align=left>Temperature Scale:</td><td width=100 align=left>" + boolToTempScale(isCelcius()) + "</td></tr>";
    report += "<tr><td width=120 align=left>Audible Alarm:</td><td width=100 align=left>" + boolToText(isAudibleAlarm()) + "</td></tr>";
    report += "<tr><td width=120 align=left>Recon Auto:</td><td width=100 align=left>" + boolToText(isAuto()) + "</td></tr>";
    report += "<tr><td width=120 align=left>Software Auto:</td><td width=100 align=left>" + boolToText(isSoftwareAuto()) + "</td></tr>";
    report += "</table>";
    report += "<p></p>";

    return report;
}

QString FanControllerProfile::htmlReportManual(void) const
{
    QString report;

    report += "<table border=0>";
    report += "<tr><th width=120 align=left>Channel</p></th><th width=100 align=left>RPM</th><th align=left width=100>Alert Temp</th></tr>";

    for (int channel = 0; channel < FC_MAX_CHANNELS; ++channel)
    {
        QString channelRPM = QString::number(speed(channel));
        QString channelAlarm = FanControllerData::temperatureString(alarmTemp(channel), true, isCelcius());

        report += "<tr><td width=120 align=left>" + ph_prefs().channelName(channel) + "</td>";
        report += "<td width=100 align=left>" + channelRPM + "</td>";
        report += "<td width=100 align=left>" + channelAlarm + "</td></tr>";
    }
    report += "</table>";

    return report;
}

QString FanControllerProfile::htmlReportSwAuto(void) const
{
    QString report;

    report += "<table border=0><tr>";
    report += "<th width=120 align=left>Channel</p></th>";
    report += "<th width=100 align=left>Alert Temp</th>";
    report += "<th width=100 align=left>Turn Off</th>";
    report += "<th width=100 align=left>Temp Fan On</p></th>";
    report += "<th width=100 align=left>Temp/Speed Start</th>";
    report += "<th width=100 align=left>Temp/Speed Mid</th>";
    report += "<th width=100 align=left>Temp/Speed End</p></th>";
    report += "<th width=100 align=left>Temp Fan Max</th>";
    report += "<th width=100 align=left>Min RPM</th>";
    report += "<th width=100 align=left>Max RPM</p></th>";
    report += "<th width=100 align=left>Probe Affinity</th>";
    report += "<th width=100 align=left>Hysteresis Up</th>";
    report += "<th width=100 align=left>Hysteresis Down</p></th>";
    report += "<th width=100 align=left>Hysteresis Off</th>";
    report += "</tr>";

    for (int channel = 0; channel < FC_MAX_CHANNELS; ++channel)
    {
        const FanSpeedRamp& framp = ramp(channel);

        QString channelAlarm = FanControllerData::temperatureString(alarmTemp(channel), true, isCelcius());
        QString temperatureF_fanOn = FanControllerData::temperatureString(framp.temperatureF_fanOn(), true, isCelcius());
        QString tempRampStart = FanControllerData::temperatureString(framp.temperatureF_rampStart(), true, isCelcius());
        QString speedRampStart = QString::number(framp.speed_rampStart());
        QString tempRampMid = FanControllerData::temperatureString(framp.temperatureF_rampMid(), true, isCelcius());
        QString speedRampMid = QString::number(framp.speed_rampMid());
        QString tempRampEnd = FanControllerData::temperatureString(framp.temperatureF_rampEnd(), true, isCelcius());
        QString speedRampEnd = QString::number(framp.speed_rampEnd());

        report += "<tr><td width=120 align=left>" + ph_prefs().channelName(channel) + "</td>";
        report += "<td width=100 align=left>" + channelAlarm + "</td>";
        report += "<td width=100 align=left>" + boolToText(framp.allowFanToTurnOff()) + "</td>";
        report += "<td width=100 align=left>" + temperatureF_fanOn + "</td>";
        report += "<td width=100 align=left>" + tempRampStart + " / " + speedRampStart+ "</td>";
        report += "<td width=100 align=left>" + tempRampMid + " / " + speedRampMid+ "</td>";
        report += "<td width=100 align=left>" + tempRampEnd + " / " + speedRampEnd+ "</td>";
        report += "<td width=100 align=left>" + FanControllerData::temperatureString(framp.temperatureF_fanToMax(), true, isCelcius()) +"</td>";
        report += "<td width=100 align=left>" + QString::number(framp.minUsableRpm()) +"</td>";
        report += "<td width=100 align=left>" + QString::number(framp.maxUsableRpm()) +"</td>";
        report += "<td width=100 align=left>" + QString::number(framp.probeAffinity()) +"</td>";
        report += "<td width=100 align=left>" + QString::number(framp.hysteresisUp()) +"</td>";
        report += "<td width=100 align=left>" + QString::number(framp.hysteresisDown()) +"</td>";
        report += "<td width=100 align=left>" + QString::number(framp.hysteresisFanOff()) +"</td>";
        report += "</tr>";
    }

    report += "</table>";

    return report;

}
