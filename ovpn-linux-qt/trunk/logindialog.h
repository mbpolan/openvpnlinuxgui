#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
    class LoginDialog;
}

class LoginDialog: public QDialog {
    Q_OBJECT

    public:
	LoginDialog(QWidget *parent=NULL);

	QPair<QString,QString> getAuthData() const;

    private:
	Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
