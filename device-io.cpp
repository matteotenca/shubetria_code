#include <QByteArray>
#include <QDebug>
#include "device-io.h"

#define PHOEBETRIA_HID_INPUT_BUFFSIZE 255

DeviceIO::DeviceIO(QObject *parent) :
    QObject(parent)
{
    m_device = NULL;
    hid_init();
}

bool DeviceIO::connect(unsigned short vendorId, unsigned short productId)
{
    m_device = hid_open(vendorId, productId, NULL);
    if (m_device) hid_set_nonblocking(m_device, 1); // set non-blocking

    return m_device != NULL;
}

void DeviceIO::disconnect(void)
{
    if (m_device == NULL) return;
    hid_close(m_device);
    m_device = NULL;
}

bool DeviceIO::isConnected(void) const
{
    return m_device != NULL;
}

int DeviceIO::sendData(const unsigned char* data, int len)
{
    return hid_write(m_device, data, len);
}

QString DeviceIO::lastErrorString(void) const
{
    QString err;

    err.fromWCharArray(hid_error(m_device));

    return err;
}

void DeviceIO::setBlocking(bool block)
{
    hid_set_nonblocking(m_device, block ? 0 : 1);
}

void DeviceIO::pollForData(void)
{
    unsigned char buff[PHOEBETRIA_HID_INPUT_BUFFSIZE];
    int bytesRead;

    if (!m_device) return;

    while ( (bytesRead = hid_read(m_device, buff, sizeof(buff))) > 0 )
    {
        //rawdata.fromRawData((const char*) buff, bytesRead);
        QByteArray rawdata;
        for (int i = 0; i < bytesRead; i++)
        {
            rawdata.append(buff[i]);
        }
        emit dataRX(rawdata);
    }


}
