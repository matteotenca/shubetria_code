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

#ifndef FANCONTROLLERIO_H
#define FANCONTROLLERIO_H

#include <QObject>
#include <QQueue>

#include <QDateTime>        // For debugging

#include "device-io.h"
#include "fanprofiles.h"
#include "fancontrollerdata.h"
#include "dispatcher.h"

class FanControllerIO : public QObject
{
    Q_OBJECT

public:

    //---------------------------------------------------------------------

    enum ControlByte
    {
        NOTSET                          = 0x00, /**< Not used by device. Required for calcChecksum() */
        RX_NULL                         = 0x00, /**< Not used by device. Required for calcChecksum() */
        TX_NULL                         = 0x00, /**< Not used by device; Internal use only */

        TX_CurrentChannel               = 0x10, /**< (TX/RX?) Current selected channel (??) */

        TX_SetDisplayChannel_Ch0        = 0x20, /**< OUT Set the Recon's display to channel */
        TX_SetDisplayChannel_Ch1        = 0x21, /**< OUT Set the Recon's display to channel */
        TX_SetDisplayChannel_Ch2        = 0x22, /**< OUT Set the Recon's display to channel */
        TX_SetDisplayChannel_Ch3        = 0x23, /**< OUT Set the Recon's display to channel */
        TX_SetDisplayChannel_Ch4        = 0x24, /**< OUT Set the Recon's display to channel */

        TX_TempAndSpeed_Channel0        = 0x30, /**< OUT  Temp in F and current speed (RPM) */
        TX_TempAndSpeed_Channel1        = 0x31, /**< OUT   in F and current speed (RPM) */
        TX_TempAndSpeed_Channel2        = 0x32, /**< OUT   in F and current speed (RPM) */
        TX_TempAndSpeed_Channel3        = 0x33, /**< OUT   in F and current speed (RPM) */
        TX_TempAndSpeed_Channel4        = 0x34, /**< OUT   in F and current speed (RPM) */

        RX_TempAndSpeed_Channel0        = 0x40, /**< IN  Temp in F and speed (RPM)*/
        RX_TempAndSpeed_Channel1        = 0x41, /**< IN  Temp in F and speed (RPM)*/
        RX_TempAndSpeed_Channel2        = 0x42, /**< IN  Temp in F and speed (RPM)*/
        RX_TempAndSpeed_Channel3        = 0x43, /**< IN  Temp in F and speed (RPM)*/
        RX_TempAndSpeed_Channel4        = 0x44, /**< IN  Temp in F and speed (RPM)*/

        TX_ReqDeviceSettings            = 0x50, /**< OUT Device settings. See DeviceSetttingsFlag */
        TX_SetDeviceSettings            = 0x60, /**< OUT Set Device settings. See DeviceSetttingsFlag */
        RX_DeviceSettings               = 0x60, /**< IN  Device settings. See DeviceSetttingsFlag */

        TX_ReqAlarmAndSpeed_Channel0    = 0x70, /**< OUT Alarm Temperature and Max RPM*/
        TX_ReqAlarmAndSpeed_Channel1    = 0x71, /**< OUT Alarm Temperature and Max RPM*/
        TX_ReqAlarmAndSpeed_Channel2    = 0x72, /**< OUT Alarm Temperature and Max RPM*/
        TX_ReqAlarmAndSpeed_Channel3    = 0x73, /**< OUT Alarm Temperature and Max RPM*/
        TX_ReqAlarmAndSpeed_Channel4    = 0x74, /**< OUT Alarm Temperature and Max RPM*/

        TX_SetAlarmAndSpeed_Channel0    = 0x80, /**< OUT Alarm Temperature and Max RPM*/
        TX_SetAlarmAndSpeed_Channel1    = 0x81, /**< OUT Alarm Temperature and Max RPM*/
        TX_SetAlarmAndSpeed_Channel2    = 0x82, /**< OUT Alarm Temperature and Max RPM*/
        TX_SetAlarmAndSpeed_Channel3    = 0x83, /**< OUT Alarm Temperature and Max RPM*/
        TX_SetAlarmAndSpeed_Channel4    = 0x84, /**< OUT Alarm Temperature and Max RPM*/

        RX_AlarmAndSpeed_Channel0       = 0x80, /**< IN  Alarm Temperature and Max RPM*/
        RX_AlarmAndSpeed_Channel1       = 0x81, /**< IN  Alarm Temperature and Max RPM*/
        RX_AlarmAndSpeed_Channel2       = 0x82, /**< IN  Alarm Temperature and Max RPM*/
        RX_AlarmAndSpeed_Channel3       = 0x83, /**< IN  Alarm Temperature and Max RPM*/
        RX_AlarmAndSpeed_Channel4       = 0x84, /**< IN  Alarm Temperature and Max RPM*/

        TX_DeviceStatus                 = 0x90, /**< OUT Request Status: Ready/Not Ready */
        RX_DeviceStatus                 = 0xA0, /**< IN  Status: Ready/Not Ready */

        RX_ACK                          = 0xF0,
        RX_NAK                          = 0xFA

    };

    //---------------------------------------------------------------------

    enum DeviceSettingsFlag
    {
        Auto                            = 1U << 0,
        Celcius                         = 1U << 1,
        AudibleAlarm                    = 1U << 2
    };


    //---------------------------------------------------------------------

    /** Calculates the data checksum present at the end of data blocks.
      *
      * The BitFenix Recon data block has a checksum at the end of the data.
      * The checksum for input blocks the ctrlByte does \em not influence
      * the checksum and therefore RX_NULL should be passed to this function
      * if the data is input data. If the data is output data, then the
      * ctrlByte is used in the checksum calculation and therefore should be
      * set appropriately.
      *
      * @param ctrlByte     @see FanControllerIO::ControlByte.
      * @param blockLen     The number of bytes in \em block .
      * @param block        The data to calculate a checksum for.
      */
    static unsigned char calcChecksum(
        ControlByte ctrlByte,
        int blockLen,
        const unsigned char* block
    );


    //---------------------------------------------------------------------

    /* Parsed Input from the device
     */
    class Input
    {

        friend class FanControllerIO;

    public:

        Input();

        // Set from raw data
        bool set(int blockLen, const unsigned char* block);

    private:
        ControlByte m_controlByte;
        unsigned char m_checksum;
        int m_dataLen;              // This may be < sizeof(m_data)
        unsigned char m_data[5];
    };


    //---------------------------------------------------------------------

    class Request
    {

        friend class FanControllerIO;

        enum Category
        {
            Passive,
            Set_ChannelSettings,
            Set_DeviceSettings
        };

    public:
        Request();
        Request(const Request& ref);
        explicit Request(ControlByte controlByte);


        bool setURB(void)
        {
            return toURB (9, m_URB, true);
        }

        bool toURB(int blockLen, unsigned char *block, bool pad);

    private:
        Category m_category;
        ControlByte m_controlByte;
        int m_dataLen;              // This may be < sizeof(m_data)
        unsigned char m_data[4];    // Can only _send_ 4 data bytes
        // (incoming may have 5)
        unsigned char m_URB[9];     // Must be one byte longer than max URB size

        bool m_URB_isSet;
    };

    //---------------------------------------------------------------------

public:
    /* Constructors
     */
    explicit FanControllerIO(QObject *parent = 0);

    void connectSignals(void);

    bool connect(void);
    bool isConnected(void) const;
    void disconnect(void);
    void reset(void);


    FanControllerData& fanControllerData(void)
    {
        return m_fanControllerData;
    }

    bool setDeviceFlags(bool isCelcius, bool isAuto, bool isAudibleAlarm);
    bool setChannelSettings(int channel, unsigned thresholdF, unsigned speed);
    void setDisplayChannel(int channel);
    bool setFromProfile(const FanControllerProfile& profile);

    int minProbeTemp(bool inCelcius) const
    {
        return inCelcius ? -18 : 0;    /* C : F */
    }
    int maxProbeTemp(bool inCelcius) const
    {
        return inCelcius ? 124 : 255;    /* C : F */
    }

    void processTempAndSpeed(int channel, int tempF, int rpm, int maxRpm);
    void processAlarmTemp(int channel, int alarmTempF);
    void processManualSpeed(int channel, int rpm);
    void processDeviceSettings(bool isAuto, bool isCelcius, bool isAudibleAlarm);

    void doSoftwareAuto(int channel, int tempF);
    void doSoftwareAutoChannel(int channel, int tempF);

    int readCpuTemp(int probetemp, int channel);
    
    // Requests

    void requestDeviceFlags(void);

    void requestTempAndSpeed(int channel);

    void requestAlarmAndSpeed(int channel);

    bool isRequestQueueEmpty(void) const
    { return m_requestQueue.isEmpty(); }

    bool shutdown(void)
    {
        if (!isConnected())
            return true;
        if (isRequestQueueEmpty())
            return true;
        processRequestQueue();

        return isRequestQueueEmpty();
    }

protected:

    int rawToTemp(unsigned char byte) const;
    int rawToRPM(char highByte, char lowByte) const;

    void updateProcessedReqs(bool ack);
    void issueRequest(const Request& req);
    void processRequestQueue(void);

    inline void clearRequestQueue(void);

public slots:
    void onDispatcherSignal(EventDispatcher::TaskId taskId);
    void onRawData(QByteArray rawdata);

private:

    DeviceIO m_io_device;

    int m_pollNumber;

    QQueue<Request> m_requestQueue;

    FanControllerData m_fanControllerData;


    // For Debugging

    QDateTime m_lastPollTime;

    QDateTime m_pollTime_maxDelta_start;
    QDateTime m_pollTime_maxDelta_end;

    unsigned long m_pollTime_maxDelta;

    int m_maxQueueSize;

public:

    unsigned long maxPollDelta(void) const
    { return m_pollTime_maxDelta; }

    int maxReqQueueSize(void) const
    { return m_maxQueueSize; }

    // End For Debugging

};

void FanControllerIO::clearRequestQueue(void)
{
    m_requestQueue.clear();
}

#endif // FANCONTROLLERIO_H
