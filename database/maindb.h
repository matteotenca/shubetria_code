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

#ifndef SHUBETRIA_PRIMARYDB_H
#define SHUBETRIA_PRIMARYDB_H

#include <QStringList>
#include <QList>
#include <QtSql>
#include "dbmanager.h"
#include "bfx-recon/bfxrecon.h"

/* Fwd Decls
 */
class FanControllerProfile;
class FanSpeedRamp;

class MainDb
{
public:
    MainDb();

    inline static QString dbConnectionName(void);
    inline QSqlError lastSqlError(void) const;

    static void init(void);

    QStringList profileNames();
    NameAndControlModeList profileNamesAndModes(void);

    QString profileDescription(const QString& profileName);

    bool writeProfile(const QString &name, const FanControllerProfile &profile);

    bool readProfile(const QString&name, FanControllerProfile& profile);

    bool deleteProfile(const QString& profileName);

    int getProfileId(const QString& name);

    QString schemaVersion(void);

    static bool isValid(void);

    static bool verifyDbAndPathExist(void);

protected:

    bool readChannelSpeedRamps(const QString&name,
                               FanControllerProfile& profile);

    int writeProfileCommonSettings(const QString& profileName,
                                   const QString &profileDescription,
                                   bool isAuto,
                                   bool isCelcius,
                                   bool isAudibleAlarm, bool isSoftwareAuto);

    bool writeProfileChannelSettings(int profileId,
                                     int channel,
                                     int rpm,
                                     int alarmTempInF);

    bool writeChannelSpeedRamps(int profileId,
                                 const FanControllerProfile &profile);

    bool writeChannelSpeedRamp(int profileId,
                               int channel,
                               const FanSpeedRamp& ramp);


    bool deleteChannelSpeedRamps(int profileId);

    static QSqlError connect(const QString &connectionName);

    static QSqlError createNewDb(const QString &connectionName);
    static QSqlError checkExistingDb(const QString &connectionName);
    static QSqlError recreateDb(const QString &connectionName);
    static QSqlError enableFkSupport(const QString &connectionName);

    static int importOldStyleProfiles(void);

private:

    QSqlError m_lastSqlError;

    QString m_profileDesc();
};


QString MainDb::dbConnectionName(void)
{
    return DatabaseManager::primaryDbConnName();
}

QSqlError MainDb::lastSqlError(void) const
{
    return m_lastSqlError;
}

#endif // SHUBETRIA_PRIMARYDB_H
