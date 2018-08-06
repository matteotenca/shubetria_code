#include "gui_diagnostic.h"
#include "ui_gui_diagnostic.h"
#include "phoebetriaapp.h"
#include "appinfo.h"
#include "QPrinter"
#include "QDir"
#include "QFileDialog"

gui_Diagnostic::gui_Diagnostic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gui_Diagnostic)
{
    ui->setupUi(this);
    restoreGeometry(ph_prefs().windowDiagGeometry());
    m_font.setPointSize(8);

    initControls();
    // (c) 2018 Shub
    connect(ui->ctrl_saveAsPDF,SIGNAL(clicked()), this, SLOT(on_ctrl_saveAsPDF_clicked()));
    // connect(ui->ctrl_saveAsPDF, SIGNAL(clicked), this, SLOT(on_ctrl_saveAsPDF_clicked(m_diagnosticReport)));

}

void gui_Diagnostic::initControls()
{
    m_diagnosticReport = AppInfo::basicInfoReport();

    ui->ctrl_diagnosticReport->setFont(m_font);
    ui->ctrl_diagnosticReport->setText(m_diagnosticReport);
}

gui_Diagnostic::~gui_Diagnostic()
{
    delete ui;
}

void gui_Diagnostic::on_ctrl_saveAsPDF_clicked()
{
    QTextDocument doc;
    QPrinter printer;
    QString filename = QFileDialog::getSaveFileName(  this
                                                    , ("Save PDF")
                                                    , QDir::homePath()
                                                    , ("Documents (*.pdf)") );
    if (!filename.isNull())
    {
        doc.setHtml(m_diagnosticReport);
        doc.setDefaultFont(m_font);
        printer.setOutputFileName(filename);
        printer.setOutputFormat(QPrinter::PdfFormat);

        doc.print(&printer);
        printer.newPage();
    }

}

void gui_Diagnostic::on_ctrl_close_clicked()
{
    this->close();
}

void gui_Diagnostic::done(int result)
{
    ph_prefs().setWindowDiagGeometry(saveGeometry());
    QDialog::done(result);
}
