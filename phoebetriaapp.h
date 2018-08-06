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

#ifndef PHOEBETRIA_APP_H
#define PHOEBETRIA_APP_H

// (c) 2018 Shub
#include "fancontrollerio.h"

#include <QApplication>
#include <QTimer>
#include <QThread>

#include "dispatcher.h"
#include "preferences.h"

/*
    By the time PhoebetriaApp::shutdown() is called, the global timer has
    been disabled by Qt. Since the request thread still needs to be emptied,
    and requests can must be sent with a minumum interval of 200ms between
    them we need a mechanism for delay. This class implements a wait function
    that is used to add this delay between calls to m_fanControllerIO.shutdown()
*/

class ShutdownHelper : QThread
{

public:
    explicit ShutdownHelper(QThread *parent = 0);

    static void wait(unsigned long ms);
};


class PhoebetriaApp : public QApplication
{
    friend void EventDispatcher::connectToTimerSignal(void);

public:
    PhoebetriaApp(int &argc, char **argv);

    void commitData(QSessionManager& manager);

    FanControllerIO& fanControllerIO(void)
        { return m_fanControllerIO; }

    FanControllerData& fcd(void)
        { return m_fanControllerIO.fanControllerData(); }

    unsigned int globalTimerInterval(void)
        { return m_globalTimer.interval(); }

    EventDispatcher& dispatcher(void)
        { return m_dispatcher; }

    Preferences& prefs(void)
        { return m_prefs; }

    bool isShuttingDown(void)
        { return m_dispatcher.isShuttingDown(); }

    bool shutdown(void);

    void resetScheduler(void)
        { m_dispatcher.resetElapsedTime(); }

    bool setTheme(const QString& styleFilename);

    const QString& getCurrentThemeFilename(void) const;

private:
    static FanControllerIO m_fanControllerIO;
    static EventDispatcher m_dispatcher;
    static QTimer m_globalTimer;
    static Preferences m_prefs;

    QString m_currentStyle_filename;
};


/* Convenience functions
 */

inline PhoebetriaApp* ph_phoebetriaApp(void)
{
    return static_cast<PhoebetriaApp*> qApp;
}

inline FanControllerData& ph_fanControllerData(void)
{
    return ph_phoebetriaApp()->fcd();
}

inline FanControllerIO& ph_fanControllerIO(void)
{
    return ph_phoebetriaApp()->fanControllerIO();
}

inline EventDispatcher& ph_dispatcher(void)
{
    return ph_phoebetriaApp()->dispatcher();
}

inline bool ph_isShuttingDown(void)
{
    return ph_phoebetriaApp()->isShuttingDown();
}

inline void ph_shutdown(void)
{
    ph_phoebetriaApp()->shutdown();
}

inline void ph_resetSchedulerElapsedTime(void)
{
    ph_phoebetriaApp()->resetScheduler();
}

inline Preferences& ph_prefs(void)
{
    return ph_phoebetriaApp()->prefs();
}

#endif // PHOEBETRIA_APP_H
