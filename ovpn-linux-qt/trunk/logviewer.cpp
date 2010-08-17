#include "logviewer.h"
#include "ui_logviewer.h"

LogViewer::LogViewer(QWidget *parent):
	QDialog(parent), ui(new Ui::LogViewer) {
    ui->setupUi(this);
}

void LogViewer::append(const QString &str) {
    ui->m_LogWindow->appendPlainText(str);
}

void LogViewer::clear() {
    ui->m_LogWindow->clear();
}

void LogViewer::on_buttonBox_clicked(QAbstractButton* button) {
    if (button->text()=="Reset")
	clear();
}
