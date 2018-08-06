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

#ifndef PHOEBETRIA_DATABASE_H
#define PHOEBETRIA_DATABASE_H

#include <QStringList>
#include <QList>
#include <QtSql>

class DatabaseManager : public QSqlDatabase
{

public:

    typedef enum
    {
        PrimaryDb,
        LoggingDb
    } DatabaseId;

    typedef struct
    {
        const char* connName;
        const char* filename;
    } DbDetails;

    typedef QList<DbDetails> DbDetailsList;

    DatabaseManager();
    DatabaseManager(const QSqlDatabase& other);

    static void initAllDatabases(void);

    inline static QString dbConnectionName(DatabaseId dbId);
    inline static QString primaryDbConnName(void);

    inline static QString dbFilenameWithPath(DatabaseId dbId);

    static bool driverIsAvailable(void);

    static QStringList tables(const QString& dbConnectionName);

    static QStringList tableFields(const QString &dbConnectionName,
                                   const QString& tablename);

    inline static QString pathToDatabases(void);

    static DbDetailsList connections(void);

protected:

    static QString prependDbPath(const QString& filename);

private:

    static QString m_dbPath;

    static const DbDetails m_dbConnectionDetails[];
};



QString DatabaseManager::dbConnectionName(DatabaseId dbId)
{
    return m_dbConnectionDetails[dbId].connName;
}

QString DatabaseManager::primaryDbConnName(void)
{
    return dbConnectionName(PrimaryDb);
}

QString DatabaseManager::dbFilenameWithPath(DatabaseId dbId)
{
    QString fnwp(m_dbConnectionDetails[dbId].filename);
    return prependDbPath(fnwp);
}

QString DatabaseManager::pathToDatabases(void)
{
    return m_dbPath;
}

#endif // PHOEBETRIA_DATABASE_H
