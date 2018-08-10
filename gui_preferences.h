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

#ifndef GUI_PREFERENCES_H
#define GUI_PREFERENCES_H

#include <QDialog>

namespace Ui
{
class gui_Preferences;
}

class gui_Preferences : public QDialog
{
    Q_OBJECT
    
public:
    explicit gui_Preferences(QWidget *parent = 0);
    ~gui_Preferences();
    
protected:

    void initControls(void);

    void commitChanges(void) const;

public slots:

private slots:
    void on_ctrl_actionButtons_accepted();

    void on_ctrl_preferenceTabList_itemSelectionChanged();

private:
    Ui::gui_Preferences *ui;

    void populateProfileComboBoxes(void);
    void populateThemesComboBox(void);
//    void populateLanguageComboBox(void);
    void done(int result);
};

#endif // GUI_PREFERENCES_H
