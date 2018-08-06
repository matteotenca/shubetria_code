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

#include "themes.h"

#include <QDir>
#include <QFile>

#include "preferences.h"
#include "shubetriaapp.h"

Themes::Themes()
{
}

/* Populates "dest" with files with .qss extension in the directory
   Themes::themePath().

   dest will be cleared even if an error occurs

   Check dest->size() for the number of results

   The results do NOT include the filepath in the names
*/

void Themes::getCustomThemeList(ThemeNameAndFilenameList *dest)
{
    dest->clear();

    QDir dir(themePath());

    if (!dir.exists())
        return;

    QFileInfoList files = dir.entryInfoList(QDir::Files);

    int m = files.size();


    for (int i = 0; i < m; ++i)
    {
        QString filename(files.at(i).fileName());
        if (filename.endsWith(".qss"))
        {
            ThemeNameAndFilename item;
            item.name = filename;
            item.fileNameAndPath = prependPath(filename);

            dest->append(item);
        }
    }

}

QString Themes::themePath(void)
{
    QString path;

    path = Preferences::filepath();
    path = QDir::toNativeSeparators(path);
    path = path + QDir::separator() + "themes" + QDir::separator();

    return path;
}

QString Themes::getStyleSheet(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly))
        return QString();

    return QLatin1String(file.readAll());
}

bool Themes::setAppStyleSheet(const QString& filename)
{
    QString ss = getStyleSheet(filename);
    ss = ss.simplified();
    if (ss.isEmpty() || ss.isNull())
        return false;

    ph_shubetriaApp()->setStyleSheet(ss);

    return true;
}

