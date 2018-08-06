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

#include "preferences.h"

#include <QSettings>
#include <QFileInfo>

#include "themes.h"
#include "languages.h"

static const char* key_startMinimized       = "UserPrefs/startMinimized";
static const char* key_minimizeToTray       = "UserPrefs/minimizeToTray";
static const char* key_alwaysShowTrayIcon   = "UserPrefs/alwaysShowTrayIcon";
static const char* key_showTrayIconTT       = "UserPrefs/showTrayIconTooltips";
static const char* key_useLogScaleRpmSliders  = "UserPrefs/useLogScaleRpmSliders";
static const char* key_startupProfile       = "UserPrefs/startupProfile";
static const char* key_shutdownProfile      = "UserPrefs/shutdownProfile";
static const char* key_quitOnClose          = "UserPrefs/quitOnClose";
static const char* key_windowGeometry       = "UserPrefs/windowGeometry";
static const char* key_windowState          = "UserPrefs/windowState";
static const char* key_windowProfileGeometry        = "UserPrefs/windowProfileGeometry";
static const char* key_splitterProfileState         = "UserPrefs/windowProfileGeometry";
static const char* key_windowSWAutoGeometry         = "UserPrefs/windowSWAutoGeometry";
static const char* key_windowAboutGeometry          = "UserPrefs/windowAboutGeometry";
static const char* key_windowPrefsGeometry          = "UserPrefs/windowPrefsGeometry";
static const char* key_windowDiagGeometry        = "UserPrefs/windowDiagGeometry";
static const char* key_windowHelpGeometry        = "UserPrefs/windowHelpGeometry";
static const char* key_applicationLanguage      = "UserPrefs/applicationLanguage";

static const char* keyBase_channelName          = "UserPrefs/channelName";
static const char* keyBase_probeName            = "UserPrefs/probeName";

static const char* key_stylesheet               = "UserPrefs/stylesheet";
static const char* key_showChannelLabels     = "UserPrefs/showChannelLabels";

Preferences::Preferences(QObject *parent) :
    QObject(parent),
    m_settings(QSettings::IniFormat, QSettings::UserScope,
               "Shubetria", "Shubetria")

{
}

QString Preferences::filepath(void)
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       "Shubetria",
                       "Shubetria");
    return QFileInfo(settings.fileName()).path();
}

/****************************************************************************
 Access functions
 ****************************************************************************/

bool Preferences::startMinimized(bool defaultVal) const
{
    return m_settings.value(key_startMinimized, defaultVal).toBool();
}

bool Preferences::minimizeToTray(bool defaultVal) const
{
    return m_settings.value(key_minimizeToTray, defaultVal).toBool();
}

bool Preferences::alwaysShowTrayIcon(bool defaultVal) const
{
    return m_settings.value(key_alwaysShowTrayIcon, defaultVal).toBool();
}

bool Preferences::showTrayIconTooltips(bool defaultVal) const
{
    return m_settings.value(key_showTrayIconTT, defaultVal).toBool();
}

bool Preferences::useLogScaleRpmSliders(bool defaultVal) const
{
    return m_settings.value(key_useLogScaleRpmSliders, defaultVal).toBool();
}

QString Preferences::startupProfile(QString defaultVal) const
{;
    return m_settings.value(key_startupProfile, defaultVal).toString();
}

QString Preferences::shutdownProfile(QString defaultVal) const
{
    return m_settings.value(key_shutdownProfile, defaultVal).toString();
}

bool Preferences::quitOnClose(bool defaultVal) const
{
    return m_settings.value(key_quitOnClose, defaultVal).toBool();
}

QByteArray Preferences::windowGeometry(QByteArray defaultVal) const
{
    return m_settings.value(key_windowGeometry, defaultVal).toByteArray();
}

QByteArray Preferences::windowState(QByteArray defaultVal) const
{
    return m_settings.value(key_windowState, defaultVal).toByteArray();
}

QByteArray Preferences::windowProfileGeometry(QByteArray defaultVal) const
{
    return m_settings.value(key_windowProfileGeometry, defaultVal).toByteArray();
}

QByteArray Preferences::splitterProfileState(QByteArray defaultVal) const
{
    return m_settings.value(key_splitterProfileState, defaultVal).toByteArray();
}

QByteArray Preferences::windowSWAutoGeometry(QByteArray defaultVal) const
{
    return m_settings.value(key_windowSWAutoGeometry, defaultVal).toByteArray();
}

QByteArray Preferences::windowAboutGeometry(QByteArray defaultVal) const
{
    return m_settings.value(key_windowAboutGeometry, defaultVal).toByteArray();
}


QByteArray Preferences::windowPrefsGeometry(QByteArray defaultVal) const
{
    return m_settings.value(key_windowPrefsGeometry, defaultVal).toByteArray();
}


QByteArray Preferences::windowDiagGeometry(QByteArray defaultVal) const
{
    return m_settings.value(key_windowDiagGeometry, defaultVal).toByteArray();
}

QByteArray Preferences::windowHelpGeometry(QByteArray defaultVal) const
{
    return m_settings.value(key_windowHelpGeometry, defaultVal).toByteArray();
}

QString Preferences::applicationLanguage(QString defaultVal) const
{
    return m_settings.value(key_applicationLanguage, defaultVal).toString();
}

QString Preferences::channelName(unsigned channel, QString defaultVal) const
{
    QString keyName = channelNameKeyString(channel);
    QString name;

    name = m_settings.value(keyName, defaultVal).toString();

    if (name.isEmpty()) {
        name = "Channel " + QString::number(channel+1);
    }
    return name;
}

QString Preferences::probeName(unsigned channel, QString defaultVal) const
{
    QString keyName = probeNameKeyString(channel);
    QString name;

    name = m_settings.value(keyName, defaultVal).toString();

    if (name.isEmpty()) {
        name = "Probe " + QString::number(channel+1);
    }
    return name;
}

QString Preferences::stylesheet(void) const
{
    QString fn = m_settings.value(key_stylesheet,
                            Themes::getBuiltInStyleSheetName()
                            ).toString();
    if (fn.isEmpty())
        fn = Themes::getBuiltInStyleSheetName();

    return fn;
}

bool Preferences::showChannelLabels(bool defaultVal) const
{
    return m_settings.value(key_showChannelLabels, defaultVal).toBool();
}

/****************************************************************************
 Set functions
 ****************************************************************************/

void Preferences::setStartMinimized(bool istrue)
{
    m_settings.setValue(key_startMinimized, istrue);
}

void Preferences::setMinimizeToTray(bool istrue)
{
    m_settings.setValue(key_minimizeToTray, istrue);
}

void Preferences::setAlwaysShowTrayIcon(bool istrue)
{
    m_settings.setValue(key_alwaysShowTrayIcon, istrue);
}

void Preferences::setShowIconTooltips(bool istrue)
{
    m_settings.setValue(key_showTrayIconTT, istrue);
}

void Preferences::setUseLogScaleRpmSliders(bool istrue)
{
    m_settings.setValue(key_useLogScaleRpmSliders, istrue);
}

void Preferences::setStartupProfile(const QString& profileName)
{
    m_settings.setValue(key_startupProfile, profileName);
}

void Preferences::setShutdownProfile(const QString& profileName)
{
    m_settings.setValue(key_shutdownProfile, profileName);
}

void Preferences::setQuitOnClose(bool istrue)
{
    m_settings.setValue(key_quitOnClose, istrue);
}

void Preferences::setWindowGeometry(const QByteArray& windowGeometry)
{
    m_settings.setValue(key_windowGeometry, windowGeometry);
}

void Preferences::setWindowState(const QByteArray& windowState)
{
    m_settings.setValue(key_windowState, windowState);
}

void Preferences::setWindowProfileGeometry(const QByteArray& windowGeometry)
{
    m_settings.setValue(key_windowProfileGeometry, windowGeometry);
}

void Preferences::setSplitterProfileState(const QByteArray& splitterState)
{
    m_settings.setValue(key_splitterProfileState, splitterState);
}

void Preferences::setWindowSWAutoGeometry(const QByteArray& windowGeometry)
{
    m_settings.setValue(key_windowSWAutoGeometry, windowGeometry);
}

void Preferences::setWindowAboutGeometry(const QByteArray& windowGeometry)
{
    m_settings.setValue(key_windowAboutGeometry, windowGeometry);
}

void Preferences::setWindowPrefsGeometry(const QByteArray& windowGeometry)
{
    m_settings.setValue(key_windowPrefsGeometry, windowGeometry);
}

void Preferences::setWindowDiagGeometry(const QByteArray& windowGeometry)
{
    m_settings.setValue(key_windowDiagGeometry, windowGeometry);
}

void Preferences::setWindowHelpGeometry(const QByteArray& windowGeometry)
{
    m_settings.setValue(key_windowHelpGeometry, windowGeometry);
}

void Preferences::setApplicationLanguage(const QString& language)
{
    Languages lang;
    QString languageFile = lang.convertLanguageToFile(language);
    m_settings.setValue(key_applicationLanguage, languageFile);
}

void Preferences::setChannelName(unsigned channel, const QString& name)
{
    QString keyName = channelNameKeyString(channel);

    m_settings.setValue(keyName, name);
}

void Preferences::setProbeName(unsigned channel, const QString& name)
{
    QString keyName = probeNameKeyString(channel);

    m_settings.setValue(keyName, name);
}

void Preferences::setStylesheet(const QString& filenameAndPath)
{
    m_settings.setValue(key_stylesheet, filenameAndPath);
}

void Preferences::setShowChannelLabels(bool istrue)
{
    m_settings.setValue(key_showChannelLabels, istrue);
}

/****************************************************************************
 Private
 ****************************************************************************/

QString Preferences::channelNameKeyString(int channel) const
{
    return QString(keyBase_channelName) + QString("_") + QString::number(channel);
}

QString Preferences::probeNameKeyString(int channel) const
{
    return QString(keyBase_probeName) + QString("_") + QString::number(channel);
}
