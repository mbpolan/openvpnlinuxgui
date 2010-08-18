#include <QDesktopServices>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QUrl>

#include "logindialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass) {
    ui->setupUi(this);

    m_Menu=new QMenu(this);
    createActions();

    m_VPN=new VPNHandler(this);
    connect(m_VPN, SIGNAL(needsAuthentication()), this, SLOT(onVPNNeedsAuth()));
    connect(m_VPN, SIGNAL(stateChanged(VPNHandler::State)), this, SLOT(onVPNStateChanged(VPNHandler::State)));
    connect(m_VPN, SIGNAL(logDataReady(QString)), this, SLOT(onVPNLogData(QString)));

    m_Menu->addMenu(m_ProfileMenu);
    m_Menu->addAction(m_DisconnectAct);
    m_Menu->addSeparator();
    m_Menu->addAction(m_OpenConfAct);
    m_Menu->addAction(m_ShowLogAct);
    m_Menu->addSeparator();
    m_Menu->addAction(m_QuitAct);

    m_Icon=new QSystemTrayIcon(this);
    m_Icon->setContextMenu(m_Menu);
    m_Icon->setIcon(QIcon(":/default/icons/connect_no.png"));
    m_Icon->show();

    connect(m_Icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	    this, SLOT(onIconActivated(QSystemTrayIcon::ActivationReason)));

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::createActions() {
    m_ProfileMenu=new QMenu("Connect to", m_Menu);

    // scan for profiles
    QDir dir(QDir::currentPath()+"/conf");
    QStringList filters;
    filters << "*.ovpn";
    dir.setNameFilters(filters);

    m_ProfGroup=new QActionGroup(this);
    m_ProfGroup->setExclusive(true);

    QStringList profs=dir.entryList();
    for (int i=0; i<profs.size(); i++) {
	QString p=profs[i];
	QAction *con=new QAction(p.left(p.indexOf(".")), m_ProfileMenu);
	connect(con, SIGNAL(activated()), this, SLOT(onConnect()));

	con->setCheckable(true);

	m_ProfGroup->addAction(con);

	m_ProfileMenu->addAction(con);
    }

    m_DisconnectAct=new QAction("Disconnect", this);
    m_DisconnectAct->setEnabled(false);
    connect(m_DisconnectAct, SIGNAL(activated()), this, SLOT(onDisconnect()));

    m_OpenConfAct=new QAction("Profiles", this);
    connect(m_OpenConfAct, SIGNAL(activated()), this, SLOT(onOpenConf()));

    m_ShowLogAct=new QAction("View log", this);
    connect(m_ShowLogAct, SIGNAL(activated()), this, SLOT(onShowLog()));

    m_QuitAct=new QAction("Quit", this);
    connect(m_QuitAct, SIGNAL(activated()), this, SLOT(onQuit()));

    m_LogViewer=new LogViewer(this);
}

void MainWindow::onConnect() {
    QString profile="";
    QList<QAction*> acts=m_ProfGroup->actions();
    for (QList<QAction*>::iterator it=acts.begin(); it!=acts.end(); ++it) {
	if ((*it)->isChecked()) {
	    profile=(*it)->text();
	    break;
	}
    }

    m_VPN->connect(profile);
}

void MainWindow::onDisconnect() {
    m_VPN->disconnect();
}

void MainWindow::onOpenConf() {
    QDesktopServices::openUrl(QUrl(QDir::currentPath()+"/conf"));
}

void MainWindow::onShowLog() {
    m_LogViewer->show();
}

void MainWindow::onQuit() {
    m_VPN->disconnect();

    qApp->quit();
}

void MainWindow::onIconActivated(QSystemTrayIcon::ActivationReason act) {
    if (act==QSystemTrayIcon::DoubleClick) {
	if (this->isHidden())
	    show();
	else
	    hide();
    }
}

void MainWindow::onVPNStateChanged(VPNHandler::State state) {
    const int t=3000;

    switch(state) {
	case VPNHandler::Connecting: {
		m_Icon->setIcon(QIcon(":/default/icons/in_progress.png"));
		m_Icon->showMessage("VPN Connection", "Connection in progress...", QSystemTrayIcon::Information, t);

		m_DisconnectAct->setEnabled(true);
		m_ProfileMenu->setEnabled(false);
	} break;

	case VPNHandler::Connected: {
		m_Icon->setIcon(QIcon(":/default/icons/connect_ok.png"));
		m_Icon->showMessage("VPN Connection", "You are now connected!", QSystemTrayIcon::Information, t);
	} break;

	case VPNHandler::Disconnected: {
		m_Icon->setIcon(QIcon(":/default/icons/connect_no.png"));
		m_Icon->showMessage("VPN Connection", "Disconnected from server.", QSystemTrayIcon::Information, t);

		m_DisconnectAct->setEnabled(false);
		m_ProfileMenu->setEnabled(true);
	} break;

	default: break;
    }
}

void MainWindow::onVPNNeedsAuth() {
    LoginDialog ld(this);
    if (ld.exec()==QDialog::Accepted) {
	QPair<QString, QString> p=ld.getAuthData();
	m_VPN->authenticate(p.first, p.second);
    }

    else
	m_VPN->disconnect();
}

void MainWindow::onVPNLogData(QString data) {
    m_LogViewer->append(data);
}
