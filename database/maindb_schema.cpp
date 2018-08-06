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

#include "maindb_schema.h"

#include <QStringList>
#include <QSqlDatabase>

#include "dbmanager.h"

#include "utils.h"


static const char* newDbConnName = "pdbs_tmp_newPrimaryDb";
static const char* tmpDbConnName = "pdsb_tmp_pdb";


int MainDbSchema::m_schemaVersion = 1;

static MainDbSchema::TableDef schema[] =
{
    {
        "DatabaseInfo",
        "CREATE TABLE DatabaseInfo ("
        "     name    VARCHAR(32)     PRIMARY KEY"
        "    ,value   TEXT"
        ");"
    },

    {
        "Profile",
        "create table Profile("
        "     p_id           INTEGER        PRIMARY KEY"
        "    ,name           VARCHAR(32)    UNIQUE"
        "    ,label          VARCHAR(32)    UNIQUE"
        "    ,description    VARCHAR(255)"
        "    ,isAuto         BOOLEAN        DEFAULT 'true'"
        "    ,isCelcius      BOOLEAN        DEFAULT 'true'"
        "    ,isAudibleAlarm BOOLEAN        DEFAULT 'true'"
        "    ,isSoftwareAuto BOOLEAN        DEFAULT 'false'"
        ");"
    },

    {
        "ChannelSetting",
        "create table ChannelSetting ("
        "     p_id       INTEGER"
        "    ,channel    INTEGER"
        "    ,manualRpm  INTEGER"
        "    ,alarmTempF INTEGER"
        "    ,foreign key ( p_id ) references Profile ( p_id )"
        "         on delete cascade deferrable initially deferred"
        ");"
    },

    {
        /* Note: cp1_x, cp1_y, cp2_x, and cp2_y are not currently used.
                 They are control points for (up to cubic) Bezier curves
                 and although Bezier curve support has been dropped for the
                 current version and foreseeable future there is no harm in
                 leaving them in the schema
        */
        "SoftwareAutoSetting",
        "create table SoftwareAutoSetting ("
        "    p_id                   INTEGER"
        "   ,channel                INTEGER"
        "   ,probeAffinity          INTEGER     DEFAULT -1"
        "   ,allowFanToTurnOff      BOOLEAN"
        "   ,speed_minUsable        INTEGER"
        "   ,temperatureF_fanOn     INTEGER"
        "   ,temperatureF_rampStart INTEGER"
        "   ,temperatureF_rampMid   INTEGER"
        "   ,temperatureF_rampEnd   INTEGER"
        "   ,temperatureF_fanToMax  INTEGER"
        "   ,speed_fanOn            INTEGER"
        "   ,speed_rampStart        INTEGER"
        "   ,speed_rampMid          INTEGER"
        "   ,speed_rampEnd          INTEGER"
        "   ,cp1_x                  INTEGER"
        "   ,cp1_y                  INTEGER"
        "   ,cp2_x                  INTEGER"
        "   ,cp2_y                  INTEGER"
        "   ,speed_stepSize         INTEGER     DEFAULT 100"
        "   ,rampType               VARCHAR(16) DEFAULT 'linear'"
        "   ,tHysteresisUp          INTEGER     DEFAULT 1"
        "   ,tHysteresisDown        INTEGER     DEFAULT 1"
        "   ,tHysteresisFanOff      INTEGER     DEFAULT 2"
        "   ,adjustSpeedDelay       INTEGER     DEFAULT 15000 /* ms */"
        "   ,maxFanSpeed            INTEGER"
        "   ,primary key ( p_id, channel )"
        "   ,foreign key ( p_id ) references Profile ( p_id )"
        "       on delete cascade deferrable initially deferred"
        ");"
    }
};
#define SHUBETRIA_DB_SCHEMA_TABLE_COUNT ( sizeof schema / sizeof schema[0] )


static const char* defaultDataSql[] =
{
        // Note: schema version is inserted by insertDefaultData()
    "INSERT INTO [Profile] ([p_id], [name], [isAuto], [isCelcius], [isAudibleAlarm], [isSoftwareAuto]) VALUES (1, '__SHUBETRIA_DEFAULT', 'true', 'true', 'true', 'false')",
    "INSERT INTO ChannelSetting (p_id, channel, manualRpm, alarmTempF) VALUES (1, 0, 50000, 194)",
    "INSERT INTO ChannelSetting (p_id, channel, manualRpm, alarmTempF) VALUES (1, 1, 50000, 194)",
    "INSERT INTO ChannelSetting (p_id, channel, manualRpm, alarmTempF) VALUES (1, 2, 50000, 194)",
    "INSERT INTO ChannelSetting (p_id, channel, manualRpm, alarmTempF) VALUES (1, 3, 50000, 194)",
    "INSERT INTO ChannelSetting (p_id, channel, manualRpm, alarmTempF) VALUES (1, 4, 50000, 194)"
};
#define SHUBETRIA_DB_DEFAULT_DATA_COUNT \
    ( sizeof defaultDataSql / sizeof defaultDataSql[0] )


MainDbSchema::MainDbSchema()
{
}

/*! Verify the database

    Returns true if the correct tables are present and the schema is the
    correct version.

    \pre The database referred to by \e dbFilename exists and is a valid
         database.
*/
bool MainDbSchema::verify(const QString* dbFilename,
                             QStringList* missingTablesList)
{
    return checkTables(*dbFilename, missingTablesList) && schemaVersionOk();
}


/*! Creates a new database with the default tables and data.

    Creates a new database with the filename \e dbFilename.
    If \e oldDbFilename != NULL, then migrate the data from the old database
    to the newly created database.
*/
QSqlError MainDbSchema::create(const QString* newDbFilename,
                                  const QString* oldDbFilename)
{
    QSqlError err;

    QSqlDatabase newDb;
    newDb = QSqlDatabase::addDatabase("QSQLITE", newDbConnName);
    newDb.setDatabaseName(*newDbFilename);
    if (!newDb.open())
        return newDb.lastError();

    if ( (err = createTables()).isValid() ) goto abort;

    if ( (err = migrateData(newDbFilename, oldDbFilename)).isValid()) goto abort;

    err = insertDefaultData();

abort:
    newDb.close();

    return err;
}


/*! Basic schema check: check that the correct tables are present.

    This functions provides a \b basic check of the database. It only checks
    that the required tables are present; no other checks are made.

    If the required tables are present in the database, true is returned. If
    false is returned and \e missingTablesList != NULL then missingTablesList
    will be populated with the names of the tables that are missing.

    \note If \e missingTablesList is supplied (i.e. !NULL) it is cleared upon
         calling this function.

    \post If \e missingTablesList != NULL and the return value == true, then
         missingTablesList->size() == 0
*/
bool MainDbSchema::checkTables(const QString& dbFilename,
                                  QStringList* missingTablesList)
{
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE", tmpDbConnName);
    db.setDatabaseName(dbFilename);
    if (!db.open())
        return false;

    bool ok = true;

    QStringList db_tables = db.tables();

    if (missingTablesList) missingTablesList->clear();

    for (unsigned i = 0 ; i < SHUBETRIA_DB_SCHEMA_TABLE_COUNT; i++)
    {
        if (!db_tables.contains(schema[i].name))
        {
            ok = false;
            if (missingTablesList)
                missingTablesList->append(schema[i].name);
        }
    }

    db.close();

    return ok;
}

/*! Check if the database's schema version matches the internal version.

    \pre A connection named \e newDbConnectionName (static global in this file)
         has been established.
*/
bool MainDbSchema::schemaVersionOk(void)
{
    SHUBETRIA_STUB_FUNCTION
    return true; // TODO IMPLEMENT
}


/*! Create the schema.

    Creates the tables and inserts default data.
 */
QSqlError MainDbSchema::createSchema(void)
{
    QSqlError err;

    if ( (err = createTables()).isValid() ) return err;

    return insertDefaultData();
}


/*! Create the tables for the schema.

    Creates all the tables for the database. Note that this function does not
    delete (drop) a table if it's already present.

    \pre A connection named \e newDbConnectionName (static global in this file)
         has been established.
*/
QSqlError MainDbSchema::createTables(void)
{
    QSqlError err;

    QSqlDatabase db = QSqlDatabase::database(newDbConnName);
    QStringList db_tables = db.tables();

    for (unsigned i = 0 ; i < SHUBETRIA_DB_SCHEMA_TABLE_COUNT; i++)
    {
        // If the table doesn't exist, create it
        if (!db_tables.contains(schema[i].name))
        {
            QSqlQuery qry(db);

            if (!qry.exec(schema[i].ddl))
            {
                err = db.lastError();
                break;
            }
        }
    }

    return err;
}

/*! Migrate data from the old database to the new database.

    \pre    newDbFilename != NULL, oldDbFilename != NULL
*/
QSqlError MainDbSchema::migrateData(const QString* newDbFilename,
                                       const QString* oldDbFilename)
{
    /* This is the first version of the schema, so there is nothing to
     * do here
     */

    (void)newDbFilename;    // Unused
    (void)oldDbFilename;    // Unused

    return QSqlError(); // no error
}

/*!
    \pre A connection named \e newDbConnectionName (static global in this file)
         has been established.
*/
QSqlError MainDbSchema::insertDefaultData(void)
{
    QSqlError err;

    QSqlDatabase db = QSqlDatabase::database(newDbConnName);
    QSqlQuery qry(db);

    db.transaction();

    for (unsigned i = 0 ; i < SHUBETRIA_DB_DEFAULT_DATA_COUNT; i++)
    {
        if (!qry.exec(defaultDataSql[i]))
        {
            err = db.lastError();
            break;
        }
    }

    if (!err.isValid())
    {
        qry.prepare("INSERT INTO DatabaseInfo (name, value)"
                    " VALUES (:keyname, :version)");
        qry.bindValue(":keyname", "schemaVersion");
        qry.bindValue(":version", m_schemaVersion);
        if (!qry.exec())
            err = db.lastError();
    }


    if (!err.isValid())
        db.commit();
    else
    {
        db.rollback();
        qDebug() << "Error:" << db.lastError();
    }
    return err;
}
