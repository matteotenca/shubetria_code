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

#ifndef DEVICEIO_H
#define DEVICEIO_H

#include <QObject>
#include "hidapi.h"

class DeviceIO : public QObject
{
    Q_OBJECT
public:
    explicit DeviceIO(QObject *parent = 0);

    bool connect(unsigned short vendorId, unsigned short productId);

    void disconnect(void);

    bool isConnected(void) const;

    int sendData(const unsigned char *data, int len);

    QString lastErrorString(void) const;

    void setBlocking(bool block);

signals:
    void dataRX(QByteArray data);

public slots:
    void pollForData(void);

private:
    hid_device *m_device;
};

#endif // DEVICEIO_H
