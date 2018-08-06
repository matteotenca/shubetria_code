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

#ifndef PHOEBETRIA_UTILS_H
#define PHOEBETRIA_UTILS_H

#include <QString>

#ifdef QT_DEBUG
#   define PHOEBETRIA_STUB_FUNCTION \
        qDebug() << "Stub function called. File:" \
        << __FILE__ << "Line:" << __LINE__;
#else
#   define PHOEBETRIA_STUB_FUNCTION \
        {}
#endif

QString toHexString(const unsigned char *data, int len);

bool checkPath(const QString& path);
bool fileExists(const QString& filename);

#endif // PHOEBETRIA_UTILS_H
