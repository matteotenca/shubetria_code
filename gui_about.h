#ifndef GUI_ABOUT_H
#define GUI_ABOUT_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui
{
class gui_About;
}

class gui_About : public QDialog
{
    Q_OBJECT

public:
    explicit gui_About(QWidget *parent = 0);
    ~gui_About();

private slots:

    void on_ctrl_aboutClose_clicked();

private:
    Ui::gui_About *ui;

    void done(int result);
};

#endif // GUI_ABOUT_H
