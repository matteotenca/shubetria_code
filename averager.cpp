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

#include "averager.h"

#include <stdlib.h>
#include <cmath>

#include <QDebug>

// const int Averager::m_sampleSize = 10;

const int Averager::m_sampleSize = 2;

Averager::Averager() :
    m_samples(NULL),
    m_pos(0),
    m_storedAverage(0),
    m_totalRealSamples(0)
{
    m_samples = new int[m_sampleSize];
    for (int i = 0; i < m_sampleSize; ++i)
        m_samples[i] = 0;

    m_pos = 0;
}

Averager::~Averager()
{
    if (m_samples)
        delete [] m_samples;
}

void Averager::addSampleValue(int value)
{
    *(m_samples + m_pos) = value;
    m_pos++;
    m_pos %= m_sampleSize;

    m_totalRealSamples++;

    updateStoredAverage();
}

void Averager::setAllSamplesToValue(int value)
{
    m_pos = 0;

    for (int i = 0; i < m_sampleSize; ++i)
        m_samples[i]  = value;

    m_totalRealSamples = 0;
    updateStoredAverage();

}

QList<int> Averager::getSampleValues(void) const
{
    QList<int> r;

    for (int i = 0; i < m_sampleSize; ++i)
        r.append(m_samples[i]);

    return r;
}


int Averager::updateStoredAverage(void)
{
    double total = 0;
    for (int i = 0; i < m_sampleSize; ++i)
    {
        total += m_samples[i];
    }

    m_storedAverage = ceil(total / m_sampleSize);
    return m_storedAverage;
}

