#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QSystemTrayIcon>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QProcess>

#include "logviewer.h"
#include "vpnhandler.h"

namespace Ui {
    class MainWindowClass;
}

class MainWindow: public QMainWindow {
    Q_OBJECT

    public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

    public slots:
	void onConnect();
	void onDisconnect();
	void onOpenConf();
	void onShowLog();
	void onQuit();

	void onIconActivated(QSystemTrayIcon::ActivationReason);

	void onVPNStateChanged(VPNHandler::State);
	void onVPNNeedsAuth();
	void onVPNLogData(QString);

    private:
	void createActions();

	Ui::MainWindowClass *ui;

	QSystemTrayIcon *m_Icon;
	QMenu *m_Menu;
	QMenu *m_ProfileMenu;

	QActionGroup *m_ProfGroup;
	QAction *m_DisconnectAct;
	QAction *m_OpenConfAct;
	QAction *m_ShowLogAct;
	QAction *m_QuitAct;

	LogViewer *m_LogViewer;
	VPNHandler *m_VPN;
};

#endif // MAINWINDOW_H
