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

#include "utils.h"

#include <QDir>
#include <QFile>


/* QByteArray::toHex() can do a similar thing, but as far as I know it cannot
 * be forced to format the output in the way this function does.
 */
QString toHexString(const unsigned char *data, int len)
{

    static const char* hexDigits = "0123456789ABCDEF";

    QString result;

    for (int i = 0; i < len; i++)
    {
        unsigned char cc = *(data + i);
        if (i != 0) result.append(' ');
        result.append(hexDigits[(cc >> 4) & 0x0f]);
        result.append(hexDigits[cc & 0x0f]);
    }

    return result;
}

/*! Check the settings path. If it doesn't exist, create it.

    Checks that the \path exists and creates the path if it doesn't.
    Returns true if the path exists (or was successfully created), otherwise
    false.
 */
bool checkPath(const QString& path)
{
    QDir dir(path);
    if (!dir.exists())
    {
        if (!dir.mkpath(path))
            return false;
    }
    return dir.exists();
}


bool fileExists(const QString& filename)
{
    QFile f(filename);

    return f.exists();
}
