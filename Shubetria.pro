#-------------------------------------------------
#
# Project created by QtCreator 2012-08-02T07:50:29
#
#-------------------------------------------------

QT       += core gui sql

#Added in anticipation of QT 5
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# (c) Shub 2018
greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

TARGET = Shubetria
TEMPLATE = app

TRANSLATIONS =  translations/shubetria.it_IT.ts \
                translations/shubetria.en_US.ts
#                translations/shubetria.en_EN.ts


PREFIX = /usr
BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share
ICONDIR = $$DATADIR/icons/hicolor

INSTALLS += target icon16 icon22 icon32 icon48 icon64 icon128 icon256 pixmap

target.path = $${BINDIR}

icon16.path = $${ICONDIR}/16x16/apps
icon16.files += Images/icons/16x16/shubetria.png

icon22.path = $${ICONDIR}/22x22/apps
icon22.files += Images/icons/22x22/shubetria.png

icon32.path = $${ICONDIR}/32x32/apps
icon32.files += Images/icons/32x32/shubetria.png

icon48.path = $${ICONDIR}/48x48/apps
icon48.files += Images/icons/48x48/shubetria.png

icon64.path = $${ICONDIR}/64x64/apps
icon64.files += Images/icons/64x64/shubetria.png

icon128.path = $${ICONDIR}/128x128/apps
icon128.files += Images/icons/128x128/shubetria.png

icon256.path = $${ICONDIR}/256x256/apps
icon256.files += Images/icons/256x256/shubetria.png

icon512.path = $${ICONDIR}/512x512/apps
icon512.files += Images/icons/512x512/shubetria.png

pixmap.path = $${DATADIR}/pixmaps
pixmap.files += Images/icons/48x48/shubetria.png

# coretemp.path = $${BINDIR}

SOURCES += QCustomPlot/qcustomplot.cpp \
    appinfo.cpp \
    averager.cpp \
    builddetails.cpp \
    database/dbmanager.cpp \
    database/maindb.cpp \
    database/maindb_schema.cpp \
    device-io.cpp \
    dispatcher.cpp \
    fanchanneldata.cpp \
    fancontrollerdata.cpp \
    fanprofiles.cpp \
    gui_about.cpp \
    gui_diagnostic.cpp \
    gui_help.cpp \
    gui_mainwindow.cpp \
    gui_preferences.cpp \
    gui_profiles.cpp \
    gui_setmanualrpm.cpp \
    gui_softwareautosetup.cpp \
    languages.cpp \
    main.cpp \
    preferences.cpp \
    shubetriaapp.cpp \
    softwareauto/fanramp.cpp \
    themes.cpp \
    timestampedtemperature.cpp \
    utils.cpp


HEADERS  += QCustomPlot/qcustomplot.h \
    appinfo.h \
    averager.h \
    bfx-recon/bfxrecon.h \
    builddetails.h \
    database/dbmanager.h \
    database/maindb.h \
    database/maindb_schema.h \
    device-io.h \
    dispatcher.h \
    fanchanneldata.h \
    fancontrollerdata.h \
    fanprofiles.h \
    gui_about.h \
    gui_diagnostic.h \
    gui_help.h \
    gui_mainwindow.h \
    gui_preferences.h \
    gui_profiles.h \
    gui_setmanualrpm.h \
    gui_softwareautosetup.h \
    hidapi/hidapi/hidapi.h \
    languages.h \
    main.h \
    preferences.h \
    shubetriaapp.h \
    softwareauto/fanramp.h \
    themes.h \
    timestampedtemperature.h \
    utils.h


FORMS    += gui_mainwindow.ui \
    gui_about.ui \
    gui_preferences.ui \
    gui_softwareautosetup.ui \
    gui_profiles.ui \
    gui_setmanualrpm.ui \
    gui_diagnostic.ui \
    gui_help.ui

RESOURCES += \
    gui_resources.qrc \
    gui_locale.qrc \
#    gui_languages.qrc \
    gui_help.qrc

#-------------------------------------------------
# Config for the HID API library
#-------------------------------------------------

INCLUDEPATH += hidapi/hidapi/ \
               database/ \
               softwareauto/ \
               QCustomPlot/
               
HEADERS += hidapi/hidapi/hidapi.h

#-------------------------------------------------
# Platform specific
#-------------------------------------------------

win32 {
    SOURCES += hidapi/windows/hid.c \
               bfx-recon/windows/fancontrollerio.cpp
    HEADERS += Windows/coretemp/GetCoreTempInfo.h \
               bfx-recon/windows/fancontrollerio.h
    INCLUDEPATH += Windows/coretemp \
                 bfx-recon/windows
    LIBS += -L..\\shubetria_code\\Windows\\coretemp -lsetupapi -lGetCoreTempInfo
    # QMAKE_LIBDIR += .\\Windows\\coretemp
    # -lGetCoreTempInfo coretemp\\GetCoreTempInfo.lib
    RC_FILE = Windows/Shubetria.rc
    CONFIG += exceptions rtti
}

unix:!macx {
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0
    SOURCES += hidapi/libusb/hid.c bfx-recon/linux/fancontrollerio.cpp
    HEADERS += bfx-recon/linux/fancontrollerio.h
    INCLUDEPATH += bfx-recon/linux
    LIBS += -lusb-1.0 -ludev -lrt
    INSTALLS += desktop udev
    desktop.path = $${DATADIR}/applications
    desktop.files += $${TARGET}.desktop
    udev.path = /etc/udev/rules.d
    udev.files += 99-fancontroller.rules
}

macx {

    QMAKE_INFO_PLIST = mac/Info.plist
    #QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    #QMAKE_MAC_SDK=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk
    LIBS += -framework CoreFoundation -framework IOKit
    SOURCES += hidapi/mac/hid.c bfx-recon/mac/fancontrollerio.cpp bfx-recon/mac/smc_temp.cpp bfx-recon/mac/smc.cpp
    HEADERS += bfx-recon/mac/fancontrollerio.h bfx-recon/mac/smc_temp.h bfx-recon/mac/smc.h
    INCLUDEPATH += bfx-recon/mac
    ICON = mac/Shubetria.icns
    APP_QML_FILES.files = mac/qt_menu.nib
    APP_QML_FILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += APP_QML_FILES
    DISTFILES += mac/Shubetria.icns

}

OTHER_FILES +=

DISTFILES += translations/shubetria.en_US.ts \
    translations/shubetria.it_IT.ts
#    translations/shubetria.en_US.qm \
#    translations/shubetria.it_IT.qm
#   translations/shubetria.en_EN.qm \
#    translations/shubetria.en_EN.ts \
