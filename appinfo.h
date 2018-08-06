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

#ifndef SHUBETRIA_APPINFO_H
#define SHUBETRIA_APPINFO_H

#include <QString>
#include <QStringList>
#include <QWidget>

class AppInfo
{
public:
    AppInfo();

    static QString basicInfoReport(void);

    static QString qtVersion(void);
    static QString shubetriaVersion(void);
    static QString platformInfo(void);

    static unsigned long maxFanControllerPollTime(void);

    static QString channelTemp(int channel, bool getAverage);
    static QString channelTemps(bool getAverage);

    static QString connectedToDevice(void);
    static QStringList hidDevices(void);

    static bool databaseDriverLoaded(void);
    static bool mainDatabaseExists(void);
    static bool mainDatabaseIsConnected(void);
    static QString mainDatabaseVersion(void);
    static QString mainDatabasePath(void);

//    static installLocation(void);

private:
    static QString osVersionAsString(void);

};

#endif // SHUBETRIA_APPINFO_H
