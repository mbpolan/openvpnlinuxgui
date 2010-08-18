#ifndef VPNHANDLER_H
#define VPNHANDLER_H

#include <QObject>
#include <QProcess>

class VPNHandler: public QObject {
    Q_OBJECT

    public:
	enum State { Connecting=0,
		     WaitingAuth,
		     GettingIP,
		     Connected,
		     Disconnecting,
		     Disconnected };

	VPNHandler(QObject *parent=NULL);
	~VPNHandler();

	// parse output from the openvpn process
	void parseOutput(const QString &str);

	// send authentication details
	void authenticate(const QString &username, const QString &password);

	void connect(const QString &profile);

	// terminate an active connection
	void disconnect();

	// returns the state of the connection
	State state() const { return m_State; }

    private slots:
	void onParseStdOut();
	void onParseStdErr();
	void onProcessFinished(int, QProcess::ExitStatus);

    signals:
	// emitted when the process requires authentication credentials
	void needsAuthentication();

	// emitted when the connection status has changed
	void stateChanged(VPNHandler::State);

	// emitted when data should be logged
	void logDataReady(QString);

    private:
	State m_State;

	QProcess *m_VPNProc;
	QString m_CachePassword;
};

#endif
