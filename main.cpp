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

#include "main.h"

#include <QApplication>
#include <QMessageBox>

#include <stdlib.h>

#include "gui_mainwindow.h"
#include "shubetriaapp.h"
#include "device-io.h"

#include <QTranslator>
#include <QDir>

CloseHelper::CloseHelper(QObject *parent)
    : QObject(parent)
{
}

void CloseHelper::onLastWindowClosed(void)
{
    ph_shutdown();
}

int main(int argc, char *argv[])
{

    ShubetriaApp a(argc, argv);
    
    QLocale mylocale;
    //    QString lan = mylocale.language();
    QString locale = QLocale::system().name();
    //    QString langfilename = "test_" + locale;
//     QString langfilename = "shubetria." + locale;
    QString langfilename = "shubetria";
    QString langfilepath = ":/translations";
    QString langprefix = ".";
    bool test = false;
    QString tifotto = QDir::currentPath();
    QStringList langlist = mylocale.uiLanguages();
    QTranslator translator;
    test = translator.load(mylocale, langfilename, langprefix, langfilepath);
    // test = translator.load(langfilename, langfilepath);;
//    test = translator.
    //    if ( ! test) {
    //        printf("%s", "Error.");
    //        exit(1);
    //    }
//    translator.load(langfilename, langfilepath);
    a.installTranslator(&translator);
    gui_MainWindow w;
    CloseHelper chelper;

    /* Connect to the lastWindowClosed signal so that we can do cleanup
       and/or save preferences, etc
      */
    QObject::connect(&a, SIGNAL(lastWindowClosed()),
                     &chelper, SLOT(onLastWindowClosed()));
    w.show();

    return a.exec();
}
