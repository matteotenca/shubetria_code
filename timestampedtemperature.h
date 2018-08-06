#ifndef TIMESTAMPEDTEMPERATURE_H
#define TIMESTAMPEDTEMPERATURE_H

#include <QDateTime>

class TimestampedTemperature
{
public:

    TimestampedTemperature();

    bool isSet(void) const
        { return m_temperature != tempNotSetValue; }

    void clear(void)
        { m_temperature = tempNotSetValue; }

    int temperature(void) const
        { return m_temperature; }

    QDateTime timeUpdated(void) const
        { return m_timeUpdated; }

    inline qint64 elapsedSinceSet(void) const;
    inline qint64 elapsedSinceChecked(void);

    void setTemperature(int tF)
    {
        m_temperature = tF;
        m_timeUpdated = QDateTime::currentDateTime();
        m_timeLastChecked = m_timeUpdated;
    }

    void setTimeUpdatedToNow(void)
    {
        m_timeUpdated = QDateTime::currentDateTime();
    }

    void setCheckedTimeToNow(void)
    {
        m_timeLastChecked = QDateTime::currentDateTime();
    }

protected:
    static int tempNotSetValue;

    int m_temperature;
    QDateTime m_timeUpdated;
    QDateTime m_timeLastChecked;
};

qint64 TimestampedTemperature::elapsedSinceSet(void) const
{
    return QDateTime::currentMSecsSinceEpoch() - m_timeUpdated.toMSecsSinceEpoch();
}

qint64 TimestampedTemperature::elapsedSinceChecked(void)
{
    QDateTime cdt = QDateTime::currentDateTime();
    qint64 t = cdt.currentMSecsSinceEpoch() - m_timeLastChecked.toMSecsSinceEpoch();
    return t;
}

#endif // TIMESTAMPEDTEMPERATURE_H
