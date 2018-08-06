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

#include "fancontrollerio.h"

#include <QDebug>

#include "utils.h"
#include "shubetriaapp.h"
#include "smc_cputemp.h"

// Maximum length of the command queue. Set to -1 for no max length
#define MAX_COMMANDQUEUE_LEN -1

// Uncomment to output raw hex to/from the device
//#define SHUBETRIA_OUTPUT_RAW_HEX 1

#define MAX_FAN_CHANNELS 5

//---------------------------------------------------------------------

static const unsigned short int HID_ProductId =   28928;
static const unsigned short HID_VendorId =  3141;


/**********************************************************************
  FanControllerIO::Input
 *********************************************************************/

FanControllerIO::Input::Input()
    : m_controlByte(FanControllerIO::NOTSET),
      m_checksum(0),
      m_dataLen(0)
{
}

bool FanControllerIO::Input::set(int blockLen, const unsigned char *block)
{
    /* HID Report as recieved *from* the device
     *
     * Structure for the BitFenix Recon (BfxRecon):
     *
     *  Byte#
     *    0     Data Length (len). The number of bytes of data (0 <= len <= 5)
     *    1     Control Byte (see FanControllerIO::ControlByte)
     *    2-7   Data + checksum. The checksum at the byte after 3 + len.
     *          The bytes after the checksum (if any) are zero padding.
     */

    if (blockLen < 3)   // Need at least Data Length, Control Byte, Checksum
    {
        qDebug ("FanController::parseRawData() not enough data");
        return false;
    }

    m_dataLen = *block;
    m_controlByte = (ControlByte) (*(block+1));

    if (m_controlByte == FanControllerIO::NOTSET) return false;

    /* This check is not here just for paranoia... sometimes byte 0 actually
       *is* > 7; i.e. the block doesn't follow the general structure documented
       above. I have no idea if these blocks actually mean something or if
       they are transmission errors. This doesn't happen very often (it might
       not happen in a session at all) but it does occassionally so the check
       must stay.
      */
    if (m_dataLen > 7)
    {
#       ifdef QT_DEBUG
        qDebug() << "Got input with data block length set to"
                    << m_dataLen
                    << "(ignoring block)";
        qDebug() << "Block is:"
                 << toHexString(block, blockLen);
#       endif
        return false;
    }

    for (int i = 2; i < m_dataLen; i++)
    {
        *(m_data + i - 2) = *(block + i);
    }

    m_checksum = *(block + m_dataLen);

    // Actual data length does not include bytes 0 and 1 of the input
    m_dataLen = m_dataLen - 2;

    unsigned calculatedChecksum = FanControllerIO::calcChecksum(
                                      RX_NULL,
                                      *(block)-1,   // Skipping the first byte
                                      block+1
                                  );
    if (m_checksum != calculatedChecksum)
    {
#ifdef QT_DEBUG
        qDebug() << "File:" << QString(__FILE__)
                 << "Line:" << QString::number(__LINE__)
                 << "++++ Checksum mismatch."
                 << "Expected" << QString::number(m_checksum)
                 << "Got" << QString::number(calculatedChecksum);

        qDebug() << "Block is:"
                 << toHexString(block, blockLen);
#endif
        return false; // Checksum failure
    }


    return true;
}


/**********************************************************************
  FanControllerIO::Request
 *********************************************************************/


FanControllerIO::Request::Request()
{
    m_category = Passive;
    m_controlByte = TX_NULL;
    m_dataLen = 0;
    m_URB_isSet = false;
}

FanControllerIO::Request::Request(const Request& ref)
{
    m_controlByte = ref.m_controlByte;
    m_dataLen = ref.m_dataLen;
    m_URB_isSet = ref.m_URB_isSet;
    m_category = ref.m_category;

    for (unsigned i = 0; i < sizeof(m_data); i++)
    {
        *(m_data + i) = *(ref.m_data + i);
    }
    if (m_URB_isSet)
    {
        for (unsigned i = 0; i < sizeof(m_URB); i++)
        {
            *(m_URB + i) = *(ref.m_URB + i);
        }
    }
}

FanControllerIO::Request::Request(ControlByte controlByte)
{
    m_controlByte = controlByte;
    m_dataLen = 0;
    setURB();
}

bool FanControllerIO::Request::toURB(int blockLen,
                                     unsigned char* block,
                                     bool pad)
{
    int i;

    *block = 0x00;              // URB report ID
    *(block + 1) = m_dataLen + 2;
    *(block + 2) = m_controlByte;

    for (i = 3; i < m_dataLen + 3; i++)
    {
        *(block + i) = m_data[i-3];
    }

    *(block + i) = FanControllerIO::calcChecksum(m_controlByte,
                   m_dataLen,
                   m_data) - 1;
    i++;

    if (pad)
    {
        for (; i < blockLen; i++)
        {
            *(block + i) = 0x00;
        }
    }

    m_URB_isSet = true;

    return true;
}


/**********************************************************************
  FanControllerIO
 *********************************************************************/

/* Static functions */

unsigned char FanControllerIO::calcChecksum(
    ControlByte ctrlByte,
    int blockLen,
    const unsigned char *block
)
{

    unsigned checksum;

    checksum = blockLen + 1;
    checksum += ctrlByte;

    for (int i = 0; i < blockLen; ++i)
    {
        checksum += *(block + i);
    }
    checksum ^= 0xff;
    checksum = (checksum + 1) & 0xff;

    return checksum;
}

//---------------------------------------------------------------------

FanControllerIO::FanControllerIO(QObject *parent) :
    QObject(parent),
    m_pollNumber(0),
    m_fanControllerData(),
    m_lastPollTime(QDateTime::currentDateTime()),
    m_pollTime_maxDelta(0),
    m_maxQueueSize(0)
{
}

void FanControllerIO::connectSignals(void)
{
    QObject::connect(&m_io_device, SIGNAL(dataRX(QByteArray)),
                     this, SLOT(onRawData(QByteArray)));

    QObject::connect(&ph_shubetriaApp()->dispatcher(), SIGNAL(task(EventDispatcher::TaskId)),
                     this, SLOT(onDispatcherSignal(EventDispatcher::TaskId)));

}

bool FanControllerIO::connect(void)
{
    bool r = m_io_device.connect(HID_VendorId, HID_ProductId);

    // TODO if (r) emit deviceConnected();

    return r;
}


bool FanControllerIO::isConnected(void) const
{
    return m_io_device.isConnected();
}


void FanControllerIO::disconnect(void)
{
    m_io_device.disconnect();

    // TODO emit deviceDisconnected();
}

void FanControllerIO::reset(void)
{
    clearRequestQueue();
}

void FanControllerIO::onDispatcherSignal(EventDispatcher::TaskId taskId)
{
    if (!isConnected()) return;

    if (signalsBlocked()) return;

    // For debugging
    {
        QDateTime now(QDateTime::currentDateTime());
        unsigned long delta;
        delta = now.toMSecsSinceEpoch() - m_lastPollTime.toMSecsSinceEpoch();
        if (delta > m_pollTime_maxDelta)
        {
            m_pollTime_maxDelta = delta;
            m_pollTime_maxDelta_start = m_lastPollTime;
            m_pollTime_maxDelta_end = now;
        }
        m_lastPollTime = now;

        if (m_requestQueue.size() > m_maxQueueSize)
            m_maxQueueSize = m_requestQueue.size();
    }


    if (taskId == EventDispatcher::Tick)
    {
        processRequestQueue();
        return;
    }

    // Don't issue any more requests if we're shutting down
    if (ph_isShuttingDown())
        return;

    if (taskId == EventDispatcher::CheckForDeviceData)
    {
        m_io_device.pollForData();
        return;
    }

    if (taskId == EventDispatcher::ReqDeviceFlags)
    {
        requestDeviceFlags();
        return;
    }

    if (taskId == EventDispatcher::ReqAlarmTempAndManualRpm)
    {
        for (int i = 0; i < MAX_FAN_CHANNELS; ++i)
            requestAlarmAndSpeed(i);
    }
    else if (taskId == EventDispatcher::ReqTempAndCurrRpmAndMaxRpm)
    {
        for (int i = 0; i < MAX_FAN_CHANNELS; ++i)
            requestTempAndSpeed(i);
    }
    else if (taskId == EventDispatcher::ResetFanRampTemps)
    {
        ph_fanControllerData().clearRampTemps();
    }
}

int FanControllerIO::readCpuTemp(int probetemp, int channel)
{
    int temp = 0;
    char t[5];
 
    
    if ( channel != 2 ) {
        temp = probetemp;
#ifdef QT_DEBUG
        qDebug() << "Channel: "
        <<  QString::number(channel)
        << "Reporting probetemp: "
        <<  QString::number(temp);
#endif
    }
    else {
        stpcpy(t, "TC0D");
        temp = newmain(t);
        temp = round( temp * 9/5.0 + 32);
        
#ifdef QT_DEBUG
        qDebug() << "Channel: "
        <<  QString::number(channel)
        << "Reporting cpu temp: "
        <<  QString::number(temp);
#endif
            
    }
//        else {
//            // Display DLL related errors.
//            qDebug() << "Error: Core Temp's shared memory could not be read";
//            qDebug() << "Error number: "
//            << QString::number(ctp.GetDllError());
//            //wprintf(L"Error description: %s\n", ctp.GetErrorMessage());
//        }
    
    
    return temp;
}

void FanControllerIO::onRawData(QByteArray rawdata)
{
    Input parsedData;
    bool inputParsed;

#if defined QT_DEBUG && SHUBETRIA_OUTPUT_RAW_HEX
    qDebug() << "#### Raw Data From Device:"
             << toHexString((const unsigned char*)rawdata.constData(), rawdata.length());
#endif

    inputParsed = parsedData.set(rawdata.length(), (const unsigned char*)rawdata.constData());

    if (!inputParsed)
    {
#ifdef QT_DEBUG
        qDebug() << "Could not parse input."
                 << "File:" << QString(__FILE__)
                 << "Line:" << QString::number(__LINE__);
#endif
        return;
    }

    switch (parsedData.m_controlByte)
    {
    case RX_ACK:
    case RX_NAK:
        break;

    case RX_TempAndSpeed_Channel0:
    case RX_TempAndSpeed_Channel1:
    case RX_TempAndSpeed_Channel2:
    case RX_TempAndSpeed_Channel3:
    case RX_TempAndSpeed_Channel4:

        processTempAndSpeed(
            // Channel
            parsedData.m_controlByte - RX_TempAndSpeed_Channel0,
            // (c) 2018 Shub
            // read cpu temp, not probe temp
            // Current temperature
            // old:
            // rawToTemp(rawdata.at(2)),
            // new:
            readCpuTemp(rawToTemp(rawdata.at(2)), parsedData.m_controlByte - RX_TempAndSpeed_Channel0),

            // Current RPM
            rawToRPM(rawdata.at(4), rawdata.at(3)),
            // RPM at alarm temp (MAX RPM)
            rawToRPM(rawdata.at(6), rawdata.at(5))
        );
        break;

    case RX_AlarmAndSpeed_Channel0:
    case RX_AlarmAndSpeed_Channel1:
    case RX_AlarmAndSpeed_Channel2:
    case RX_AlarmAndSpeed_Channel3:
    case RX_AlarmAndSpeed_Channel4:

        processAlarmTemp(
            // Channel
            parsedData.m_controlByte - RX_AlarmAndSpeed_Channel0,
            // Alarm temperature
            rawToTemp(rawdata.at(2))
        );

        processManualSpeed(
            // Channel
            parsedData.m_controlByte - RX_AlarmAndSpeed_Channel0,
            // Manual speed
            rawToRPM(rawdata.at(4), rawdata.at(3))
        );
        break;

    case RX_DeviceSettings:

        processDeviceSettings(
            // Is Auto
            rawdata[2] & FanControllerIO::Auto ? false : true,
            // Is Celcius
            rawdata[2] & FanControllerIO::Celcius ? false : true,
            // Is Audible Alarm
            rawdata[2] & FanControllerIO::AudibleAlarm ? true : false
        );
        break;

    default:

#ifdef QT_DEBUG
        qDebug() << "Unhandled response code:"
                 << QString::number(parsedData.m_controlByte);
#endif
        break;

    }
}

void FanControllerIO::processTempAndSpeed(int channel,
        int tempF,
        int rpm,
        int maxRpm)
{
    m_fanControllerData.updateTempF(channel, tempF);
    m_fanControllerData.updateRPM(channel, rpm);
    m_fanControllerData.updateMaxRPM(channel, maxRpm);
}

void FanControllerIO::processAlarmTemp(int channel, int alarmTempF)
{
    m_fanControllerData.updateAlarmTemp(channel, alarmTempF);
}

void FanControllerIO::processManualSpeed(int channel, int rpm)
{
    m_fanControllerData.updateManualRPM(channel, rpm);
}


void FanControllerIO::processDeviceSettings(bool isAuto,
        bool isCelcius,
        bool isAudibleAlarm)
{
    m_fanControllerData.updateIsAuto(isAuto);
    m_fanControllerData.updateIsCelcius(isCelcius);
    m_fanControllerData.updateIsAudibleAlarm(isAudibleAlarm);
}

int FanControllerIO::rawToTemp(unsigned char byte) const
{
    return (int)byte;
}

int FanControllerIO::rawToRPM(char highByte, char lowByte) const
{
    return (unsigned char)highByte*256 + (unsigned char)lowByte;
}


//---------------------------------------------------------------------
// Command queue
//---------------------------------------------------------------------

void FanControllerIO::issueRequest(const Request& req)
{
    // Discard old commands
    if (MAX_COMMANDQUEUE_LEN != -1)
    {
        while (m_requestQueue.length() > MAX_COMMANDQUEUE_LEN - 1)
        {
            m_requestQueue.dequeue();
        }
    }

    m_requestQueue.enqueue(req);
}

void FanControllerIO::processRequestQueue(void)
{
    /* We don't want to process all the commands at once because the device
     * only supports sending one command at a time; i.e. if more than one
     * command is sent only the last command sent is processed by the device
     */
    if (m_requestQueue.isEmpty()) return;

    Request r = m_requestQueue.dequeue();

    if (!r.m_URB_isSet)
    {
#       ifdef QT_DEBUG
        qDebug() << "Attempt to issue request without URB set";
#       endif

        return;
    }

#if defined QT_DEBUG && SHUBETRIA_OUTPUT_RAW_HEX
    qDebug() << "#### Raw Data To Device:  "
             << toHexString(r.m_URB, sizeof(r.m_URB));
#endif

    bool bs = blockSignals(true);

    m_io_device.sendData(r.m_URB, sizeof(r.m_URB));

    blockSignals(bs);
}


//---------------------------------------------------------------------
// Passive Requests
//---------------------------------------------------------------------
void FanControllerIO::requestDeviceFlags(void)
{
    Request r(TX_ReqDeviceSettings);
    issueRequest(r);
}


void FanControllerIO::requestTempAndSpeed(int channel)
{
    ControlByte cb = (ControlByte)(TX_TempAndSpeed_Channel0 + channel);

    Request r(cb);
    issueRequest(r);
}


void FanControllerIO::requestAlarmAndSpeed(int channel)
{
    ControlByte cb = (ControlByte)(TX_ReqAlarmAndSpeed_Channel0 + channel);

    Request r(cb);
    issueRequest(r);
}


//---------------------------------------------------------------------
// Active Requests
//---------------------------------------------------------------------

bool FanControllerIO::setDeviceFlags(bool isCelcius,
                                     bool isAuto,
                                     bool isAudibleAlarm)
{
    unsigned char bits;

    bits = !isCelcius ? FanControllerIO::Celcius : 0;
    bits |= !isAuto ? FanControllerIO::Auto : 0;
    bits |= isAudibleAlarm ? FanControllerIO::AudibleAlarm : 0;

    Request req;

    req.m_category = Request::Set_DeviceSettings;
    req.m_controlByte = TX_SetDeviceSettings;

    req.m_dataLen = 1;
    req.m_data[0] = bits;

    req.setURB();

    issueRequest(req);

    m_pollNumber = 0;

    return true;
}

bool FanControllerIO::setChannelSettings(int channel,
        unsigned thresholdF,
        unsigned speed)
{
    Request req;

    req.m_category = Request::Set_ChannelSettings;
    req.m_controlByte = (ControlByte)(TX_SetAlarmAndSpeed_Channel0 + channel);

    req.m_dataLen = 3;
    req.m_data[0] = thresholdF;
    req.m_data[1] = speed % 256;
    req.m_data[2] = speed / 256;

    req.setURB();

    issueRequest(req);

    m_pollNumber = 0;

    return true;
}

void FanControllerIO::setDisplayChannel(int channel)
{
    Request req;


    req.m_category = Request::Set_DeviceSettings;
    req.m_controlByte = (ControlByte)(TX_SetDisplayChannel_Ch0 + channel);

    req.m_dataLen = 0;

    req.setURB();

    issueRequest(req);

    m_pollNumber = 0;
}

bool FanControllerIO::setFromProfile(const FanControllerProfile& profile)
{
    setDeviceFlags(profile.isCelcius(), profile.isAuto(), profile.isAudibleAlarm());

    for (int i = 0; i < FC_MAX_CHANNELS; i++)
    {
        BasicChannelData chd = profile.getChannelSettings(i);
        int speed = (chd.speed == -1 || chd.speed == 65535)
                        ? m_fanControllerData.lastRPM(i)
                        : chd.speed;
        setChannelSettings(i, chd.alarmTemp, speed);
    }

    m_pollNumber  = 0;

    return true;
}



