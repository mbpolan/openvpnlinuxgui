#include <QDir>

#include "vpnhandler.h"

VPNHandler::VPNHandler(QObject *parent): QObject(parent) {
    m_State=Disconnected;
    m_VPNProc=NULL;
    m_CachePassword="";
}

VPNHandler::~VPNHandler() {
    // don't leave the process hanging
    disconnect();
}

void VPNHandler::parseOutput(const QString &data) {
    // emit the authentication signal
    if (data.indexOf("Enter Auth Username")>-1) {
	m_State=WaitingAuth;
	emit needsAuthentication();
    }

    // supply the cached password
    else if (data.indexOf("Enter Auth Password")>-1) {
	m_State=Connecting;
	emit stateChanged(m_State);

	m_VPNProc->write(m_CachePassword.toAscii()+"\n");
    }

    // all done
    else if (data.indexOf("Initialization Sequence Completed")>-1) {
	m_State=Connected;
	emit stateChanged(m_State);
    }

    else
	emit logDataReady(data);
}

void VPNHandler::authenticate(const QString &uname, const QString &pword) {
    m_CachePassword=pword;

    m_VPNProc->write(uname.toAscii()+"\n");
}

void VPNHandler::connect(const QString &profile) {
    // stop if we are already connected
    if (m_State==Connected)
	return;

    // create command string
    QString cmd="openvpn ";
    cmd+="--config "+QDir::currentPath()+"/conf/"+profile+".ovpn ";
    cmd+="--script-security 3";

    // and spawn our process
    m_VPNProc=new QProcess(this);
    QObject::connect(m_VPNProc, SIGNAL(readyReadStandardError()), this, SLOT(onParseStdErr()));
    QObject::connect(m_VPNProc, SIGNAL(readyReadStandardOutput()), this, SLOT(onParseStdOut()));
    QObject::connect(m_VPNProc, SIGNAL(finished(int,QProcess::ExitStatus)),
	    this, SLOT(onProcessFinished(int,QProcess::ExitStatus)));

    m_VPNProc->start(cmd);
}

void VPNHandler::disconnect() {
    if (m_VPNProc && m_VPNProc->state()==QProcess::Running) {
	// flag our new state and emit a signal
	m_State=Disconnecting;
	emit stateChanged(m_State);

	// terminate the connection
	m_VPNProc->terminate();
	if (!m_VPNProc->waitForFinished(3000))
	    m_VPNProc->kill();

	delete m_VPNProc;
	m_VPNProc=NULL;
    }

    // one more state update
    m_State=Disconnected;
    emit stateChanged(m_State);
}

void VPNHandler::onParseStdOut() {
    QString data=m_VPNProc->readAllStandardOutput();
    parseOutput(data);
}

void VPNHandler::onParseStdErr() {
    QString data=m_VPNProc->readAllStandardError();
    parseOutput(data);
}

void VPNHandler::onProcessFinished(int, QProcess::ExitStatus) {
    m_State=Disconnected;
    emit stateChanged(m_State);
}
