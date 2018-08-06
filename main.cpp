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
#include "phoebetriaapp.h"
#include "device-io.h"


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

    PhoebetriaApp a(argc, argv);
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
