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

#include "gui_profiles.h"
#include "ui_gui_profiles.h"
#include "phoebetriaapp.h"

#include <QDebug>
#include <QMessageBox>
#include <QWidget>

#include "fanprofiles.h"

gui_Profiles::gui_Profiles(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gui_Profiles),
    m_action(0)
{
    ui->setupUi(this);
    restoreGeometry(ph_prefs().windowProfileGeometry());
    ui->splitter->restoreState(ph_prefs().splitterProfileState());

    init();
}

gui_Profiles::~gui_Profiles()
{
    delete ui;
}

bool gui_Profiles::init(void)
{
    getProfileList();
    return true;
}

bool gui_Profiles::getProfileList(void)
{
    FanControllerProfile fcp;
    ui->ctrl_profileList->clear();

    QStringList m_ProfileList = fcp.getProfileNames();

    bool bs = ui->ctrl_profileList->blockSignals(true);

    for (int i = 0; i < m_ProfileList.count(); ++i)
    {
        const QString& item = m_ProfileList.at(i);
        // Skip reserved profile names
        if (FanControllerProfile::isReservedProfileName(item)) continue;
        ui->ctrl_profileList->addItem(item);
    }

    ui->ctrl_profileList->blockSignals(bs);

    return true;
}

void gui_Profiles::on_ctrl_profileList_itemClicked()
{
    FanControllerProfile fcp;

    m_profileName = ui->ctrl_profileList->currentItem()->text();
    m_profileDescription = fcp.profileDescription(m_profileName);

    ui->ctrl_profileName->setText(m_profileName);
    ui->ctrl_profileDescription->setPlainText(m_profileDescription);

    fcp.load(m_profileName);

    QFont font = ui->ctrl_profilePreview->font();
    font.setPointSize(9);

    ui->ctrl_profilePreview->setFont(font);
    ui->ctrl_profilePreview->setHtml(fcp.htmlReport());
}

void gui_Profiles::on_ctrl_EraseProfile_clicked()
{
    FanControllerProfile fcp;

    if (FanControllerProfile::isReservedProfileName(m_profileName))
    {
        QMessageBox::critical(
                    this,
                    tr("Invalid profile name"),
                    tr("Profile names beginning with %1"
                       " are reserved.\nThe profile has not been erased!")
                    .arg(FanControllerProfile::reservedProfileNameStartChars())
                    );

        return;
    }

    if (!fcp.getProfileNames().contains(m_profileName))
    {
        return; // Nothing to delete; i.e. profile does not exist
    }

    QMessageBox msgbox;
    QString text;
    bool changeStartupProfile = false;
    bool changeShutdownProfile = false;

    if (m_profileName == ph_prefs().startupProfile())
    {
        text = QString ("You have selected to delete profile: "
                "%1\n\n"
                "This is currently set as your startup profile.\n"
                "Your preferences will be modified appropriately if you continue.\n\n"
                "Would you like to continue?\n").arg(m_profileName);
        changeStartupProfile = true;
    }
    else if (m_profileName == ph_prefs().shutdownProfile())
    {
        text = QString ("You have selected to delete profile:"
                "%1\n\n"
                "This is currently set as your shutdown profile. "
                "Your preferences will be modified appropriately if you continue.\n\n"
                "Would you like to continue?\n").arg(m_profileName);
        changeShutdownProfile = true;
    }
    else
    {
        text = QString("You have selected to delete profile: "
                           "%1\n\n"
                           "Would you like to continue?\n").arg(m_profileName);
    }
    msgbox.setWindowTitle("Erase Profile");
    msgbox.setText(text);
    msgbox.addButton(QMessageBox::Ok);
    msgbox.addButton(QMessageBox::Cancel);

    int status = msgbox.exec();
    switch (status) {
        case QMessageBox::Ok:
            if (fcp.erase(m_profileName))
            {
                getProfileList();
                ui->ctrl_profileName->clear();
                ui->ctrl_profileDescription->clear();
                m_profileName.clear();
                m_profileDescription.clear();
                m_action |= RefreshProfileDisplay;

                if (changeStartupProfile)
                    ph_prefs().setStartupProfile("");
                if (changeShutdownProfile)
                    ph_prefs().setShutdownProfile("");

                return;
            }
            else
            {
                QMessageBox::critical(
                            this,
                            tr("Erase failed"),
                            tr("An error occurred deleting the profile.\n"
                               "The profile has NOT been deleted!")
                            );
            }
            break;
        case QMessageBox::Cancel:
            break;
     }

}

void gui_Profiles::on_ctrl_LoadProfile_clicked()
{
    FanControllerProfile fcp;

    if (!fcp.getProfileNames().contains(m_profileName))
    {
        QMessageBox::critical(
                    this,
                    tr("Profile not found!"),
                    tr("Profile named: %1 was not found!"
                       "\n\nPlease verify the name and try again.")
                    .arg(ui->ctrl_profileName->text())
                    );
        return;
    }

    m_action = LoadProfile;
    this->accept();
}

void gui_Profiles::on_ctrl_SaveProfile_clicked()
{

    m_profileName = ui->ctrl_profileName->text().trimmed();
    m_profileDescription = ui->ctrl_profileDescription->toPlainText();

    if (m_profileName.trimmed().isEmpty())
    {
        QMessageBox::critical(
                    this,
                    tr("Invalid profile name"),
                    tr("The profile name you have entered is\n"
                       " blank, please enter a valid name!")
                    );
        return;
    }

    // TODO FIXME Check that the profile name is not reserved

    m_action = SaveProfile;
    this->accept();
}

void gui_Profiles::on_ctrl_profileClose_clicked()
{
    this->close();
}

void gui_Profiles::done(int result)
{
    ph_prefs().setWindowProfileGeometry(saveGeometry());
    ph_prefs().setSplitterProfileState(ui->splitter->saveState());
    QDialog::done(result);
}
