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

#ifndef GUI_SOFTWAREAUTOSETUP_H
#define GUI_SOFTWAREAUTOSETUP_H

#include <QDialog>

#include "fanramp.h"
#include "bfx-recon/bfxrecon.h"

// Fwd decls
class FanControllerData;

namespace Ui {
class gui_SoftwareAutoSetup;
}

class gui_SoftwareAutoSetup : public QDialog
{
    Q_OBJECT
    
public:
    explicit gui_SoftwareAutoSetup(QWidget *parent = 0);
    ~gui_SoftwareAutoSetup();
    
    void init(FanControllerData* fcdata);

    inline bool userAccepted(void) const;

protected:

    void setupAxes(const FanControllerData & fcdata, int channel);
    void setupRpmAxis_currentRamp(void);
    void setupTemperatureCtrlLimits(const FanControllerData & fcdata);
    void setupSpeedCtrlLimits(int maxRpm);
    void setupChannelComboBox(void);

    void xferSettings_toGui(const FanControllerData &fcdata, int channel);

    void drawPlot(void);

    void regenerateCurve(void);

    int tempInF(int t) const;

    inline bool ignoreSignals(bool ignore = true);

private slots:

    void on_ctrl_rampStartTemp_valueChanged(int arg1);

    void on_ctrl_rampMidTemp_valueChanged(int arg1);

    void on_ctrl_rampEndTemp_valueChanged(int arg1);

    void on_fan_fanToMaxTemp_valueChanged(int arg1);

    void on_ctrl_rampStartSpeed_valueChanged(int arg1);

    void on_ctrl_rampMidSpeed_valueChanged(int arg1);

    void on_ctrl_rampEndSpeed_valueChanged(int arg1);

    void on_ctrl_minRpm_valueChanged(int arg1);

    void on_ctrl_channel_currentIndexChanged(int index);

    void on_ctrl_probeAffinity_valueChanged(int arg1);

    void on_buttonBox_accepted();

    void on_ctrl_isFanAlwaysOn_clicked(bool checked);

    void on_ctrl_fanOnTemp_valueChanged(int arg1);

    void on_ctrl_fanOnSpeed_valueChanged(int arg1);

    void on_ctrl_hysteresisUp_editingFinished();

    void on_ctrl_hysteresisDown_editingFinished();

    void on_ctrl_hysteresisFanOff_editingFinished();

private:
    Ui::gui_SoftwareAutoSetup *ui;

    bool m_isCelcius;

    int m_currChannel;

    bool m_ignoreSignals;

    FanSpeedRamp m_ramp[FC_MAX_CHANNELS];

    FanControllerData* m_fcdata;

    bool m_wasAccepted;

    void done(int result);
};

bool gui_SoftwareAutoSetup::ignoreSignals(bool ignore)
{
    bool r = m_ignoreSignals;
    m_ignoreSignals = ignore;
    return r;
}

bool gui_SoftwareAutoSetup::userAccepted(void) const
{
    return m_wasAccepted;
}


#endif // GUI_SOFTWAREAUTOSETUP_H
