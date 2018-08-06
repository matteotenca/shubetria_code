#include "gui_setmanualrpm.h"
#include "ui_gui_setmanualrpm.h"

#include "bfx-recon/bfxrecon.h"

gui_setManualRpm::gui_setManualRpm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gui_setManualRpm),
    m_accepted(false)
{
    ui->setupUi(this);
}

gui_setManualRpm::~gui_setManualRpm()
{
    delete ui;
}

bool gui_setManualRpm::useMaxRpm(void) const
{
    return ui->ctrl_useMaxRpm->isChecked();
}

int gui_setManualRpm::getUserValue(void) const
{
    return ui->ctrl_manualRpm->value();
}

void gui_setManualRpm::setChannelLabel(int channel)
{
    ui->ctrl_title->setText(tr("Set manual speed for channel %1")
                                .arg(channel+1));
}

void gui_setManualRpm::setCurrentRpm(int rpm)
{
    if (rpm == RECON_MAXRPM)
    {
        ui->ctrl_useMaxRpm->setChecked(true);
        ui->ctrl_manualRpm->setEnabled(false);
    }
    else
    {
        ui->ctrl_manualRpm->setValue(rpm);
        ui->ctrl_manualRpm->setEnabled(true);
    }

}

void gui_setManualRpm::on_buttonBox_accepted()
{
    m_accepted = true;
    accept();
}

void gui_setManualRpm::on_buttonBox_rejected()
{
    //reject();
}

void gui_setManualRpm::on_ctrl_useMaxRpm_toggled(bool checked)
{
    ui->ctrl_manualRpm->setEnabled(!checked);
}
