#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QAbstractButton>
#include <QDialog>

namespace Ui {
    class LogViewer;
}

class LogViewer: public QDialog {
    Q_OBJECT

    public:
	LogViewer(QWidget *parent=NULL);

	void append(const QString &str);
	void clear();

    private:
	Ui::LogViewer *ui;

    private slots:
	void on_buttonBox_clicked(QAbstractButton* button);
};

#endif // LOGVIEWER_H
