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

#ifndef THEMES_H
#define THEMES_H

#include <QString>
#include <QList>

typedef struct
{
    QString name;
    QString fileNameAndPath;
} ThemeNameAndFilename;

enum BuiltInStyleSheets
{
    Shubetria_Stylesheet_Standard,
    Shubetria_Stylesheet_Dark
};

typedef QList<ThemeNameAndFilename> ThemeNameAndFilenameList;

class Themes
{
public:
    Themes();

    static void getCustomThemeList(ThemeNameAndFilenameList *dest);

    static QString themePath(void);

    static QString getStyleSheet(const QString& filename);

    static bool setAppStyleSheet(const QString& filename);

    inline static QString prependPath(const QString& filename);

    inline static QString getBuiltInStyleSheetName(enum BuiltInStyleSheets style = Shubetria_Stylesheet_Standard);
    inline static bool setAppToBuiltInStyleSheet(BuiltInStyleSheets style = Shubetria_Stylesheet_Standard);
};


QString Themes::prependPath(const QString& filename)
{
    return themePath() + filename;
}

QString Themes::getBuiltInStyleSheetName(enum BuiltInStyleSheets style)
{
    if (style == Shubetria_Stylesheet_Standard)
        return QString(":/other/Shubetria.qss");
    else
        return QString(":/other/Shubetria-Dark.qss");
}

bool Themes::setAppToBuiltInStyleSheet(enum BuiltInStyleSheets style)
{
    QString ss = getBuiltInStyleSheetName(style);
    return setAppStyleSheet(ss);
}

#endif // THEMES_H
