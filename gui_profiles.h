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

#ifndef GUI_PROFILES_H
#define GUI_PROFILES_H

#include <QDialog>
#include "fancontrollerdata.h"

namespace Ui {
class gui_Profiles;
}

class gui_Profiles : public QDialog
{
    Q_OBJECT

public:

    typedef enum
    {
        LoadProfile             = 1U << 0,
        SaveProfile             = 1U << 1,
        RefreshProfileDisplay   = 1U << 2
    } Action;

    // Constructors/Destructors
    explicit gui_Profiles(QWidget *parent = 0);
    ~gui_Profiles();

    bool init();

    bool saveProfile(void);
    bool eraseProfile(void);

    inline const QString& selectedName(void) const;
    inline const QString& selectedDescription(void) const;

    inline unsigned action(void) const;
    
private slots:
    void on_ctrl_profileList_itemClicked();

    void on_ctrl_SaveProfile_clicked();

    void on_ctrl_EraseProfile_clicked();

    void on_ctrl_LoadProfile_clicked();

    void on_ctrl_profileClose_clicked();

private:
    Ui::gui_Profiles *ui;

    bool getProfileList(void);

    QStringList m_ProfileList;
    QString m_profileDescription;
    QString m_profileName;

    unsigned m_action;

    void done(int result);
};

const QString& gui_Profiles::selectedName(void) const
{
    return m_profileName;
}

const QString& gui_Profiles::selectedDescription(void) const
{
    return m_profileDescription;
}

inline unsigned gui_Profiles::action(void) const
{
    return m_action;
}

#endif // GUI_PROFILES_H
