#ifndef GUI_DIAGNOSTIC_H
#define GUI_DIAGNOSTIC_H

#include <QDialog>
#include <QTextBrowser>

namespace Ui {
class gui_Diagnostic;
}

class gui_Diagnostic : public QDialog
{
    Q_OBJECT
    
public:
    explicit gui_Diagnostic(QWidget *parent = 0);
    ~gui_Diagnostic();
    
private slots:
    void done(int result);
    void on_ctrl_saveAsPDF_clicked();

    void on_ctrl_close_clicked();

private:
    Ui::gui_Diagnostic *ui;

    void initControls();

    QString m_diagnosticReport;
    QFont m_font;

};

#endif // GUI_DIAGNOSTIC_H
