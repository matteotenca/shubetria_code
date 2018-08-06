/*  Copyright 2012 Craig Robbins and Christopher Ferris
    Copyright 2018 Matteo Tenca

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

#ifndef BUILDDETAILS_H
#define BUILDDETAILS_H

#include <QString>

#define SHUBETRIA_VERSION  0
#define SHUBETRIA_MAJOR_REVISION   0
#define SHUBETRIA_MINOR_REVISION   1

#define SHUBETRIA_STATUS_STR       ""

class BuildDetails
{
public:
    BuildDetails();

    static QString versionStr(void);
    static QString buildDateTimeStr(void);
    static QString qtVersion(void);

};

#endif // BUILDDETAILS_H
