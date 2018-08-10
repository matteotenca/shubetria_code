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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QObject>
#include <QVariant>
#include <QSettings>
#include <QString>


/** TODO

    PREFERENCES FOR V1.3

    tooltips on minimize[y/n]       ??
    show tray icon tooltips[y/n]    ??

    */

#if defined Q_WS_MAC
#   define DEFAULT_CLOSEBEHAVIOUR false
#else
#   define DEFAULT_CLOSEBEHAVIOUR true
#endif


class Preferences : public QObject
{
    Q_OBJECT

public:

    explicit Preferences(QObject *parent = 0);

    static QString filepath(void);

    bool    startMinimized(bool defaultVal = false) const;
    bool    minimizeToTray(bool defaultVal = true) const;
    bool    alwaysShowTrayIcon(bool defaultVal = false) const;
    bool    showTrayIconTooltips(bool defaultVal = true) const;
    bool    useLogScaleRpmSliders(bool defaultVal = true) const;
    QString startupProfile(QString defaultVal = "") const;
    QString shutdownProfile(QString defaultVal = "") const;
    bool    quitOnClose(bool defaultVal = DEFAULT_CLOSEBEHAVIOUR) const;
    QByteArray windowGeometry(QByteArray defaultVal = "") const;
    QByteArray windowState(QByteArray defaultVal = "") const;
    QByteArray windowProfileGeometry(QByteArray defaultVal = "") const;
    QByteArray splitterProfileState(QByteArray defaultVal = "") const;
    QByteArray windowSWAutoGeometry(QByteArray defaultVal = "") const;
    QByteArray windowAboutGeometry(QByteArray defaultVal = "") const;
    QByteArray windowPrefsGeometry(QByteArray defaultVal = "") const;
    QByteArray windowDiagGeometry(QByteArray defaultVal = "") const;
    QByteArray windowHelpGeometry(QByteArray defaultVal = "") const;
//    QString applicationLanguage(QString defaultVal = "") const;
    QString channelName(unsigned channel, QString defaultVal = "") const;
    QString probeName(unsigned channel, QString defaultVal = "") const;
    QString stylesheet(void) const;
    bool    showChannelLabels(bool defaultVal = true) const;

    void setStartMinimized(bool istrue);
    void setMinimizeToTray(bool istrue);
    void setAlwaysShowTrayIcon(bool istrue);
    void setShowIconTooltips(bool istrue);
    void setUseLogScaleRpmSliders(bool istrue);
    void setQuitOnClose(bool istrue);
    void setWindowGeometry(const QByteArray& windowGeometry);
    void setWindowState(const QByteArray& windowState);
    void setWindowProfileGeometry(const QByteArray& windowGeometry);
    void setSplitterProfileState(const QByteArray& splitterState);
    void setWindowSWAutoGeometry(const QByteArray& windowGeometry);
    void setWindowAboutGeometry(const QByteArray& windowGeometry);
    void setWindowPrefsGeometry(const QByteArray& windowGeometry);
    void setWindowDiagGeometry(const QByteArray& windowGeometry);
    void setWindowHelpGeometry(const QByteArray& windowGeometry);
//    void setApplicationLanguage(const QString& language);
    void setStartupProfile(const QString& profileName);
    void setShutdownProfile(const QString& profileName);
    void setChannelName(unsigned channel, const QString& name);
    void setProbeName(unsigned channel, const QString& name);
    void setStylesheet(const QString& filenameAndPath);
    void setShowChannelLabels(bool istrue);

    inline void sync(void);

private:

    QString channelNameKeyString(int channel) const;
    QString probeNameKeyString(int channel) const;

    QSettings m_settings;

signals:
    
public slots:
    
};

// Force settings to be written to disk
void Preferences::sync(void)
{
    m_settings.sync();
}

#undef DEFAULT_CLOSEBEHAVIOUR

#endif // PREFERENCES_H
