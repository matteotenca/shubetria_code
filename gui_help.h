#ifndef GUI_HELP_H
#define GUI_HELP_H

#include <QDialog>
#include <QTreeWidget>

namespace Ui {
class gui_Help;
}

class gui_Help : public QDialog
{
    Q_OBJECT
    
public:
    explicit gui_Help(QWidget *parent = 0);
    ~gui_Help();
    
private slots:
    // (c) 2018 Shub
    void on_ctrl_helpList_currentItemChanged(QTreeWidgetItem *current);
    // void on_ctrl_helpList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_ctrl_close_clicked();

private:
    Ui::gui_Help *ui;
};

#endif // GUI_HELP_H
