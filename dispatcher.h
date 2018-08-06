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

#ifndef SHUBETRIA_DISPATCHER_H
#define SHUBETRIA_DISPATCHER_H

#include <QObject>
#include <QList>
#include <QTimer>
#include <QDateTime>

class ShubetriaApp;    // fwd decl

class EventDispatcher : public QObject
{
    Q_OBJECT

    friend class ShubetriaApp;

public:
    typedef enum TaskId
    {
        //!< Task Id isn't set
        NotSet,

        //!< Emitted every time the timer is triggered
        Tick,

        //!< Check the device for data
        CheckForDeviceData,

        //!< All channel related reqs
        ReqAllDeviceRelated,

        //!< Device flags
        ReqDeviceFlags,

        //! Alarm temp and manual rpm (all channels)
        ReqAlarmTempAndManualRpm,

        //! Current temp and rpm (all channels)
        ReqTempAndCurrRpmAndMaxRpm,

        //!< Log data
        LogData,

        //!< Reset fan ramp temps (all channels) to "not set"
        ResetFanRampTemps



    } TaskId; //!< Signal "types" that can be issued.

    class Task
    {
    public:
        Task();
        Task(EventDispatcher::TaskId taskId, int interval);

        TaskId taskId(void) const { return m_taskId; }
        int interval(void) const { return m_interval; }

    private:
        TaskId m_taskId;
        int m_interval;    // the task dispatch interval, for this task
    };


    explicit EventDispatcher(QObject *parent = 0);

    int start(unsigned interval);
    bool shutdown(void);

    bool isShuttingDown(void) const
        { return m_isShuttingDown; }

    //! The minumum interval (in milliseconds) that can be set
    unsigned minInterval(void) const
        { return m_minInterval; }

    unsigned intervalToTick(unsigned interval) const;
    unsigned tickToInterval(unsigned tick) const;

    inline void resetElapsedTime(void);

protected:

    int initTasks(void);
    void addTask(const Task& e);
    void connectToTimerSignal(void);

private:

    QList<Task> m_tasks;        //! Scheduled tasks

    unsigned m_minInterval;
    unsigned m_elapsedTicks;

    bool m_isStarted;
    bool m_isShuttingDown;

    QDateTime m_timeLastCalled;

signals:

    void task(EventDispatcher::TaskId task);
    void tick(void);

    void refresh_critical(void);
    void refresh_low(void);

public slots:

    void onTimer(void);
};

void EventDispatcher::resetElapsedTime(void)
{
    m_elapsedTicks = 0;
}

#endif // SHUBETRIA_DISPATCHER_H
