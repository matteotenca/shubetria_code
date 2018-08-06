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

#include "appinfo.h"

#include <QSysInfo>
#include <QDebug>
#include <QFile>
#include <QByteArray>

#include "builddetails.h"
#include "shubetriaapp.h"
#include "hidapi.h"
#include "maindb.h"
#include "shubetriaapp.h"

AppInfo::AppInfo()
{
}

QString AppInfo::basicInfoReport(void)
{
    QString report;
    QString trueStr("true");
    QString falseStr("FALSE");

    report = "<html><body>";
    report += "<table border=0>";
    report += "<tr><td width=120 align=left><h3>Shubetria:</h3></td><td width=500 align=left><h3>" + shubetriaVersion() + "</h3></td></tr>";
    report += "<tr><td width=120 align=left>Using Qt:</td><td width=200 align=left>" + qtVersion() + "</td></tr>";
    report += "<tr><td width=120 align=left>OS:</td><td width=200 align=left>" + platformInfo() + "</td></tr>";
    report += "<tr><td width=120 align=left>Recon:</td><td width=200 align=left>" + connectedToDevice() + "</td></tr>";
    report += "</table>";
    report += "<p></p>";
    report += "<table border=0>";
    report += "<tr><td width=120 align=left><h3>Database</h3></td></tr>";
    report += "<tr><td width=120 align=left>Main DB Exists:</td><td width=200 align=left>" + (mainDatabaseExists() ? trueStr : falseStr) + "</td></tr>";
    report += "<tr><td width=120 align=left>Driver ok:</td><td width=200 align=left>" + (databaseDriverLoaded() ? trueStr : falseStr) + "</td></tr>";
    report += "<tr><td width=120 align=left>Connected:</td><td width=200 align=left>" + (mainDatabaseIsConnected() ? trueStr : falseStr) + "</td></tr>";
    report += "<tr><td width=120 align=left>DB Version:</td><td width=200 align=left>" + mainDatabaseVersion() + "</td></tr>";
    report += "</table>";
    report += "<hr>";
    report += "<table border=0>";
    report += "<tr><td width=120 align=left><h3>Miscellaneous</h3></td></tr>";
    report += "</table><table border=0>";
    report += "<tr><td width=300 align=left>Max. elapsed time between device polling: "
            + QString::number(maxFanControllerPollTime()) + " ms</td></tr>";
    report += "</table>";
    report += "<p></p>";
    report += "<table border=0>";
    report += "<tr><th width=120 align=left>Item</th><th width=60 align=left>Ch0</th><th width=60 align=left>Ch1</th>";
    report += "<th width=60 align=left>Ch2</th><th width=60 align=left>Ch3</th><th width=60 align=left>Ch4</th></tr>";
    report += "<tr><td width=120 align=left>Last probe temps:</td>" + channelTemps(false);
    report += "</tr><tr><td width=120 align=left>Avg. probe temps:</td>" + channelTemps(true);
    report += "</tr></table>";
    report += "<hr>";
    report += "<table border=0>";
    report += "<tr><td width=120 align=left><h3>HID Devices</h3></td></tr>";

    QStringList dl = AppInfo::hidDevices();
    for (int i = 0; i < dl.count(); i++)
    {
        report += dl.at(i);
    }
    report += "</table>";

    return report;
}

QString AppInfo::qtVersion(void)
{
    return BuildDetails::qtVersion();
}

QString AppInfo::shubetriaVersion(void)
{
    return BuildDetails::versionStr().trimmed()
            + " " + BuildDetails::buildDateTimeStr();
}

QString AppInfo::platformInfo(void)
{
    return osVersionAsString();
}

QString AppInfo::connectedToDevice(void)
{
    bool isConn = ph_fanControllerIO().isConnected();

    return isConn ? "Device Connected" : "Device NOT Connected";
}

unsigned long AppInfo::maxFanControllerPollTime(void)
{
    return ph_fanControllerIO().maxPollDelta();
}

QString AppInfo::channelTemp(int channel, bool getAverage)
{
    const FanChannelData& cd = ph_fanControllerData().fanChannelSettings(channel);
    int T;

    if (getAverage)
        T = cd.tempAveraged();
    else
        T = cd.lastTemp();

    if (ph_fanControllerData().isCelcius())
        T = ph_fanControllerData().toCelcius(T);

    QString s = QString("<td width=100 align=left>%1</td>").arg(T);
    return s;
}

QString AppInfo::channelTemps(bool getAverage)
{
    QString s;

    for (int i = 0; i < FC_MAX_CHANNELS; ++i)
    {
        if (i != 0)
            s += " ";
        s += channelTemp(i, getAverage);
    }

//    FIXME: For some reason these are placed on a new line
    if (ph_fanControllerData().isCelcius())
        s += "<td width=30 align=left>&deg;C</td>";
    else
        s += "<td width=30 align=left>&deg;F</td>";

    return s;
}

QStringList AppInfo::hidDevices(void)
{
    struct hid_device_info *devs, *curr_dev;

    QStringList result;
    QString deviceStr;

    for (devs = curr_dev = hid_enumerate(0, 0)
         ; curr_dev
         ; curr_dev = curr_dev->next)
    {
        deviceStr  = "<tr><td width=120 align=left>Manufacturer:</td><td width=400 align=left>";
        deviceStr += QString::fromWCharArray(curr_dev->manufacturer_string)+"</td></tr></tr>";

        deviceStr += "<tr><td width=120 align=left>Product:</td><td width=400 align=left>";
        deviceStr += QString::fromWCharArray(curr_dev->product_string)+"</td></tr>";

        deviceStr += "<tr><td width=120 align=left>Vendor Id:</td><td width=400 align=left>";
        deviceStr += QString("%1").arg(curr_dev->vendor_id, 4, 16, QChar('0'))+"</td></tr>";

        deviceStr += "<tr><td width=120 align=left>Product Id:</td><td width=400 align=left>";
        deviceStr += QString("%1").arg(curr_dev->product_id, 4, 16, QChar('0'))+"</td></tr>";

        deviceStr += "<tr><td width=120 align=left>Serial Number:</td><td width=400 align=left>";
        deviceStr += QString::fromWCharArray(curr_dev->serial_number)+"</td></tr>";

        deviceStr += "<tr><td width=120 align=left>Release:</td><td width=400 align=left>";
        deviceStr += QString::number(curr_dev->release_number)+"</td></tr>";
        deviceStr += "<p></p>";

        result.append(deviceStr);
    }
    hid_free_enumeration(devs);
    return result;
}

bool AppInfo::databaseDriverLoaded(void)
{
    return DatabaseManager::driverIsAvailable();
}

bool AppInfo::mainDatabaseExists(void)
{
    return MainDb::verifyDbAndPathExist();
}

bool AppInfo::mainDatabaseIsConnected(void)
{
    return MainDb::isValid();
}

QString AppInfo::mainDatabaseVersion(void)
{
    MainDb mdb;

    return mdb.schemaVersion();
}

QString AppInfo::mainDatabasePath(void)
{
    return DatabaseManager::dbFilenameWithPath(DatabaseManager::PrimaryDb);
}

/*********************************************************************
 Private member functions
 *********************************************************************/

QString AppInfo::osVersionAsString(void)
{
     QString os;

#if defined Q_OS_WIN || defined Q_OS_WIN32

    enum QSysInfo::WinVersion v;
    v = QSysInfo::windowsVersion();
    switch (v)
    {
    case QSysInfo::WV_NT:
        os = "Windows NT";
        break;
    case QSysInfo::WV_2000:
        os = "Windows 2000";
        break;
    case QSysInfo::WV_XP:
        os = "Windows XP";
        break;
    case QSysInfo::WV_2003:
        os = "Windows 2003";
        break;
    case QSysInfo::WV_VISTA:
        os = "Windows Vista";
        break;
    case QSysInfo::WV_WINDOWS7:
        os = "Windows 7";
        break;
    case QSysInfo::WV_WINDOWS8:
        os = "Windows 8";
        break;
    default:
        os = "Windows (Unknown Version)";
        break;
    }

#elif defined Q_OS_MAC
     enum QSysInfo::MacVersion v;
     v = QSysInfo::MacintoshVersion;
     switch (v)
     {
     case QSysInfo::MV_10_6:
         os = "OS X 10.6 (Snow Leopard)";
         break;
     case QSysInfo::MV_10_7:
         os = "OS X 10.7 (Lion)";
         break;
     case QSysInfo::MV_10_8:
         os = "OS X 10.8 (Mountain Lion)";
         break;
     default:
         os = "OS X (Unknown Version)";
         break;
     }
#elif defined Q_OS_LINUX
    QFile f;

    f.setFileName("/proc/version");
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray ba = f.readAll();
        os = ba.simplified();
        f.close();
    }

    if (os.isEmpty())
    {
        os = "Linux";
    }

#else
    os = "Unknown";
#endif

    return os;
}

