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

#include "shubetriaapp.h"

#include <QMessageBox>

#include "dbmanager.h"
#include "themes.h"
#include "preferences.h"

FanControllerIO ShubetriaApp::m_fanControllerIO;
EventDispatcher ShubetriaApp::m_dispatcher;
QTimer ShubetriaApp::m_globalTimer;
Preferences ShubetriaApp::m_prefs;


ShutdownHelper::ShutdownHelper(QThread *parent)
    : QThread(parent)
{

}

void ShutdownHelper::wait(unsigned long ms)
{
    msleep(ms);
}


ShubetriaApp::ShubetriaApp(int &argc, char **argv)
    : QApplication(argc, argv),
      m_currentStyle_filename()
{
    setOrganizationName("Shubetria");
    setApplicationName("Shubetria");

    QSettings::setDefaultFormat(QSettings::IniFormat);

    DatabaseManager db;
    db.initAllDatabases();

    if (!setTheme(m_prefs.stylesheet()))
    {
        QMessageBox::critical(
                    NULL,
                    tr("Could not set style."),
                    tr("Could not set the application style."
                               " It may not exist. Please check preferences.\n\n"
                               "Style filename: %1\n\n"
                               "Setting to the standard profile instead."
                       ).arg(m_prefs.stylesheet())
                    );
        setTheme(Themes::getBuiltInStyleSheetName());
    }

    m_globalTimer.start(200);
    m_dispatcher.start(200);

    m_fanControllerIO.connect();            // Connect to the IO device
    m_fanControllerIO.connectSignals();

    m_fanControllerIO.fanControllerData().connectSignals();
}

void ShubetriaApp::commitData(QSessionManager& manager)
{
    shutdown();
    // (c) 2018 Shub
    // QApplication::commitData(manager);
    QGuiApplication::commitDataRequest(manager);
}


/* This function ensures that the FanControllerIO is shutdown correctly;
   i.e. that all request in the event queue have been processed. This is
   especially required when the app was in software auto mode and the
   Recon needs to be reset back to a pre-software-auto state.
 */

bool ShubetriaApp::shutdown(void)
{
    // Wait for all pending tasks to be processed
    m_dispatcher.shutdown();

    /* We don't know how many ms have elapsed since the request queue was
       last processed, so delay before the loop to make sure at least 200ms
       have elapsed since the previous processing of the request queue
      */
    ShutdownHelper::wait(200);

    /* Loop until the request queue is emptied
     */
    while (!m_fanControllerIO.shutdown())
    {
        ShutdownHelper::wait(200);
    }
    return true;
}

bool ShubetriaApp::setTheme(const QString& styleFilename)
{
    bool r = false;
    if (Themes::setAppStyleSheet(styleFilename))
    {
        m_currentStyle_filename = styleFilename;
        r = true;
    }
    return r;
}

const QString& ShubetriaApp::getCurrentThemeFilename(void) const
{
    return m_currentStyle_filename;
}
