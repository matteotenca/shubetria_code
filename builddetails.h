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

#ifndef BUILDDETAILS_H
#define BUILDDETAILS_H

#include <QString>

#define PHOEBETRIA_VERSION  1
#define PHOEBETRIA_MAJOR_REVISION   4
#define PHOEBETRIA_MINOR_REVISION   0

#define PHOEBETRIA_STATUS_STR       ""

class BuildDetails
{
public:
    BuildDetails();

    static QString versionStr(void);
    static QString buildDateTimeStr(void);
    static QString qtVersion(void);

};

#endif // BUILDDETAILS_H
