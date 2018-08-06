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

#include "gui_preferences.h"
#include "ui_gui_preferences.h"

#include <QDebug>
#include <QStringList>
#include <QDirIterator>

#include "shubetriaapp.h"
#include "themes.h"
#include "languages.h"

gui_Preferences::gui_Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gui_Preferences)
{
    ui->setupUi(this);
    restoreGeometry(ph_prefs().windowPrefsGeometry());
    ui->ctrl_preferenceTabList->setCurrentRow(0);
    populateProfileComboBoxes();
    populateThemesComboBox();
    populateLanguageComboBox();
    initControls();

    // FIXME: Hiding controls for non-working preferences

    ui->lbl_ShowTooltipOnMinimize->hide();
    ui->ctrl_showTooltipOnMinimize->hide();
}

gui_Preferences::~gui_Preferences()
{
    delete ui;
}

void gui_Preferences::initControls(void)
{
    Languages lang;
    ui->ctrl_minimizeOnStart->setChecked(ph_prefs().startMinimized());
    ui->ctrl_minimizeToTray->setChecked(ph_prefs().minimizeToTray());
    ui->ctrl_alwaysShowTrayIcon->setChecked(ph_prefs().alwaysShowTrayIcon());
    ui->ctrl_showTooltipOnMinimize->setChecked(ph_prefs().showTrayIconTooltips());
    ui->ctrl_useLogRpmScale->setChecked(ph_prefs().useLogScaleRpmSliders());
    ui->ctrl_quitOnClose->setChecked(ph_prefs().quitOnClose());

    QString profileName;

    profileName = ph_prefs().startupProfile();
    if (!profileName.isEmpty())
        ui->ctrl_startupProfile->setCurrentIndex(ui->ctrl_startupProfile->findText(profileName));

    profileName = ph_prefs().shutdownProfile();
    if (!profileName.isEmpty())
        ui->ctrl_shutdownProfile->setCurrentIndex(ui->ctrl_shutdownProfile->findText(profileName));

    QString languageName;

    languageName = lang.convertFileToLanguage(ph_prefs().applicationLanguage());
    if (!languageName.isEmpty())
        ui->ctrl_language->setCurrentIndex(ui->ctrl_language->findText(languageName));

    ui->ctrl_channel1FanName->setText(ph_prefs().channelName(0));
    ui->ctrl_channel2FanName->setText(ph_prefs().channelName(1));
    ui->ctrl_channel3FanName->setText(ph_prefs().channelName(2));
    ui->ctrl_channel4FanName->setText(ph_prefs().channelName(3));
    ui->ctrl_channel5FanName->setText(ph_prefs().channelName(4));

    ui->ctrl_channel1TempName->setText(ph_prefs().probeName(0));
    ui->ctrl_channel2TempName->setText(ph_prefs().probeName(1));
    ui->ctrl_channel3TempName->setText(ph_prefs().probeName(2));
    ui->ctrl_channel4TempName->setText(ph_prefs().probeName(3));
    ui->ctrl_channel5TempName->setText(ph_prefs().probeName(4));

    ui->ctrl_showChannelLabels->setChecked(ph_prefs().showChannelLabels());
}

void gui_Preferences::populateProfileComboBoxes(void)
{
    FanControllerProfile fcp;

    ui->ctrl_startupProfile->addItem("");
    ui->ctrl_shutdownProfile->addItem("");

    NameAndControlModeList profileList = fcp.getProfileNamesAndModes();

    for (int i = 0; i < profileList.count(); ++i)
    {
        const NameAndControlMode& item = profileList.at(i);
        // Skip reserved profile names
        if (FanControllerProfile::isReservedProfileName(item.name))
            continue;

        ui->ctrl_startupProfile->addItem(item.name);

        if (item.controlMode != FanControllerMode_Unknown
                && item.controlMode != FanControllerMode_SoftwareAuto)
        {
            ui->ctrl_shutdownProfile->addItem(item.name);
        }
    }
}


void gui_Preferences::populateThemesComboBox(void)
{
    ThemeNameAndFilenameList items;

    Themes::getCustomThemeList(&items);

    // Add the built in stylesheets
    ui->ctrl_style->addItem("Shubetria (Standard)",
                            Themes::getBuiltInStyleSheetName(Shubetria_Stylesheet_Standard));

    ui->ctrl_style->addItem("Shubetria (Dark)",
                            Themes::getBuiltInStyleSheetName(Shubetria_Stylesheet_Dark));

    int c = items.count();
    for (int i = 0; i < c; ++i)
    {
        ThemeNameAndFilename td = items.at(i);
        ui->ctrl_style->addItem(td.name, td.fileNameAndPath);
    }

    QString currThemeFilename = ph_shubetriaApp()->getCurrentThemeFilename();

    if (!currThemeFilename.isEmpty())
    {
        int index = ui->ctrl_style->findData(currThemeFilename);
        if (index != -1)
        {
            ui->ctrl_style->setCurrentIndex(index);
        }
    }
}

void gui_Preferences::populateLanguageComboBox(void)
{
    ui->ctrl_language->clear();

    Languages lang;
    QString language;
    QStringList languages = lang.getSupportedLanguagesList();

    foreach(language, languages)
    {
        QString icon=QString(":/language/%1.png").arg(language);
        ui->ctrl_language->addItem(QIcon(icon), language);
    }
}



void gui_Preferences::commitChanges(void) const
{
    ph_prefs().setStartMinimized(ui->ctrl_minimizeOnStart->isChecked());
    ph_prefs().setMinimizeToTray(ui->ctrl_minimizeToTray->isChecked());
    ph_prefs().setAlwaysShowTrayIcon(ui->ctrl_alwaysShowTrayIcon->isChecked());
    ph_prefs().setShowIconTooltips(ui->ctrl_showTooltipOnMinimize->isChecked());
    ph_prefs().setUseLogScaleRpmSliders(ui->ctrl_useLogRpmScale->isChecked());
    ph_prefs().setQuitOnClose(ui->ctrl_quitOnClose->isChecked());

    QString txt;

    if(ui->ctrl_startupProfile->currentIndex() < 1)
        txt = "";
    else
        txt = ui->ctrl_startupProfile->currentText();
    ph_prefs().setStartupProfile(txt);

    if(ui->ctrl_shutdownProfile->currentIndex() < 1)
        txt = "";
    else
        txt = ui->ctrl_shutdownProfile->currentText();
    ph_prefs().setShutdownProfile(txt);
    ph_prefs().setApplicationLanguage(ui->ctrl_language->currentText());

    ph_prefs().setChannelName(0, ui->ctrl_channel1FanName->text());
    ph_prefs().setChannelName(1, ui->ctrl_channel2FanName->text());
    ph_prefs().setChannelName(2, ui->ctrl_channel3FanName->text());
    ph_prefs().setChannelName(3, ui->ctrl_channel4FanName->text());
    ph_prefs().setChannelName(4, ui->ctrl_channel5FanName->text());

    ph_prefs().setProbeName(0, ui->ctrl_channel1TempName->text());
    ph_prefs().setProbeName(1, ui->ctrl_channel2TempName->text());
    ph_prefs().setProbeName(2, ui->ctrl_channel3TempName->text());
    ph_prefs().setProbeName(3, ui->ctrl_channel4TempName->text());
    ph_prefs().setProbeName(4, ui->ctrl_channel5TempName->text());

    int idx = ui->ctrl_style->currentIndex();
    if (idx >= 0)
    {
        QString themeFilename =  ui->ctrl_style->itemData(idx).toString();
        ph_prefs().setStylesheet(themeFilename);
        ph_shubetriaApp()->setTheme(themeFilename);
    }

    ph_prefs().setShowChannelLabels(ui->ctrl_showChannelLabels->isChecked());

    ph_prefs().sync();
}

void gui_Preferences::on_ctrl_actionButtons_accepted()
{
    commitChanges();
}

void gui_Preferences::on_ctrl_preferenceTabList_itemSelectionChanged()
{
    ui->ctrl_preferenceTabs->setCurrentIndex(ui->ctrl_preferenceTabList->currentRow());
}

void gui_Preferences::done(int result)
{
    ph_prefs().setWindowPrefsGeometry(saveGeometry());
    QDialog::done(result);
}
