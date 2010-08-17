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

    m_Proc=NULL;

    m_Menu=new QMenu(this);
    createActions();

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

void MainWindow::onReadError() {
    QString data=m_Proc->readAllStandardError();
    //qDebug() << "ERROR: " << data;
    if (data.indexOf("Enter Auth Username")>-1) {
	LoginDialog ld(this);
	if (ld.exec()) {
	    QPair<QString,QString> auth=ld.getAuthData();
	    m_CachePwd=auth.second;

	    m_Proc->write(auth.first.toAscii()+"\n");
	}

	else {
	    m_Proc->terminate();

	    m_DisconnectAct->setEnabled(false);
	    m_ProfileMenu->setEnabled(true);

	    m_Icon->setIcon(QIcon(":/default/icons/connect_no.png"));
	}
    }

    else if (data.indexOf("Enter Auth Password")>-1)
	m_Proc->write(m_CachePwd.toAscii()+"\n");

    else
	m_LogViewer->append(data);
}

void MainWindow::onReadStdOut() {
    QString data=m_Proc->readAllStandardOutput();

    if (data.indexOf("Initialization Sequence Completed")>-1)
	m_Icon->setIcon(QIcon(":/default/icons/connect_ok.png"));
    else
	m_LogViewer->append(data);
}

void MainWindow::onVpnDone(int, QProcess::ExitStatus) {
    QList<QAction*> acts=m_ProfGroup->actions();
    for (QList<QAction*>::iterator it=acts.begin(); it!=acts.end();++it)
	(*it)->setChecked(false);

    m_Icon->setIcon(QIcon(":/default/icons/connect_no.png"));
}

void MainWindow::onConnect() {
    if (m_Proc)
	delete m_Proc;

    QString profile="";
    QList<QAction*> acts=m_ProfGroup->actions();
    for (QList<QAction*>::iterator it=acts.begin(); it!=acts.end(); ++it) {
	if ((*it)->isChecked()) {
	    profile=(*it)->text();
	    break;
	}
    }

    QString args;
    args="--config "+QDir::currentPath()+"/conf/"+profile+".ovpn";
    args+=" --script-security 3";

    m_Proc=new QProcess(this);
    connect(m_Proc, SIGNAL(readyReadStandardError()), this, SLOT(onReadError()));
    connect(m_Proc, SIGNAL(readyReadStandardOutput()), this, SLOT(onReadStdOut()));
    connect(m_Proc, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onVpnDone(int,QProcess::ExitStatus)));

    m_Icon->setIcon(QIcon(":/default/icons/in_progress.png"));
    m_Proc->start("openvpn "+args);

    m_DisconnectAct->setEnabled(true);
    m_ProfileMenu->setEnabled(false);
}

void MainWindow::onDisconnect() {
    if (m_Proc) {
	m_Proc->terminate();
	m_Proc->waitForFinished(3000);

	m_DisconnectAct->setEnabled(false);
	m_ProfileMenu->setEnabled(true);

	delete m_Proc;
	m_Proc=NULL;
    }
}

void MainWindow::onOpenConf() {
    QDesktopServices::openUrl(QUrl(QDir::currentPath()+"/conf"));
}

void MainWindow::onShowLog() {
    m_LogViewer->show();
}

void MainWindow::onQuit() {
    if (m_Proc && m_Proc->state()==QProcess::Running) {
	m_Proc->terminate();
	m_Proc->waitForFinished(3000);
    }

    qApp->quit();
}
