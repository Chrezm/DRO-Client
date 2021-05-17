#ifndef AOPACKET_H
#define AOPACKET_H

#include <QString>
#include <QStringList>

class AOPacket
{
public:
  AOPacket(QString p_packet_string);
  AOPacket(QString header, QStringList &p_contents);

  QString get_header();
  QStringList &get_contents();
  QString to_string();

  void net_encode();
  void net_decode();

private:
  QString m_header;
  QStringList m_contents;
};

#endif // AOPACKET_H
