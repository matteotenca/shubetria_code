#ifndef GUI_SETMANUALRPM_H
#define GUI_SETMANUALRPM_H

#include <QDialog>

namespace Ui {
class gui_setManualRpm;
}

class gui_setManualRpm : public QDialog
{
    Q_OBJECT
    
public:
    explicit gui_setManualRpm(QWidget *parent = 0);
    ~gui_setManualRpm();
    
    inline bool accepted(void) const;

    bool useMaxRpm(void) const;

    int getUserValue(void) const;

    void setChannelLabel(int channel);

    void setCurrentRpm(int rpm);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_ctrl_useMaxRpm_toggled(bool checked);

private:
    Ui::gui_setManualRpm *ui;

    bool m_accepted;
};


bool gui_setManualRpm::accepted(void) const
{
    return m_accepted;
}


#endif // GUI_SETMANUALRPM_H
