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

#ifndef SHUBETRIA_DB_PRIMARY_SCHEMA_H
#define SHUBETRIA_DB_PRIMARY_SCHEMA_H

#include <stdlib.h>

class QStringList;      // Fwd decl
class QString;          // Fwd decl
class QSqlError;

class MainDbSchema
{
public:

    typedef struct TableDef
    {
        const char* name;
        const char* ddl;
    } TableDef;

    MainDbSchema();

    static bool verify(const QString* dbFilename,
                       QStringList *missingTablesList = NULL);

    static QSqlError create(const QString* newDbFilename,
                            const QString* oldDbFilename = NULL);

protected:

    static bool checkTables(const QString &dbFilename,
                            QStringList *missingTablesList);

    static bool schemaVersionOk(void);

    static QSqlError createSchema(void);

    static QSqlError createTables(void);

    static QSqlError migrateData(const QString* newDbFilename,
                                 const QString* oldDbFilename);

    static QSqlError insertDefaultData(void);

private:

    static int m_schemaVersion;
};

#endif // SHUBETRIA_DB_PRIMARY_SCHEMA_H
