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

#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QLayout>
#include <QSystemTrayIcon>
#include "fancontrollerdata.h"
#include <QDebug>
#include <QTranslator>
#include <QMenu>



class SliderOverlay : public QSlider
{
    Q_OBJECT
public:
     SliderOverlay(QSlider *parent = 0);
};


class LabelOverlay : public QLabel
{
    Q_OBJECT
public:
     LabelOverlay(QLabel *parent = 0);
};

class ChannelLabel : public QLabel
{
    Q_OBJECT
public:
     ChannelLabel(QLabel *parent = 0);
};

namespace Ui
{
class gui_MainWindow;
}

class gui_MainWindow : public QMainWindow
{
    Q_OBJECT

    static const double toLogScale;
    static const double toLinearScale;

public:
    explicit gui_MainWindow(QWidget *parent = 0);
    ~gui_MainWindow();

    QTranslator translator;
    void setSoftwareAutoOn(bool yes = true);

public slots:

    void syncGuiCtrlsWithFanController(void);

    void changeEvent(QEvent* e);
    void closeEvent(QCloseEvent *e);
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

    // **** Slots for common settings
    void onControlModeChanged(bool isAuto);
    void onIsAudibleAlarmChanged(bool isAudibleAlarm);
    void onMaxRPM(int channel, int RPM);

    // **** Channel related slots
    void onCurrentRPM(int channel, int RPM);
    void onManualRPMChanged(int channel, int RPM);
    void onCurrentTemp(int channel, int tempInF);
    void onCurrentAlarmTemp(int channel, int tempInF);
    void onTemperatureScaleChanged(bool isCelcius);

    // **** Logging related slots
    void onMinLoggedRpmChanged(int channel, int rpm);
    void onMaxLoggedRpmChanged(int channel, int rpm);
    void onMinLoggedTempChanged (int channel, int temperature);
    void onMaxLoggedTempChanged (int channel, int temperature);

private slots:

    void checkForReqChannelParems(void);

    void on_ctrl_channel1speedSlider_sliderPressed();
    void on_ctrl_channel1speedSlider_sliderReleased();
    void on_ctrl_channel1speedSlider_valueChanged(int value);

    void on_ctrl_channel2speedSlider_sliderPressed();
    void on_ctrl_channel2speedSlider_sliderReleased();
    void on_ctrl_channel2speedSlider_valueChanged(int value);

    void on_ctrl_channel3speedSlider_sliderPressed();
    void on_ctrl_channel3speedSlider_sliderReleased();
    void on_ctrl_channel3speedSlider_valueChanged(int value);

    void on_ctrl_channel4speedSlider_sliderPressed();
    void on_ctrl_channel4speedSlider_sliderReleased();
    void on_ctrl_channel4speedSlider_valueChanged(int value);

    void on_ctrl_channel5speedSlider_sliderPressed();
    void on_ctrl_channel5speedSlider_sliderReleased();
    void on_ctrl_channel5speedSlider_valueChanged(int value);

    void on_ctrl_channel1AlarmTemp_clicked();
    void on_ctrl_channel2AlarmTemp_clicked();
    void on_ctrl_channel3AlarmTemp_clicked();
    void on_ctrl_channel4AlarmTemp_clicked();
    void on_ctrl_channel5AlarmTemp_clicked();

    void on_ctrl_ModifyProfile_clicked();

    void on_ctrl_tempScaleToggleBtn_toggled(bool checked);

    void on_ctrl_isManualBtn_toggled(bool checked);

    void on_ctrl_isAudibleAlarmBtn_toggled(bool checked);

    void on_ctrl_configSoftwareAutoBtn_clicked();

    void on_ctrl_isSoftwareControlBtn_toggled(bool checked);

    void askUserForManualSpeed(int channel);

    void on_ctrl_channel1Select_clicked();

    void on_ctrl_channel2Select_clicked();

    void on_ctrl_channel3Select_clicked();

    void on_ctrl_channel4Select_clicked();

    void on_ctrl_channel5Select_clicked();

    void on_ctrl_channel1speed_clicked();

    void on_ctrl_channel2speed_clicked();

    void on_ctrl_channel3speed_clicked();

    void on_ctrl_channel4speed_clicked();

    void on_ctrl_channel5speed_clicked();

    void on_pushButton_clicked();

    void on_ctrl_syncGui_clicked();

    // LOAD PROFILE MENU ACTIONS
    void initLoadProfileActions(void);

    // FILE MENU
    void when_actionPreferences_selected();
    void when_actionQuit_selected();

    // HELP MENU
    void when_actionHelp_selected();
    void when_actionDiagnosticReport_selected();
    void when_actionAbout_selected();

    // TRAY CONTEXT MENU
    void trayIconMenu_Quit_Selected();
    void loadProfile_MenuItem_Selected();

    void on_pushButton_2_clicked();

private:

    void initCtrlArrays(void);

    void initWaitForReqChannelParams(void);

    void initMenus(void);

    void initTrayIconMenu(void);
    void updateLoadProfileActions(void);

    bool customAutoAvailable(void) const;

    FanControllerData& fcdata(void) const;

    void connectCustomSignals(void);

    void enableSpeedControls(bool enabled = true);

    void updateSpeedControlTooltip(int channel);
    void updateSpeedControlTooltips(void);

    void updateChannelControlTooltip(int channel);
    void updateChannelControlTooltips(void);

    int maxRPM(int channel) const;
    void updateSpeedControl(int channel, int RPM, bool updateSlider = true);
    void updateAllSpeedCtrls(bool useManualRpm = false);

    void updateTargetRpmOverlay(int channel);

    void updateProbeAffinityOverlays(void);

    void updateToggleControls(void);

    void updateCurrentTempControl(int channel, int temp);
    void updateAllTemperatureControls(void);
    void updateAlarmTempControl(int channel, int temp, bool isCelcius);
    void updateAllAlarmCtrls(bool isCelcius);
    void updateTrayIconTooltip(void);

    int rpmSliderValueToRPM(int channel, int value) const;

    int valueToLogScale(int linearValue) const
    {
        return ceil(log((double)linearValue+1) / toLogScale);
    }

    int valueToLinearScale(int linearValue) const
    {
        return floor(exp(toLinearScale*(linearValue)) - 1);
    }

    void userPressedChannelRpmSlider(int channel);
    void userReleasedChannelRpmSlider(int channel);
    void userChangedChannelRpmSlider(int channel, int value);

    void userClickedAlarmTempCtrl(int channel);

    bool loadProfile(const QString& profileName);
    bool saveProfile(const QString& profileName, const QString& description);

    void updateProfileDisplay(const QString& profileName,
                              const QString& description);

    void initTargetRpmOverlays();

    void initProbeAffinityOverlays();

    Ui::gui_MainWindow *ui;

    // Convenience pointers to controls that belong to ui->
    QLineEdit* m_ctrls_probeTemps[FC_MAX_CHANNELS];
    QPushButton* m_ctrls_currentRPM[FC_MAX_CHANNELS];
    QSlider* m_ctrls_RpmSliders[FC_MAX_CHANNELS];
    QPushButton* m_ctrls_alarmTemps[FC_MAX_CHANNELS];
    QPushButton* m_ctrls_channel[FC_MAX_CHANNELS];

    LabelOverlay* m_ctrls_probeAffinityOverlay[FC_MAX_CHANNELS];
    QHBoxLayout* m_layout_probeAffinityOverlay[FC_MAX_CHANNELS];

    // Channel Labels (Temp and Rpm)
    void addChannelLabels();
    void updateChannelLabels();
    QVBoxLayout* m_layout_probeTemps[FC_MAX_CHANNELS];
    ChannelLabel* m_lbl_probeTemps[FC_MAX_CHANNELS];
    QVBoxLayout* m_layout_channel[FC_MAX_CHANNELS];
    ChannelLabel* m_lbl_channel[FC_MAX_CHANNELS];

    // These are initialised at run time by this class
    SliderOverlay* m_ctrls_targetRpmOverlay[FC_MAX_CHANNELS];
    QGridLayout* m_layout_targetRpmOverlay[FC_MAX_CHANNELS];

    QSystemTrayIcon m_trayIcon;

    QMenu m_trayIconMenu;

    enum { MaxTrayMenuProfiles = 10 };
    QAction* m_LoadProfileActions[MaxTrayMenuProfiles];

    bool m_reqChannelParamsAreSet;

    bool m_channelLabelsCreated;

};

#endif // GUI_MAINWINDOW_H
