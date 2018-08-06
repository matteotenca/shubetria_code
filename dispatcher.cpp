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

#include "dispatcher.h"

#include <math.h>

#include "shubetriaapp.h"

/*************************************************************************/
/*!
    \class  EventDispatcher
    \brief  Responsible for scheduling all 'timer related' tasks that
            require undertaking. E.g. polling the fan controller device.

    This class issues signals on a regular basis based on \e Tasks that
    (should) require processing. The class itself does nothing but issue
    signals that other classes monitor and act upon. The minimum interval
    that signals can be sent is dependendant on minInterval(). Additionally,
    minInterval() is dependant on ShubetriaApp::m_globalTimer (the
    minimum interval for the dispatcher is set to m_globalTimer's interval
    because the dispatcher is attached to the global timer's timeout()
    signal).

    Internally, signals are issued based on timer \e ticks. Each task has
    an \e interval (in ticks) to indicate when the signal should be issued;
    e.g. if the task interval is 5 then a signal will be issued every 5
    ticks.

    \e intervals have milliseconds as their unit
    \e ticks are an integer representation of milliseconds;
       1 tick == minInterval() milleseconds

    \sa    EventItem, intervalToTick(), tickToInterval()
 */

/*! \class EventDispatcher::Task
 *  \brief Support class for EventDispatcher. Tasks define signals for the
 *         dispatcher to dispatch based on the task's interval.
 */



EventDispatcher::Task::Task()
    : m_taskId(NotSet),
      m_interval(-1)
{
}

/*! Initialise a task that will issue a signal every interval \e ticks

    \param taskId    The type of signal to emit
    \param interval  Dispatch interval in \e ticks; \sa intervalToTick()

    \sa TaskId, intervalToTick(), tickToInterval()
*/
EventDispatcher::Task::Task(EventDispatcher::TaskId taskId, int interval)
{
    m_taskId = taskId;
    m_interval = interval;
}

/*! Initialise the object. This does \e not start the "timer".
    \sa start();
 */
EventDispatcher::EventDispatcher(QObject *parent) :
    QObject(parent)
{
    m_minInterval = 0;
    m_elapsedTicks = 0;
    m_isStarted = false;
    m_isShuttingDown = false;
    m_timeLastCalled = QDateTime::currentDateTime();
}

/*! Starts the dispatcher.

    Starts the dispatcher to begin monitoring scheduled tasks at
    \e interval millesecond intervals. The paramater \e interval must
    be at least the interval of the global timer; if \e interval is
    less than that of the global timer then the global timer's interval
    is used. The internal record of elapsed ticks (intervals) is reset to
    0.

    This function can be called even if a previous call has been made. The
    primary reason this is allowed is so that the interval can be changed.
    If called again, tasks intervals are recalcuated based on the new interval
    (internally the task intervals are represented as 'ticks' where one tick
    equals one interval of the timer).

    Returns the actual interval set, which may be different to requested if the
    requested interval is less than the global timer's interval.
*/
int EventDispatcher::start(unsigned interval)
{
    unsigned gi = ph_shubetriaApp()->globalTimerInterval();

    m_minInterval = interval < gi ? gi : interval;

    // Called here because task intervals are based on m_minInterval
    initTasks();

    if (!m_isStarted)   // Don't need to reconnect signals if done previously
        connectToTimerSignal();

    m_isStarted = true;
    m_elapsedTicks = 0;

    return m_minInterval;
}

bool EventDispatcher::shutdown(void)
{
    if (!m_isShuttingDown)
        m_isShuttingDown = true;

    return m_tasks.isEmpty();
}

/*! Populates the dispatcher schedule.

    Populates the list of (default) tasks that the EventDispatcher is
    responsible for issuing. Returns the number of tasks registered with the
    dispatcher.
 */
int EventDispatcher::initTasks(void)
{

    /*
     * Note: initEvents() may be called at a point after initial constructor
     * (e.g. if setBaseInterval() is called) therefore make sure the
     * list is empty
     */
    if (!m_tasks.isEmpty())
    {
        m_tasks.clear();
    }

    addTask(Task(ReqAllDeviceRelated, intervalToTick(30000)));

    addTask(Task(ResetFanRampTemps, intervalToTick(30000)));

    addTask(Task(ReqDeviceFlags, intervalToTick(5000)));

    addTask(Task(ReqTempAndCurrRpmAndMaxRpm, intervalToTick(5000)));
    addTask(Task(ReqAlarmTempAndManualRpm, intervalToTick(5000)));

    addTask(Task(LogData, intervalToTick(30000)));

    return m_tasks.size();
}


/*! Add a task to the schedule.
 */
void EventDispatcher::addTask(const Task& e)
{
    m_tasks.append(e);

}

/*! Connect to the timer signal.
 */
void EventDispatcher::connectToTimerSignal(void)
{
    connect(&ph_shubetriaApp()->m_globalTimer, SIGNAL(timeout()),
            this, SLOT(onTimer()));
}


/*! Convert an interval in milliseconds to timer 'ticks'.

    Returns \e interval converted to dispatch timer ticks.

    \param   interval  The interval in milliseconds
    \returns The interval converted to timer ticks

    \note Ticks are a multiple of EventDispatcher::baseInterval(). If the
    supplied interval is not a multiple of the base interval then it is
    rounded \e up. Related to this is that if \e interval is less than
    minInterval() the returned \e tick will be 1.

    \sa EventDispatcher::tickToInterval()
 */
unsigned EventDispatcher::intervalToTick(unsigned interval) const
{
    return ceil((double)interval / m_minInterval);
}


/*! Returns a \e tick converted into milliseconds.

    Returns \e tick in milliseconds.
*/
unsigned EventDispatcher::tickToInterval(unsigned tick) const
{
    return tick * m_minInterval;
}

/*! The main dispatch function

    Issues a regular tick event (passes on
    the application timer's tick) and issues event signals for those
    due to be issued.
*/
void EventDispatcher::onTimer(void)
{
    if (!m_isShuttingDown)
    {
        emit tick();        // Alternative signal to task(Tick)
        emit task(Tick);
        emit task(CheckForDeviceData);

        qint64 msSinceLastCalled;

        msSinceLastCalled = QDateTime::currentDateTime().toMSecsSinceEpoch()
                            - m_timeLastCalled.toMSecsSinceEpoch();

        if (msSinceLastCalled > 20000)
        {
            if (msSinceLastCalled > 40000)
            {
                emit refresh_critical();
            }
            else
            {
                emit refresh_low();
            }
            m_elapsedTicks = 0; // force the loop below to emit all tasks
        }

        for (int i = 0; i < m_tasks.size(); ++i)
        {
            const Task& item = m_tasks.at(i);
            if (m_elapsedTicks % item.interval() == 0)
            {
                emit task(item.taskId());
            }
        }

        m_timeLastCalled = QDateTime::currentDateTime();
    }

    ++m_elapsedTicks;
}
