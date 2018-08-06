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

#include "dbmanager.h"

#include <QtSql>
#include <QSqlQuery>
#include <QString>
#include <QSettings>
#include <QDebug>
#include <QFile>

#include "maindb.h"
#include "preferences.h"
#include "utils.h"

QString DatabaseManager::m_dbPath = Preferences::filepath();

const DatabaseManager::DbDetails DatabaseManager::m_dbConnectionDetails[]  =
{
    { "primaryDb", "phoebetria.sqlite" },
    { "logDb", "sessionlog.sqlite" }
};


DatabaseManager::DatabaseManager()
    : QSqlDatabase()
{
}

DatabaseManager::DatabaseManager(const QSqlDatabase& other)
    : QSqlDatabase(other)
{
}

void DatabaseManager::initAllDatabases(void)
{
    if (!driverIsAvailable())
        return;

    MainDb::init();
}

bool DatabaseManager::driverIsAvailable(void)
{
    QStringList drivers = QSqlDatabase::drivers();
    int c = drivers.count();
    int i;
    for (i = 0; i < c; ++i)
    {
        if (drivers.at(i) == "QSQLITE")
            break;
    }

    return i < c;
}

QString DatabaseManager::prependDbPath(const QString& filename)
{
    QString result(m_dbPath);
    result.append(QDir::separator());
    result.append(filename);
    return result;
}



QStringList DatabaseManager::tables(const QString &dbConnectionName)
{
    return QSqlDatabase::database(dbConnectionName).tables();
}


QStringList DatabaseManager::tableFields(const QString &dbConnectionName,
                                         const QString& tablename)
{
    QSqlRecord rec(QSqlDatabase::database(dbConnectionName).record(tablename));

    QStringList fields;

    for (int i = 0; i < rec.count(); ++i)
        fields.append(rec.fieldName(i));

    return fields;
}

DatabaseManager::DbDetailsList DatabaseManager::connections(void)
{
    DbDetailsList supportedConnections;

    int count = sizeof m_dbConnectionDetails / sizeof m_dbConnectionDetails[0];
    for (int i = 0; i < count; ++i)
    {
        supportedConnections.append(m_dbConnectionDetails[i]);
    }
    return supportedConnections;
}

