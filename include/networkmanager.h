#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

//#define LOCAL_MS

#ifdef LOCAL_MS
#undef MS_FAILOVER_SUPPORTED
#endif

#include "aoapplication.h"
#include "aopacket.h"

#include <QDnsLookup>
#include <QTcpSocket>
#include <QTime>
#include <QTimer>

class NetworkManager : public QObject
{
  Q_OBJECT

public:
  NetworkManager(AOApplication *parent);
  ~NetworkManager();

  AOApplication *ao_app = nullptr;
  QTcpSocket *ms_socket = nullptr;
  QTcpSocket *server_socket = nullptr;
  QDnsLookup *ms_dns = nullptr;
  QTimer *ms_reconnect_timer = nullptr;

  const QString ms_srv_hostname = "_aoms._tcp.aceattorneyonline.com";
#ifdef LOCAL_MS
  const QString ms_nosrv_hostname = "localhost";
#else
  const QString ms_nosrv_hostname = "master.aceattorneyonline.com";
#endif

  static const int ms_port = 27016;
  static const int timeout_milliseconds = 2000;

  static const int ms_reconnect_delay_ms = 7000;

  bool ms_partial_packet = false;
  QString ms_temp_packet = "";

  bool partial_packet = false;
  QString temp_packet = "";

  unsigned int s_decryptor = 5;

  void connect_to_master();
  void connect_to_master_nosrv();
  void connect_to_server(server_type p_server);
  void disconnect_from_server();

public slots:
  void ship_ms_packet(QString p_packet);
  void ship_server_packet(QString p_packet);

signals:
  void ms_connect_finished(bool success, bool will_retry);

private:
  void perform_srv_lookup();

private slots:
  void on_srv_lookup();
  void handle_ms_packet();
  void handle_server_packet();
  void on_ms_nosrv_connect_success();
  void on_ms_socket_error(QAbstractSocket::SocketError error);
  void retry_ms_connect();
};

#endif // NETWORKMANAGER_H
