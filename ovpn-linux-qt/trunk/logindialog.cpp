#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent):
	QDialog(parent), ui(new Ui::LoginDialog) {
    ui->setupUi(this);
}

QPair<QString,QString> LoginDialog::getAuthData() const {
    QPair<QString,QString> p(ui->m_Username->text(), ui->m_Password->text());
    return p;
}
