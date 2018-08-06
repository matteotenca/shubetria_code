#include "timestampedtemperature.h"

int TimestampedTemperature::tempNotSetValue = 65535;

TimestampedTemperature::TimestampedTemperature() :
    m_temperature(tempNotSetValue),
    m_timeUpdated()
{
}
